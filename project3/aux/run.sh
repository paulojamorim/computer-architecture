#!/bin/bash
. ./shrc
runspec --config=teste --size=test --noreportable astar
runspec --config=teste --size=test --noreportable bzip2
runspec --config=teste --size=test --noreportable gcc
runspec --config=teste --size=test --noreportable gobmk
runspec --config=teste --size=test --noreportable h264ref
runspec --config=teste --size=test --noreportable hmmer
runspec --config=teste --size=test --noreportable mcf
runspec --config=teste --size=test --noreportable omnetpp
runspec --config=teste --size=test --noreportable perlbench
runspec --config=teste --size=test --noreportable sjeng
runspec --config=teste --size=test --noreportable xalancbmk
runspec --config=teste --size=test --noreportable bwaves
runspec --config=teste --size=test --noreportable cactusADM
runspec --config=teste --size=test --noreportable calculix

