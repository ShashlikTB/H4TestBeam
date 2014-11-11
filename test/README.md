Add Hodoscope information
====

Automatic script to add Hodoscope information

        
    ls ../DAQ/rec_capture_*_reco.root | sed s:../DAQ/rec_capture_:: | sed s/_reco.root// | awk '{print "./bin/AddHodoInfo -s ../DAQ/rec_capture_"$1"_reco.root  -b ../DAQ/hodoscope/"$1"/?.root,../DAQ/hodoscope/"$1"/??.root,../DAQ/hodoscope/"$1"/???.root  -o  ../DAQ/rec_capture_"$1"_reco_beam.root"}'

    
Plot Hodoscope information
====

    ./bin/PlotHodo  -i  ../DAQ/rec_capture_9999.root

    
Plot Hodoscope and Shashlik information
====
    
    ./bin/PlotHodoAndShashlik -i mix_1140_reco.root
    
    
    
test
====

    ./bin/AddHodoInfo -s ../DAQ/rec_capture_1140_reco.root  -b ../DAQ/hodoscope/1140/1.root   -o mix_1140.root


    ./bin/AddHodoInfo -s ../DAQ/rec_capture_1224.root  -b ../DAQ/hodoscope/1224/?.root,../DAQ/hodoscope/1224/??.root,../DAQ/hodoscope/1224/???.root  -o  ../DAQ/rec_capture_9999.root
    
    
    