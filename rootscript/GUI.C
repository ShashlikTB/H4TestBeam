// Created 10/31/2014 A.Massironi: Initial release
// Basic Graphical User Interface for DQM for shifters



#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLayout.h>
#include <TGWindow.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TString.h>


class MyMainFrame : public TGMainFrame {
 
private:
 TGCompositeFrame    *fHor1;
 TGTextButton        *fExit;
 TGTextButton        *fDrawOccupancy;
 TGTextButton        *fDrawDQM;
 TGTextButton        *fDrawBeamPosition;

 TGTextButton        *fSetNumber;
 TGGroupFrame        *fGframe;
 TGNumberEntry       *fNumber;
 TGLabel             *fLabel;
 
public:
 MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
 virtual ~MyMainFrame();
 void DoSetlabel();
 void DrawPlotOccupancy();
 void DrawPlotDQM();
 
 int _RunNumber;
 int _firstTime_Generic;
 int _firstTime_Occupancy;
 int _firstTime_DQM;
 
 ClassDef(MyMainFrame, 0)
};

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h) {
 
 _RunNumber = 0;
 _firstTime_Occupancy = 1;
 _firstTime_DQM = 1;
 _firstTime_Generic = 1;
 
 fHor1 = new TGVerticalFrame(this, 400, 800, kFixedWidth);

 
 
 
 fGframe = new TGGroupFrame(this, "Run Number"); 
 
 
 fLabel = new TGLabel(fGframe, "No input.");
 fGframe->AddFrame(fLabel, new TGLayoutHints(kLHintsTop | kLHintsLeft, 1, 1, 1, 1));
 AddFrame(fGframe, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
 
 
 fSetNumber = new TGTextButton(fGframe, "&SetNumber");
 fSetNumber->Connect("Clicked()", "MyMainFrame", this, "DoSetlabel()");
 fSetNumber->SetToolTipText("Click to set the new run number");
 fGframe->AddFrame(fSetNumber,new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
 

 
 
 
 fNumber = new TGNumberEntry(this, 0, 9,999, TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEANonNegative, 
                             TGNumberFormat::kNELLimitMinMax,
                             0, 99999);
 fNumber->Connect("ValueSet(Long_t)", "MyMainFrame", this, "DoSetlabel()");
 (fNumber->GetNumberEntry())->Connect("ReturnPressed()", "MyMainFrame", this, "DoSetlabel()");
 
 fGframe->AddFrame(fNumber, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
 
 
 
 
 
 
 
 
 
 //---- add DQM buttons
 
 
 fDrawOccupancy = new TGTextButton(fHor1, "Draw Occupancy");
 fDrawOccupancy->Connect("Clicked()", "MyMainFrame", this, "DrawPlotOccupancy()");
 fHor1->AddFrame(fDrawOccupancy, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1));
 fDrawOccupancy->SetToolTipText("Click to draw the run Occupancy plots");
 AddFrame(fHor1,new TGLayoutHints(kLHintsBottom | kLHintsRight, 1, 1, 1, 1));
 
 
 fDrawDQM = new TGTextButton(fHor1, "Draw DQM");
 fDrawDQM->Connect("Clicked()", "MyMainFrame", this, "DrawPlotDQM()");
 fHor1->AddFrame(fDrawDQM, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1));
 fDrawDQM->SetToolTipText("Click to draw the run DQM plots");
 AddFrame(fHor1,new TGLayoutHints(kLHintsBottom | kLHintsRight, 1, 1, 1, 1));
 
 
 fDrawBeamPosition = new TGTextButton(fHor1, "Draw Hodoscope vs Shashlik");
 fDrawBeamPosition->Connect("Clicked()", "MyMainFrame", this, "DrawPlotBeamPosition()");
 fHor1->AddFrame(fDrawBeamPosition, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 1, 1, 1, 1));
 fDrawBeamPosition->SetToolTipText("Click to draw the run BeamPosition plots");
 AddFrame(fHor1,new TGLayoutHints(kLHintsBottom | kLHintsRight, 1, 1, 1, 1));
 
 
 //---- add Exit button
 
 fExit = new TGTextButton(fHor1, "&Exit", "gApplication->Terminate(0)");
 fHor1->AddFrame(fExit, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 1, 1, 1, 1));
 ULong_t red;
 gClient->GetColorByName("red", red);
 fExit->ChangeBackground(red);
 
 
 
 //---- prepare
 
 SetCleanup(kDeepCleanup);
 SetWindowName("Online DQM plots");
 MapSubwindows();
 Resize(GetDefaultSize());
 MapWindow();
 
}

MyMainFrame::~MyMainFrame() {
 // Destructor.
 
 Cleanup();
}


void MyMainFrame::DrawPlotOccupancy() {
 // Slot connected to the Clicked() signal. 
 std::cout << " Draw occupancy plots " << std::endl;
 fDrawOccupancy->SetState(kButtonDown); 
  
 TString CommandToROOTSize = Form(".x rootscript/calOccupancy.C(\"../DAQ/rec_capture_%d_reco.root\",%d)",_RunNumber, _firstTime_Generic);
 gROOT->ProcessLine(CommandToROOTSize);
 
 _firstTime_Generic = 0;
 
 fDrawOccupancy->SetState(kButtonUp);
}


void MyMainFrame::DrawPlotDQM() {
 // Slot connected to the Clicked() signal. 
 std::cout << " Draw DQM plots " << std::endl;
 fDrawDQM->SetState(kButtonDown); 
 
 TString CommandToROOTSize = Form(".x rootscript/calDQM.C(\"../DAQ/rec_capture_%d_reco.root\",%d)",_RunNumber, _firstTime_Generic);
 gROOT->ProcessLine(CommandToROOTSize);
 
 _firstTime_Generic = 0;
 
 fDrawDQM->SetState(kButtonUp);
}





void MyMainFrame::DrawPlotBeamPosition() {
 // Slot connected to the Clicked() signal. 
 std::cout << " Draw BeamPosition plots " << std::endl;
 fDrawBeamPosition->SetState(kButtonDown); 
 
 TString CommandToROOTSize = Form(".x rootscript/beamPosition.C(\"../DAQ/rec_capture_%d_reco.root\",%d)",_RunNumber, _firstTime_Generic);
 gROOT->ProcessLine(CommandToROOTSize);
 
 _firstTime_Generic = 0;
 
 fDrawBeamPosition->SetState(kButtonUp);
}


void MyMainFrame::DoSetlabel()
{
 // Slot method connected to the ValueSet(Long_t) signal.
 // It displays the value set in TGNumberEntry widget.
 
 _RunNumber = fNumber->GetNumberEntry()->GetIntNumber();
 
 fLabel->SetText(Form("%d",_RunNumber));
 
 // Parent frame Layout() method will redraw the label showing the new value.
 fGframe->Layout();
}

void GUI() {
 gSystem->Load("build/lib/libTB.so");  
 new MyMainFrame(gClient->GetRoot(), 50, 50); 
}








