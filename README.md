H4TestBeam
==========

Code for H4 test H4TestBeam

    source setupTB.csh
    source setupTB.sh
    
    make -f Makefile_lib  clean
    make -f Makefile_lib

Compile exe (the files in "test" folder)

    make 

Remove exe

    make clean
 


To create the Beam information files:

create root file from raw data (if needed, in principle it is done automatically):

    cd H4DQM
    ./bin/readBinary -f ../H4TestBeam/data/1.raw.fromMarini.raw  -o ../H4TestBeam/data/1.raw.fromMarini.raw.root

transform root files into txt files:

    cd H4TestBeam
    ./bin/Hodo2text.exe --input_file data/1.raw.fromMarini.raw.root   --output_file data/test_1_Marini.txt --maxEvents 4

    ./bin/Hodo2text.exe --input_file data/1.raw.fromMarini.raw.root   --output_file data/test_1_Marini.txt --maxEvents 307

    



TBTree
====

To make a tree, do:

    TBTreeMaker.py -P  rec_capture_20141024_195701.txt.bz2
    
    TBTreeMaker.py -P  data/rec_capture_20141023_202536.txt -B data/test_hs.txt

    TBTreeMaker.py -P  data/rec_capture_20141023_202536.txt -B data/test_1_Marini.txt

    TBTreeMaker.py -P  ~/Downloads/rec_capture_20141025_155121.txt  -B data/test_1_Marini.txt

To look at the tree:

    root -l test/rootLogon.C   latest.root
     
To get a visual about which channels are present, do:

    calDisplay.py

    beamDisplay.py










OLD
====

Where:

    /home/amassiro/Cern/Code/ECAL/TB/H4TestBeam



