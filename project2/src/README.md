# To compile and test code

### Requirements

* GCC
* Intel Pin

### Set PIN environment variable:

* `export PIN_ROOT=YOUR_PIN_DIRECTORY`
* `export PATH=$PATH:YOUR_PIN_DIRECTORY`

### Compile: 
* `make`

### To test: 
* `pin -t ./obj-intel64/cache.so -- /bin/ls`
