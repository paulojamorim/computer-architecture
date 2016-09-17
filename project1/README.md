# To compile and test code

* 1) Copy `syscallcount.cpp` to `YOUR_PIN_DIRECTORY/source/tools/ManualExamples/`

* 2) Run: `make syscallcount.test` into `YOUR_PIN_DIRECTORY/source/tools/ManualExamples/`

* 3) To test pintool: `./pin -t YOUR_PIN_DIRECTORY/source/tools/ManualExamples/obj-intel64/syscallcount.so -- /bin/ls`
