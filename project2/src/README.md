# To compile and test code

### Requirements

* GCC
* Intel Pintool or [Pinplay](https://software.intel.com/en-us/articles/program-recordreplay-toolkit) to run (-pinplay option)
* [CImg-Lib](http://cimg.eu/) or `sudo apt-get install cimg-dev` (to compile benchmark toy)
* All development was done in Ubuntu 16.04

##### Set PIN environment variable:

* `export PIN_ROOT=YOUR_PIN_OR_PINPLAY_DIRECTORY/`
* `export PATH=$PATH:YOUR_PIN_OR_PINPLAY_DIRECTORY/`
* `sudo apt-get install cimg-dev` (to compile benchmark toy)

### Benchmark toy
* It is a small program that applies mean filter in an image. The amount of consumed memory in accordance with the input image size.
* An image sample is proved.
* To run: `./toy toy_input.bmp output_img.png`

### Compile: 
* `./make_all.sh` (compile all code and dependencies) NOTE: May take a few minutes.

### Run

##### Without pinplay: 
* `run_toy_with_pin.sh`
* -4kb (pages of 4kb)
* -4mb (pages of 4mb)
* -o (output file with result)

##### With pinplay
* Download pinball from [http://snipersim.org/Pinballs](http://snipersim.org/Pinballs). I used this [pinball.](http://snipersim.org/documents/pinballs/cpu2006-pinpoints-w0-d1B-m1.tar)
* `python run_benchmark_pinball.py ./cpu2006_pinballs_folder/ h264` 
* This script save all result same folder. Example h264_4kb.txt and h264_4mb.txt
