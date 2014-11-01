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
 TGTextButton        *fDraw;
 TGTextButton        *fSetNumber;
 TGGroupFrame        *fGframe;
 TGNumberEntry       *fNumber;
 TGLabel             *fLabel;
 
public:
 MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
 virtual ~MyMainFrame();
 void DoSetlabel();
 void DrawPlot();
 
 int _RunNumber;
 int _firstTime_Occupancy;
 
 ClassDef(MyMainFrame, 0)
};

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h) {
 
 _RunNumber = 0;
 _firstTime_Occupancy = 1;
 
 
 fHor1 = new TGHorizontalFrame(this, 400, 400, kFixedWidth);
 fExit = new TGTextButton(fHor1, "&Exit", "gApplication->Terminate(0)");
 fHor1->AddFrame(fExit, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 4, 4, 4, 4));
 
 fDraw = new TGTextButton(fHor1, "Draw Occupancy");
 fDraw->Connect("Clicked()", "MyMainFrame", this, "DrawPlot()");
 fHor1->AddFrame(fDraw, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
 fDraw->SetToolTipText("Click to draw the run DQM plots");
 
 
 AddFrame(fHor1,new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1));
 
 
 
 fNumber = new TGNumberEntry(this, 0, 9,999, TGNumberFormat::kNESInteger,
                             TGNumberFormat::kNEANonNegative, 
                             TGNumberFormat::kNELLimitMinMax,
                             0, 99999);
 fNumber->Connect("ValueSet(Long_t)", "MyMainFrame", this, "DoSetlabel()");
 (fNumber->GetNumberEntry())->Connect("ReturnPressed()", "MyMainFrame", this, "DoSetlabel()");
 
 
 //  AddFrame(fNumber, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));
 AddFrame(fNumber, new TGLayoutHints(kLHintsTop | kLHintsLeft, 1, 1, 1, 1));
 
 
 
 
 
 fGframe = new TGGroupFrame(this, "Run Number"); 
 
 
 fLabel = new TGLabel(fGframe, "No input.");
 fGframe->AddFrame(fLabel, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));
 AddFrame(fGframe, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 1));
//  AddFrame(fGframe, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 1, 1));
 
 
 fSetNumber = new TGTextButton(fGframe, "&SetNumber");
 fSetNumber->Connect("Clicked()", "MyMainFrame", this, "DoSetlabel()");
 fSetNumber->SetToolTipText("Click to set the new run number");
 fGframe->AddFrame(fSetNumber,new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
 

 
 SetCleanup(kDeepCleanup);
 SetWindowName("Number Entry");
 MapSubwindows();
 Resize(GetDefaultSize());
 MapWindow();
 
}

MyMainFrame::~MyMainFrame()
{
 // Destructor.
 
 Cleanup();
}

void MyMainFrame::DrawPlot()
{
 // Slot connected to the Clicked() signal. 
 std::cout << " Draw occupancy plots " << std::endl;
 fDraw->SetState(kButtonDown); 
  
 TString CommandToROOTSize = Form(".x rootscript/calOccupancy.C(\"../DAQ/rec_capture_%d_reco.root\",%d)",_RunNumber, _firstTime_Occupancy);
 gROOT->ProcessLine(CommandToROOTSize);
 
 _firstTime_Occupancy = 0;
 
 fDraw->SetState(kButtonUp);
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








