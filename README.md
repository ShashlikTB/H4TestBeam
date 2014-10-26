H4TestBeam
==========

Code for H4 test H4TestBeam

    source setupTB.csh
    source setupTB.sh
    
    make -f Makefile_lib

Compile exe

    make 

Remove exe

    make clean
 

To run (example):

    ./bin/H4data2text --input_file blabla.root --output_file test.txt



















OLD
====

Description:

    converter of the spill information from H4DAQ/H4DQM format to TBSpill + TBTree
    
Install:

    cd ..
    git clone   https://github.com/ShashlikTB/T1041.git
    cd T1041
    make 
    cd ../H4TestBeam/
    make
    

To run:

    ./bin/H4data2text --input_file blabla.root --output_file test.txt
    

Where:

    /home/amassiro/Cern/Code/ECAL/TB/H4TestBeam



