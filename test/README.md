test
====


In T1041 do

    cd ../T1041/
    source setupTB.sh
    cd -
    

    
then

    make



To create root file from raw H4DQM

    cd H4DQM
    ./bin/readBinary -f ../data/12.raw  -o test.root
    ./bin/readBinary -f ../H4TestBeam/data/1.raw.fromMarini.raw  -o ../H4TestBeam/data/1.raw.fromMarini.raw.root

    
To create tbspill:

    ./bin/H4data2text --input_file blabla.root --output_file test.txt

    
    
To create Hodoscope text file:

    ./bin/Hodo2text --input_file blabla.root --output_file test_hs.txt
    ./bin/Hodo2text --input_file blabla.root --output_file test_hs.txt --maxEvents 4
