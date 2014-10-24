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

    
