import os
import sys
import glob
from subprocess import call

def main(spec_folder, spec_benchmark):
    
    specs = glob.glob(spec_folder + "*") 
    benchmark = []

    #get benchmarks dir by name
    for sp in specs:
        if spec_benchmark in sp:

            tmp_prefix = sp + "/" + '/pinball_short.pp/'
            spec_prefix = glob.glob(tmp_prefix + "/*.address")[0]
            spec_prefix = spec_prefix.replace('.address', '')
            benchmark.append([sp, spec_benchmark, spec_prefix])
   
    for b in benchmark:
        for pag in ['-4kb', '-4mb']:
            txt_name = b[0].split("/")[-1]
            command = "pin -xyzzy -reserve_memory " + b[2] + ".address -t " + os.path.abspath("./obj-intel64/cache.so") + " -pinplay -o " + txt_name + "_" + pag.split('-')[-1] + " " + pag + " -replay -replay:basename " + spec_prefix + " -- " + "${PIN_ROOT}/extras/pinplay/bin/intel64/nullapp"
            print "Running command:  ", command
            print "\n"
            call([command], shell=True)
    #print benchmark

if __name__ == "__main__":

    if len(sys.argv) < 3:
        print "Spec2006 pinball folder missing..."
    else:
        
        spec_pinball_folder = sys.argv[1]
        spec_benchmark = sys.argv[2]

        main(spec_pinball_folder, spec_benchmark)
