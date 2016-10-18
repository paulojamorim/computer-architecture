# To compile and test code

### Requirements

* GCC
* Intel Pintool or [Pinplay](https://software.intel.com/en-us/articles/program-recordreplay-toolkit) to run (-pinplay option)

### Set PIN environment variable:

* `export PIN_ROOT=YOUR_PIN_DIRECTORY`
* `export PATH=$PATH:YOUR_PIN_DIRECTORY`

### Compile: 
* COMPILE PINPLAY (LIST) 
* `make`

### To test without pinplay: 
* `pin -t ./obj-intel64/cache.so -- /bin/ls`

### To test with pinplay
* Download pinball from [http://snipersim.org/Pinballs](http://snipersim.org/Pinballs). I used this [pinball.](http://snipersim.org/documents/pinballs/cpu2006-pinpoints-w0-d1B-m1.tar)
* `python run_benchmark_pinball.py ./cpu2006_pinballs_folder/ h264`
