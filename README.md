# single_shot_3D_HGP
Todo: 

1) introduction
2) paper references etc.
3) Software packages we've used
4) Explain input files better
5) Comment demo script

## Setup

### Clone the repo and import submodules

```
git clone https://github.com/MikeVasmer/single_shot_3D_HGP.git
cd single_shot_3D_HGP
git submodule update --init --recursive
```

### Build the demo sim script
The demo sim script can be found at "sim_scripts/single_shot_hgp3d.cpp". To build it, run the following commands from the repository route

```
mkdir build
cd build
cmake ../
make
cd ..
```


## Demo scripts

Demo input files for the 3D surface and toric codes can be found in the "sim_scripts" directory. Run them with the following commands:

### 3D surface code

```
build/single_shot_hgp3d sim_scripts/surface3D_input_demo.json sim_scripts/output
```

### 3D Toric code
```
build/single_shot_hgp3d sim_scripts/toric3D_input_demo.json sim_scripts/output
```

### General sim script syntax

The general syntax for running the sim script is as follows

```
build/single_shot_hgp3d <path_to_input_file.json> <path_to_output_directory>
``` 

### Input file structure
The single_shot_hgp3d.cpp script takes JSON files as input. An example input file is shown below:
```c
{
{
  "exec_file": "sim_scripts/single_shot_hgp3d.cpp",
  "target_runs": 500,
  "code_type": "toric3D",
  "distance_label": 5,
  "code_label": "toric3D_5",
  "hx_filename": "codes/toric3D/toric3D_5_hx.alist",
  "lx_filename": "codes/toric3D/toric3D_5_lx.alist",
  "mx_filename": "codes/toric3D/toric3D_5_mx.alist",
  "lmx_filename": "codes/toric3D/toric3D_5_lmx.alist",
  "mxlmx_filename": "codes/toric3D/toric3D_5_mxlmx.alist",
  "sustained_threshold_depth": 4,
  "input_seed": 6363,
  "bp_method": "product_sum",
  "bp_max_iter": 50,
  "osd_method": "combination_sweep",
  "osd_order": 50,
  "failure_mode_subroutine": true,
  "bit_error_rate": 0.026,
  "syndrome_error_rate": 0.026
}
}
```

Where

  - "exec_file": the location of the sim script that is being run (for future reference)
  - "target_runs": the total number of runs to simulate
  - "sustained_threshold_depth": the number of rounds of noisy error correction to be run per run 
  - "code_type": whatever you like
  - "distance_label": eg. lattice size or the code distance
  - "code_label": whatever you like
  - "hx_filename": alist file for the hx matrix
  - "lx_filename": alist file for the lx matrix
  - "mx_filename": alist file for the mx matrix
  - "lmx_filename": alist file for the lmx matrix
  - "mxlmx_filename": alist file for the mxlmx matrix
  - "input_seed": an integer,
  - "bp_method": the type of BP. Current options are: 1) "product_sum"; 2) "min_sum"; 3) "min_sum_min_synd" 
  - "bp_max_iter": the maximum number of iterations for BP
  - "osd_method": Currenlty three options are available: 1) "combination_sweep"; 2) "exhaustive"; 3) "osd_0". The details of these techniques are described in [arXiv:2005.07016](https://arxiv.org/abs/2005.07016).
  - "osd_order": integer
  - "failure_mode_subroutine": true/false
  - "bit_error_rate": error rate on qubits
  - "syndrome_error_rate": error rate on syndromes

## Licence
MIT License

Copyright (c) 2020 Michael Vasmer & Joschka Roffe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
