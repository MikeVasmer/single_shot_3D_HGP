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

Demo input files for the 3D surface and toric codes can be found in the "sim_sripts" directory. Run them with the following commands:

### 3D surface code

```
build/single_shot_hgp3d sim_scripts/surface3D_input_demo.json sim_scripts/output
```

### 3D Toric code
```
build/single_shot_hgp3d sim_scripts/toric3D_input_demo.json sim_scripts/output
```

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
