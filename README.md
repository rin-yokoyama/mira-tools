# mira-tools
Data decoding, pulse fitting, viewer toolset for mira digitizer

## Installation
```
mkdir build; cd build
cmake .. -DGPUFIT_INSTALL_PATH=[path_to_your_Gpufit_path]
make install
```
## decoder
```
decode_mira_dump [dump_file_name]
```
## gpufit
```
gpufit_mira_dump [dump_file_name]
```
## viewer
```
cd viewer
npm install
npm start
```
## dependencies
confirmed working on NVIDIA Jetson Orin Nano
- Ubuntu 20.04 / gcc 9.4.0 / cmake 3.27.7 / node.js 20.9.0 / CUDA 11.4 / ARMv8 rev1 CPU / NVIDIA 229e rev a1 GPU
