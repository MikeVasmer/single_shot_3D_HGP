//
// Single-shot decoding of 3D hypergraph product codes.
//

//header files
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <json.hpp>
#include "timing.h"
#include "json_util.h"

using json = nlohmann::json;
using namespace std;

#include "bp_osd_c.hpp"

int main(int argc, char *argv[])
{

    if(argv[1]==nullptr){
        cout<<"ERROR: Please provide a location for the input directory"<<endl;
        exit(22);
    };

    if(argv[2]==nullptr){
        cout<<"ERROR: Please provide a location for the output directory"<<endl;
        exit(22);
    };

    //timing functions setup
    timing time;
    cout<<"Start time: "<<time.start_time_string<<endl;
    int elapsed_seconds;
    elapsed_seconds=time.elapsed_time_seconds();


    //read in json input file
    cout<<"Input file: "<<argv[1]<<endl;
    ifstream json_input_file(argv[1]);
    json json_input;
    json_input_file>>json_input;
    cout<<json_input.dump(1)<<endl;
    json output=json_input;
    output["input_file"]=argv[1];
    cout<<endl;

    //setup random number generator
    int random_seed = json_read_safe(json_input,"input_seed");
    MTRand r=setup_mtwister_rng(random_seed);

    //save start time
    output["start_time"] = time.start_time_string;

    //generate UI from start time + random seed
    unsigned long long int ui =time.start_time_seconds+random_seed;
    output["ui"]=ui;

    //read in sim parameters
    double bit_error_rate=json_read_safe(json_input,"bit_error_rate");
    double syndrome_error_rate=json_read_safe(json_input,"syndrome_error_rate");
    double sustained_threshold_depth=json_read_safe(json_input,"sustained_threshold_depth");
    int osd_order = json_read_safe(json_input,"osd_order");
    int bp_max_iter=json_read_safe(json_input,"bp_max_iter");
    long long unsigned int target_runs=json_read_safe(json_input,"target_runs");

    //read in OSD method and check that it is valid
    string osd_method = json_read_safe(json_input,"osd_method","osd_0");
    int osd_method_i=-1;
    if(osd_method=="exhaustive") osd_method_i=0;
    else if(osd_method=="combination_sweep") osd_method_i=1;
    else if(osd_method=="osd_0") osd_method_i=2;
    else if(osd_method=="osd_g") osd_method_i=3;
    else cout<<"ERROR: Invalid OSD method"<<endl;


    //read in BP method and check that it is valid
    string bp_method = json_read_safe(json_input,"bp_method","min_sum");
    int bp_method_i=-1;
    if(bp_method=="min_sum") bp_method_i=0;
    else if(bp_method=="min_sum_min_synd") bp_method_i=1;
    else if(bp_method=="product_sum") bp_method_i=2;
    else if(bp_method=="product_sum_min_synd") bp_method_i=3;
    else{
        cout<<"ERROR: Invalid BP method: "<<bp_method<<endl;
        exit(22);
    }
    output["bp_method"]=bp_method;



    //set up output file
    string p_label =to_string(bit_error_rate);
    replace( p_label.begin(), p_label.end(), '.', '_');
    string label =output["code_label"];
    stringstream output_filename;
    output_filename<<argv[2]<<"/"<<label<<";p:"<<p_label<<";ui:"<<ui<<".json";
    cout<<"\nOutput filename: "<< output_filename.str() <<endl;



    //LOAD ALIST FILES
    cout<<"\nLoading alist files"<<endl;
    string temp;

    //Load parity check matrix, hx
    mod2sparse *hx=load_alist_cpp(json_read_safe(json_input,"hx_filename"));
    int hx_m=mod2sparse_rows(hx);
    int hx_n=mod2sparse_cols(hx);
    output["hx_n"]=hx_n;
    output["hx_m"]=hx_m;

    //load logical check matrix, lx
    mod2sparse *lx=load_alist_cpp(json_read_safe(json_input,"lx_filename"));
    int lx_k=mod2sparse_rows(lx);
    int lx_n=mod2sparse_cols(lx);
    output["lx_n"]=lx_n;
    output["lx_k"]=lx_k;

    //load logical check matrix, mx
    mod2sparse *mx=load_alist_cpp(json_read_safe(json_input,"mx_filename"));
    int mx_m=mod2sparse_rows(mx);
    int mx_n=mod2sparse_cols(mx);
    output["mx_n"]=mx_n;
    output["mx_m"]=mx_m;

    bool failure_mode_subroutine=json_read_safe(json_input,"failure_mode_subroutine");
    bool correct_syndrome_logical_failures=true;

    mod2sparse *lmx;
    mod2sparse *mxlmx;
    int lmx_n;
    int lmx_k;
    int mxlmx_n;
    int mxlmx_m;
    if(failure_mode_subroutine==true){

        lmx=load_alist_cpp(json_read_safe(json_input,"lmx_filename"));
        lmx_k=mod2sparse_rows(lmx);
        lmx_n=mod2sparse_cols(lmx);
        output["lmx_n"]=lmx_n;
        output["lmx_k"]=lmx_k;

        mxlmx=load_alist_cpp(json_read_safe(json_input,"mxlmx_filename"));
        mxlmx_m=mod2sparse_rows(mxlmx);
        mxlmx_n=mod2sparse_cols(mxlmx);
        output["mxlmx_n"]=mxlmx_n;
        output["mxlmx_m"]=mxlmx_m;

        assert(mxlmx_m==mx_m+lmx_k);
        assert(mxlmx_n==mx_n);
        assert(mxlmx_n==hx_m);

    }




    //Setup BP+OSD decoder for hx
    bp_osd hx_bp_osd(hx,bit_error_rate,bp_max_iter,osd_order,osd_method_i,bp_method_i);
    output["bp_max_iter"]=hx_bp_osd.max_iter;

    bp_osd mx_bp_osd(mx, syndrome_error_rate, bp_max_iter,osd_order,osd_method_i,bp_method_i);

    bp_osd *mxlmx_bp_osd;

    if(failure_mode_subroutine==true) {
        mxlmx_bp_osd=new bp_osd(mxlmx, syndrome_error_rate, bp_max_iter,osd_order,osd_method_i,bp_method_i);
    }



    //MEMORY ALLOCATION (we can do this now that we know the size of the matrices we are dealing with!)
    char *bit_error_residual=new char[hx_n]();
    char *bit_error=new char[hx_n]();
    char *hx_decoding = new char[hx_n]();
    char *mx_decoding=new char[mx_n]();
    char *hx_syndrome = new char[hx_m]();
    char *hx_syndrome_errored = new char[hx_m]();
    char *mx_syndrome = new char[mx_m]();

    char *mxlmx_syndrome;
    if(failure_mode_subroutine==true) mxlmx_syndrome=new char[mxlmx_m]();

    long long unsigned int osdw_success_count=0;
    long long unsigned int syndrome_decoding_success_count=0;
    bool syndrome_decoding_success;
    double osdw_ler;
    double syndrome_ler;

    //MAIN SIM LOOP
    for(long long unsigned int run_count=1; run_count <= target_runs; run_count++) {

        syndrome_decoding_success=true;
        //clear the residual error
        for(int bit_no=0;bit_no<hx_n;bit_no++) bit_error_residual[bit_no]=0;

        //sustained threshold loop
        for(unsigned int sim_depth=0; sim_depth<=sustained_threshold_depth;sim_depth++) {
            if(sim_depth<sustained_threshold_depth) {

                //generate error on bits
                for (int bit_no = 0; bit_no < hx_n; bit_no++) {
                    bit_error[bit_no] = bit_error_residual[bit_no] ^ (genRand(&r) < bit_error_rate);
                }

                //calculate the syndrome
                syndrome(hx, bit_error, hx_syndrome);

                //apply error to the syndrome
                for (int check_no = 0; check_no < hx_m; check_no++) {
                    hx_syndrome_errored[check_no] = hx_syndrome[check_no] ^ (genRand(&r) < syndrome_error_rate);
                }

                //calculate mx_syndrome
                syndrome(mx,hx_syndrome_errored,mx_syndrome);
                mx_decoding=mx_bp_osd.bp_osd_decode(mx_syndrome);

                //Failure mode subroutine
                if(failure_mode_subroutine==true){
                    if(check_logical_error(lmx,hx_syndrome_errored,mx_decoding)){

                        syndrome_decoding_success=false;

                        if(correct_syndrome_logical_failures) {
                            syndrome(mxlmx, hx_syndrome_errored, mxlmx_syndrome);
                            mx_decoding = mxlmx_bp_osd->bp_osd_decode(mxlmx_syndrome);
                            if (check_logical_error(lmx, hx_syndrome_errored, mx_decoding)) {
                                cout << "ERROR: mxlmx decoding. Something has gone wrong!" << endl;
                                exit(22);
                            }
                            else syndrome_decoding_success=true;
                        }
                        else break; //remocave to see whether this makes a difference
                    }
                }


                //fix the syndrome
                for (int check_no = 0; check_no < hx_m; check_no++) {
                    hx_syndrome[check_no] = hx_syndrome_errored[check_no]^mx_decoding[check_no];
                }

                //decode the main code with the fixed syndrome
                hx_decoding=hx_bp_osd.bp_osd_decode(hx_syndrome);

                //update the residual error
                for (int bit_no = 0; bit_no < hx_n; bit_no++) {
                    bit_error_residual[bit_no] = bit_error[bit_no] ^ hx_decoding[bit_no];
                }



            }
            else if(sim_depth==sustained_threshold_depth){
                //final round of perfect measurement
                for (int bit_no = 0; bit_no < hx_n; bit_no++) {
                    bit_error[bit_no] = bit_error_residual[bit_no] ^ (genRand(&r) < bit_error_rate);
                }

                //calc syndrome
                syndrome(hx, bit_error, hx_syndrome);

                //decode
                hx_decoding=hx_bp_osd.bp_osd_decode(hx_syndrome);

            }
            else{
                cout<<"ERROR. Something has gone wrong!"<<endl;
            }

        } //end of sustained threshold loop


        //check for logical error

        if(syndrome_decoding_success) {
            syndrome_decoding_success_count++;
            if (!check_logical_error(lx, bit_error, hx_decoding)) {
                osdw_success_count++;
            }

        }


        //write to output every 3 seconds. Change this as you see fit
        if((time.elapsed_time_seconds()-elapsed_seconds>3) | (run_count==target_runs) ){

            //calculate logical error rates
            osdw_ler= 1.0 - osdw_success_count / (double(run_count));

            syndrome_ler=1.0 - syndrome_decoding_success_count / (double(run_count));

            elapsed_seconds=time.elapsed_time_seconds();

            output["run_count"] = run_count;
            output["osdw_success_count"]=osdw_success_count;
            output["osdw_ler"] = osdw_ler;
            output["runtime_seconds"] = time.elapsed_time_seconds();
            output["runtime_readable"] = time.elapsed_time_readable();
            output["syndrome_decoding_success_count"]=syndrome_decoding_success_count;
            output["syndrome_ler"]=syndrome_ler;


            //command line output
            cout << "Runs: " << run_count << "; OSDW_LER: " << osdw_ler << "; Runtime: " << time.elapsed_time_readable() << endl;

            //json file output
            ofstream output_file(output_filename.str(),ofstream::trunc);
            output_file<<output.dump(1);
            output_file.close();


        }





    }

    cout<<output.dump(1)<<endl;


}
