void GetBeamInformation(int runNumber) {
 TTree* tree = (TTree*) _file0->Get("t1041");
 
 TTreeFormula* momentum = new TTreeFormula("momentum","tbspill.GetMomentum()",tree);
 
 tree->GetEntry(0);
 float value_momentum = momentum->EvalInstance();

 std::cout << " momentum = " << value_momentum << std::endl;
 
 ofstream myfile;
 myfile.open ("runNumber_momentum.txt",  std::ofstream::app);
 myfile << runNumber << "   " <<  value_momentum << std::endl;
 myfile.close(); 
 
}