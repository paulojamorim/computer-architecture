#!/bin/bash

#------------------------------------------------------------------
# ATTENTION: Copy to "cpu2006/" directory. 
#------------------------------------------------------------------

# to split by "/". 
in="$1";
arr=(${in//// })

#to set file name for each program (take second split)
txt=${arr[2]}

#capture spec commands
commd=" "
space=" "

#join spec commands
for var in "$@"
do
    commd=$commd$space$var
done

#write command before instructions counter
echo $commd >> /home/USR_NAME/Desktop/spec_results/$txt;

#run pintool with spec
/home/USR_NAME/Installations/pin-3.0-76991-gcc-linux/pin -t /home/USR_NAME/Installations/pin-3.0-76991-gcc-linux/source/tools/ManualExamples/obj-intel64/inscount0.so -o /home/USR_NAME/Desktop/spec_results/$txt -- $commd

#skip line
echo " " >> /home/USR_NAME/Desktop/spec_results/$txt;

