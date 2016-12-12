# To compile and test code

### Requirements

* GCC
* Intel Pintool or [Pinplay](https://software.intel.com/en-us/articles/program-recordreplay-toolkit) to run (-pinplay option)
* All development was done in Ubuntu 16.04

##### Set PIN environment variable:

* `export PIN_ROOT=YOUR_PIN_OR_PINPLAY_DIRECTORY/`
* `export PATH=$PATH:YOUR_PIN_OR_PINPLAY_DIRECTORY/`
* `sudo apt-get install cimg-dev` (to compile benchmark toy)

### To run example

* Simple run to test: `runspec --config=teste --size=test --noreportable astar` 
* In particular I created a script to run all benchmarks automatically (aux folder).
* 1 - Copy `teste.cfg` to `cpu2006/config` folder 
* 2 - Copy `run.sh` to `cpu2006` folder and `./run`
* 3 - Results is saved in `size.out` file.
