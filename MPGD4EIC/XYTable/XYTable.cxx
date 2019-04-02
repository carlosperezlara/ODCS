#include <TApplication.h>
#include <TGFrame.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>
#include <TGTab.h>
#include <TRootEmbeddedCanvas.h>

#include <TSystem.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMath.h>
#include <TTimer.h>
#include <TH2F.h>
#include <TTimeStamp.h>
#include <TLatex.h>
#include <TImage.h>

#include <fstream>
#include <iostream>

#include "XYTable.h"
#include "Velmex.h"
#include "Mitutoyo.h"

ClassImp(XYTable);
const int kMotorX=1;
const int kMotorY=2;
const TString devVelmex="/dev/ttyUSB1110JUNK"; //leave insane values so that
const TString devMitutoyo="/dev/ttyUSB1111JUNK"; //it does not attempt to ping
const bool _TURN_ON_READER_ = true;
const bool _TURN_ON_DRIVER_ = true;
const Double_t kPrecX = 0.1;
const Double_t kPrecY = 0.1;
const Double_t kXYTable_OffX = -6.0+2.5+2.5;
const Double_t kXYTable_OffY = -9.0;
const Double_t kRod = 10;

void XYTable::CreateControl(TGCompositeFrame *mf) {
  TGTab *tabcontainer = new TGTab(mf,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Manual");
  CreateManualControl(tab1);
  TGCompositeFrame *tab2 = tabcontainer->AddTab("Automatic");
  CreateScriptControl(tab2);
  tabcontainer->SetTab(0);
  mf->AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));
}
//====================
void XYTable::CreatePreLoadedTable(TGCompositeFrame *mf) {
  TGCompositeFrame *fFCell[11];
  TGLabel *lab = new TGLabel(mf,"Pre-Loaded Values");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  char cellstr[19] = {'A','L','B','M','C','N','D','O','E','P',
		      'F','Q','G','R','H','S','I','T','J'};
  for(int r=0; r!=10; ++r) {
    fFCell[r] = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
    mf->AddFrame(fFCell[r], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    for(int c=0; c!=19; ++c) {
      TString mycell = Form("%c%d",cellstr[c],9-r);
      fCell[r][c] = new TGTextButton(fFCell[r], c%2==0?mycell.Data():"" );
      fCell[r][c]->Connect("Clicked()", "XYTable", this, Form("ChangeCoordsFromCell(=\"%s\")",mycell.Data()));
      fCell[r][c]->SetToolTipText( Form("set to ( x = %.1f, y = %.1f)",fPreLoaded[9-r][c][0],fPreLoaded[9-r][c][1]) );
      fFCell[r]->AddFrame(fCell[r][c], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
    }
  }
  fFCell[10] = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(fFCell[10], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextButton *fOutOfWay = new TGTextButton(fFCell[10], "out of the beam" );
  fOutOfWay->Connect("Clicked()", "XYTable", this, "SetOutOfWay()");
  fOutOfWay->SetToolTipText( "set to ( x = 0, y = -80)" );
  fFCell[10]->AddFrame(fOutOfWay, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
}
//====================
void XYTable::CreateManualControl(TGCompositeFrame *mf) {
  CreatePreLoadedTable(mf);
  TGCompositeFrame *r1 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  TGCompositeFrame *r2 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(r1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(r2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  TGLabel *labs;
  labs = new TGLabel(r1,"X [mm] :");
  r1->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTXobj = new TGNumberEntry(r1,10,9,9999,
			      TGNumberFormat::kNESRealThree,
			      TGNumberFormat::kNEAAnyNumber,
			      TGNumberFormat::kNELLimitMinMax,-50,+50);
  fGTXobj->Connect("ValueSet(Long_t)", "XYTable", this, "SetObj()");
  (fGTXobj->GetNumberEntry())->Connect("ReturnPressed()", "XYTable", this,"SetObj()");
  r1->AddFrame(fGTXobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(r1,"Y [mm] :");
  r1->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYobj = new TGNumberEntry(r1,10,9,999,
			      TGNumberFormat::kNESRealThree,
			      TGNumberFormat::kNEAAnyNumber,
			      TGNumberFormat::kNELLimitMinMax,-50,+50);
  fGTYobj->Connect("ValueSet(Long_t)", "XYTable", this, "SetObj()");
  (fGTYobj->GetNumberEntry())->Connect("ReturnPressed()", "XYTable", this,"SetObj()");
  r1->AddFrame(fGTYobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  
  fMove = new TGTextButton(r2, "&Move ");
  fReset = new TGTextButton(r2, "&Reset ");
  fCancel = new TGTextButton(r2, "&Cancel ");
  fMove->Connect("Clicked()", "XYTable", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "XYTable", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "XYTable", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  r2->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void XYTable::ChangeCoordsFromCell(const char* rc) {
  // Slot connected to the Clicked() signal.
  char cellstr[19] = {'A','L','B','M','C','N','D','O','E','P',
		      'F','Q','G','R','H','S','I','T','J'};
  TString mycell = rc;
  const char tmpC = mycell[0];
  TString tmpR = mycell[1];
  int row = tmpR.Atoi();
  int col = 0;
  for(int i=0; i!=19; ++i) {
   if(tmpC==cellstr[i])
      col = i;
  }
  //std::cout << tmpC << "(" << row << ", " << col << ")" << std::endl;
  fCell[9-row][col]->SetState(kButtonDown);
  fXobj = fDXobj = fPreLoaded[row][col][0];
  fYobj = fDYobj = fPreLoaded[row][col][1];
  UpdateXYState();
  PrepareMove();
  fCell[9-row][col]->SetState(kButtonUp);
}
//====================
void XYTable::SetOutOfWay() {
  fXobj = fDXobj = 0;
  fYobj = fDYobj = -80;
  PrepareMove();
}
//====================
void XYTable::SetObj() {
  fXobj = fDXobj = fGTXobj->GetNumberEntry()->GetNumber( );
  fYobj = fDYobj = fGTYobj->GetNumberEntry()->GetNumber( );
  PrepareMove();
}
//====================
void XYTable::LoadLogX() {
  fGTXlog->LoadFile(Form("%sPositionX.log",sPath.Data()));
  //std::cout << "  " << Form("%sPositionX.log",sPath.Data()) << std::endl;
  fGTXlog->SetReadOnly(kTRUE);
  fGTXlog->GetText();
  fGTXlog->Goto( fGTXlog->GetText()->RowCount() , 0 );
}
//====================
void XYTable::LoadLogY() {
  fGTYlog->LoadFile(Form("%sPositionY.log",sPath.Data()));
  //std::cout << "  " << Form("%sPositionY.log",sPath.Data()) << std::endl;
  fGTYlog->SetReadOnly(kTRUE);
  fGTYlog->GetText();
  fGTYlog->Goto( fGTYlog->GetText()->RowCount() , 0 );
}
//====================
void XYTable::LoadLogs() {
  LoadLogX();
  LoadLogY();
}
//====================
void XYTable::CreateMotorInspection(TGCompositeFrame *mf) {
  TGLabel *labX = new TGLabel(mf,"X [steps]");
  mf->AddFrame(labX, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  TGCompositeFrame *tExpX = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(tExpX, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGLabel *motorlab = new TGLabel(tExpX,"Motor: ");
  tExpX->AddFrame(motorlab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fMotorX = new TGLabel(tExpX,"0");
  fMotorX->SetBackgroundColor(fPixelBlue);
  fMotorX->SetForegroundColor(fPixelGreen);
  tExpX->AddFrame(fMotorX, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fMotorXLU = new TGLabel(tExpX,"now");
  tExpX->AddFrame(fMotorXLU, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
}
//====================
void XYTable::CreateStatusTextXY(TGCompositeFrame *mf) {
  TGLabel *lab;
  lab = new TGLabel(mf,"X  [mm]");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGCompositeFrame *tExpX = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fGLXnow = new TGLabel(tExpX,"0");
  fGLXnow->SetBackgroundColor(fPixelBlue);
  fGLXnow->SetForegroundColor(fPixelWhite);
  tExpX->AddFrame(fGLXnow, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGLXsta = new TGLabel(tExpX," reached ");
  tExpX->AddFrame(fGLXsta, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(tExpX, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTXlog = new TGTextEdit(mf,102,100);
  mf->AddFrame(fGTXlog, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadLogX();
  //==
  lab = new TGLabel(mf,"Y  [mm]");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGCompositeFrame *tExpY = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fGLYnow = new TGLabel(tExpY,"0");
  fGLYnow->SetBackgroundColor(fPixelBlue);
  fGLYnow->SetForegroundColor(fPixelWhite);
  tExpY->AddFrame(fGLYnow, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGLYsta = new TGLabel(tExpY," reached ");
  tExpY->AddFrame(fGLYsta, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(tExpY, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYlog = new TGTextEdit(mf,102,100);
  mf->AddFrame(fGTYlog, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadLogY();
}
//====================
void XYTable::CreateScriptControl(TGCompositeFrame *mf) {
  TGLabel *lab;
  //--------
  TGCompositeFrame *line0 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line0, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextEdit *fScriptInit = new TGTextEdit(line0,300,110);
  line0->AddFrame(fScriptInit, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  //--------
  /*
  TGCompositeFrame *line1 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line1, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line1,"> Y mm ");
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line1,"from ");
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNYini = new TGNumberEntry(line1,-45,9,999,
					     TGNumberFormat::kNESInteger,
					     TGNumberFormat::kNEAAnyNumber,
					     TGNumberFormat::kNELLimitMinMax,-50,+50);
  line1->AddFrame(fGNYini, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line1," to ");
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNYto = new TGNumberEntry(line1,45,9,999,
					    TGNumberFormat::kNESInteger,
					    TGNumberFormat::kNEAAnyNumber,
					    TGNumberFormat::kNELLimitMinMax,-50,+50);
  line1->AddFrame(fGNYto, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line1," delta ");
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNYinc = new TGNumberEntry(line1,10,9,999,
					    TGNumberFormat::kNESInteger,
					    TGNumberFormat::kNEAAnyNumber,
					    TGNumberFormat::kNELLimitMinMax,-50,+50);
  line1->AddFrame(fGNYinc, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  //--------
  TGCompositeFrame *line2 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line2, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line2,">> X mm ");
  line2->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line2,"from ");
  line2->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNXini = new TGNumberEntry(line2,-45,9,999,
					     TGNumberFormat::kNESInteger,
					     TGNumberFormat::kNEAAnyNumber,
					     TGNumberFormat::kNELLimitMinMax,-50,+50);
  line2->AddFrame(fGNXini, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line2," to ");
  line2->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNXto = new TGNumberEntry(line2,45,9,999,
					    TGNumberFormat::kNESInteger,
					    TGNumberFormat::kNEAAnyNumber,
					    TGNumberFormat::kNELLimitMinMax,-50,+50);
  line2->AddFrame(fGNXto, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line2," delta ");
  line2->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGNumberEntry *fGNXinc = new TGNumberEntry(line2,10,9,999,
					    TGNumberFormat::kNESInteger,
					    TGNumberFormat::kNEAAnyNumber,
					    TGNumberFormat::kNELLimitMinMax,-50,+50);
  line2->AddFrame(fGNXinc, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  */
  //--------
  TGCompositeFrame *line3 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line3, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextEdit *fScriptBody = new TGTextEdit(line3,300,250);
  line3->AddFrame(fScriptBody, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  fScriptBody->LoadFile("./Position_Data/Script.sh");
  fScriptBody->GetText();
  fScriptBody->Goto( fScriptBody->GetText()->RowCount() , 0 );
  fScriptInit->LoadFile("./Position_Data/sequence.txt");
  fScriptInit->GetText();
  fScriptInit->Goto( fScriptInit->GetText()->RowCount() , 0 );
  TGCompositeFrame *line4 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line4, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fMoveS = new TGTextButton(mf, "&Start ");
  fMoveS->Connect("Clicked()", "XYTable", this, "MoveStart()");
  fMoveS->SetToolTipText( "Run in automatic mode" );
  mf->AddFrame(fMoveS, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void XYTable::CreateControlButtons(TGCompositeFrame *mf,TGCompositeFrame *mf2) {
  fMove = new TGTextButton(mf, "&Move ");
  fReset = new TGTextButton(mf, "&Reset ");
  fCancel = new TGTextButton(mf2, "&Cancel ");
  fMove->Connect("Clicked()", "XYTable", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "XYTable", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "XYTable", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  mf->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void XYTable::CreateEyes(TGCompositeFrame *mf) {
  /*
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("canvasimg",mf,400,400,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasImg = new TCanvas("CanvasImg", 10, 10, cId);
  fCanvasImg->SetTopMargin(0.03);
  fCanvasImg->SetBottomMargin(0.08);
  fCanvasImg->SetLeftMargin(0.11);
  fCanvasImg->SetRightMargin(0.02);
  embeddedCanvas->AdoptCanvas(fCanvasImg);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fImg = TImage::Open(Form("%scurrentShot.JPG",sPath.Data()));
  fCanvasImg->SetFixedAspectRatio();
  fImg->SetConstRatio(kFALSE);
  fImg->Draw("N");
  */
  //fCanvasMap->SetEditable(kFALSE);
  fIcon = new TGIcon(mf,Form("%scurrentShot.JPG",sPath.Data()));
  mf->AddFrame(fIcon, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fIcon->Resize(640*0.61,480*0.61);
}
//====================
void XYTable::CreatePlot(TGCompositeFrame *mf) {
  char cellstr[19] = {'A','L','B','M','C','N','D','O','E','P',
		      'F','Q','G','R','H','S','I','T','J'};
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("canvasplot",mf,400,400,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  fCanvasMap->SetTopMargin(0.05);
  fCanvasMap->SetBottomMargin(0.08);
  fCanvasMap->SetLeftMargin(0.11);
  fCanvasMap->SetRightMargin(0.02);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetGridx(1);
  fCanvasMap->SetGridy(1);
  Double_t desp = TMath::Sin(fAngY*TMath::Pi()/180.)*kRod;
  Double_t scale = TMath::Cos(fAngY*TMath::Pi()/180.);
  TH2F *axis = new TH2F("axis",";X  [mm];Y  [mm]",100,-55*scale+desp,+55*scale+desp,100,-55,+55);
  axis->SetStats(0);
  axis->Draw();
  axis->GetXaxis()->SetNdivisions(820);
  axis->GetYaxis()->SetNdivisions(820);
  TLatex *tex = new TLatex();
  tex->SetTextSize(0.05);
  tex->SetTextColor(kGray);
  for(int r=0; r!=10; ++r) 
    for(int c=0; c!=19; ++c)
      if(c%2==0) {
	tex->DrawLatex(fPreLoaded[r][c][0]-3.5*scale,fPreLoaded[r][c][1]-2, Form("%c%d",cellstr[c],r) );
      }
  tex->SetTextSize(0.03);
  tex->SetTextColor(kBlack);
  tex->DrawLatex(-50*scale,+57, Form("at %.1f deg  => desp %.1f",fAngY,desp) );
  Double_t x[1] = {0};
  Double_t y[1] = {0};
  fPointer = new TGraph(1,x,y);
  fPointer->SetMarkerStyle(20);
  fPointer->SetMarkerColor(kBlue-3);
  fPointerObj = new TGraph(1,x,y);
  fPointerObj->SetMarkerStyle(24);
  fPointerObj->SetMarkerColor(kRed-3);
  fPointerObj->SetMarkerSize(2);
  fPointerMust = new TGraph(1,x,y);
  fPointerMust->SetMarkerStyle(24);
  fPointerMust->SetMarkerColor(kGreen-3);
  fPointerMust->SetMarkerSize(2);
  fPointer->Draw("psame");
  fPointerObj->Draw("psame");
  fPointerMust->Draw("psame");
  fCanvasMap->SetEditable(kFALSE);
}
//====================
void XYTable::UpdatePointer() {
  if(fPointer) fPointer->SetPoint(0,fDXnow,fDYnow);
  if(fPointerObj) fPointerObj->SetPoint(0,fDXobj,fDYobj);
  if(fPointerMust) fPointerMust->SetPoint(0,fDXmust,fDYmust);
  if(fCanvasMap) fCanvasMap->Update();
}
//====================
void XYTable::UpdateXYState() {
  if(fGTXobj) fGTXobj->GetNumberEntry()->SetNumber( fDXobj );
  if(fGTYobj) fGTYobj->GetNumberEntry()->SetNumber( fDYobj );
  if(fGLXnow) fGLXnow->SetText( Form("%.3f",fDXnow) );
  if(fGLYnow) fGLYnow->SetText( Form("%.3f",fDYnow) );

  Double_t deltaX1 = fDXmust-fDXobj;
  Double_t deltaX2 = fDXmust-fDXnow;
  Double_t deltaY1 = fDYmust-fDYobj;
  Double_t deltaY2 = fDYmust-fDYnow;
  if( (TMath::Abs(deltaX1)<kPrecX)&&(TMath::Abs(deltaX2)<kPrecX)  ) {
    fGLXsta->SetText( " reached " );
    fGLXsta->SetForegroundColor(fPixelBlack);
    fGTXobj->GetNumberEntry()->SetForegroundColor(fPixelBlack);
  }
  if( (TMath::Abs(deltaY1)<kPrecY)&&(TMath::Abs(deltaY2)<kPrecY)  ) {
    fGLYsta->SetText( " reached " );
    fGLYsta->SetForegroundColor(fPixelBlack);
    fGTYobj->GetNumberEntry()->SetForegroundColor(fPixelBlack);
  }
  UpdatePointer();
}
//====================
void XYTable::PrepareMove() {
  if(fMotor) {
    if(!fMotor->IsReady()) {
      fMove->SetEnabled(kFALSE);
      fCancel->SetEnabled(kTRUE);
      fReset->SetEnabled(kFALSE);
      return;
    }
  }
  fMove->SetEnabled(kFALSE);
  fCancel->SetEnabled(kFALSE);
  fReset->SetEnabled(kFALSE);

  bool dosomething = false;
  Double_t deltaX1 = fDXmust-fDXobj;
  Double_t deltaY1 = fDYmust-fDYobj;
  if( (TMath::Abs(deltaX1)>kPrecX)  ) {
    fGLXsta->SetText( " setting " );
    fGLXsta->SetForegroundColor(fPixelRed);
    fGTXobj->GetNumberEntry()->SetForegroundColor(fPixelRed);
    dosomething = true;
  }
  if( (TMath::Abs(deltaY1)>kPrecY)  ) {
    fGLYsta->SetText( " setting " );
    fGLYsta->SetForegroundColor(fPixelRed);
    fGTYobj->GetNumberEntry()->SetForegroundColor(fPixelRed);
    dosomething = true;
  }
  if(dosomething) {
    fMove->SetBackgroundColor(fPixelGreen);
    fMove->SetEnabled(kTRUE);
    fReset->SetEnabled(kTRUE);
  } else {
    fMove->SetBackgroundColor(fPixelDefaultBgr);
    fMove->SetEnabled(kTRUE);
    fMove->SetEnabled(kFALSE);
  }
}
//====================
void XYTable::ResetXY() {
  fXobj = fXnow = fDXobj = fDXnow;
  fYobj = fYnow = fDYobj = fDYnow;
  PrepareMove();
}
//====================
void XYTable::CancelXY() {
  if(fMotor) {
    fMotor->Abort();
    PrepareMove();
  }
}
//====================
void XYTable::MoveXY() {
  if(fMovingOperation) return;
  fCallReadPositions->TurnOff();
  //std::cout << "CallMoveXY" << std::endl;
  if( (TMath::Abs(fDXmust-fDXobj)<kPrecX) && (TMath::Abs(fDYmust-fDYobj)<kPrecY) ) return;
  //if( (fXmust==fXobj) && (fYmust==fYobj) ) return;
  //std::cout << "Something to do" << std::endl;
  fMove->SetEnabled(kFALSE);
  fReset->SetEnabled(kFALSE);
  fCancel->SetEnabled(kTRUE);
  //fClose->SetEnabled(kFALSE);

  TTimeStamp timestamp;
  TString ts = timestamp.AsString("s");;
  if( TMath::Abs( fDXmust-fDXobj ) > kPrecX ) {
    fXmust = fXobj = fDXmust = fDXobj;
    fGLXsta->SetText( " moving " );
    fGLXsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutX( Form("%sPositionX.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutX << Form("%s => %.3f mm",ts.Data(),fDXmust) << std::endl;
    foutX.close();
    LoadLogX();
    //std::cout << "XXX" << fXmust-fXnow << std::endl;
  }
  if( TMath::Abs( fDYmust-fDYobj ) > kPrecY ) {
    fYmust = fYobj = fDYmust = fDYobj;
    fGLYsta->SetText( " moving " );
    fGLYsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutY( Form("%sPositionY.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutY << Form("%s => %.3f mm",ts.Data(),fDYmust) << std::endl;
    foutY.close();
    LoadLogY();
    //std::cout << "YYY" << fYmust-fYnow << std::endl;
  }
  if(!_TURN_ON_DRIVER_) {
    fXmust = fXobj = fDXmust = fDXnow;
    fYmust = fYobj = fDYmust = fDYnow;
  } else {  
    fMovingOperation = kTRUE;    
    if(fMotor) {
      fMotor->MoveRelative(kMotorX,-1*(fDXmust-fDXnow),kMotorY,fDYmust-fDYnow);
    } else {
      fCallBusy->TurnOn();
    }
  }
  PrepareMove();
  fCallReadPositions->TurnOn();
}
//====================
void XYTable::MoveStart() {
  std::cout << std::endl << "*********************************" << std::endl;
  std::cout <<              "*** Automatic Operation START ***" << std::endl;
  TString nextcell;
  std::ifstream points("./Position_Data/sequence.txt");
  fCells.clear();
  for(;;) {
    points >> nextcell;
    if(!points.good()) break;
    fCells.push_back( nextcell  );
  }
  if(fCells.size()>0) {
    fYouMayFireWhenReady = 0;
    std::cout << "**Moving to " << fCells[fYouMayFireWhenReady] << std::endl;
    ChangeCoordsFromCell(fCells[fYouMayFireWhenReady++].Data());
    MoveXY();
  }
}
//====================
TString XYTable::WhereAmI() {
  TString cell = "A0";
  // Slot connected to the Clicked() signal.
  char cellstr[10] = {'A','B','C','D','E',
		      'F','G','H','I','J'};
  Double_t scale = TMath::Cos(fAngY*TMath::Pi()/180.);
  Double_t desp = TMath::Sin(fAngY*TMath::Pi()/180.)*kRod;
  Double_t dx = fDXnow + 50*scale;
  int yy = (fDYnow + 50)/10;
  int xx = dx/(10*scale)+desp;
  if(xx<0) xx=0;
  if(yy<0) yy=0;
  if(xx>9) xx=9;
  if(yy>9) yy=9;
  //std::cout << xx << " " << yy << std::endl;
  cell = Form("%c%d",cellstr[xx],yy);
  return cell;
}
//====================
void XYTable::ReadBusy() {
  //std::cout << " CALLING"  << std::endl;
  static bool moving = false;
  static int nstep = 0;
  if(!moving) {
    std::cout << std::endl << "Moving operation start" << std::endl;
    //fCallReadPositions->TurnOff();    
    moving = true;
    fMovingOperation = kTRUE;
    nstep = -1;
  }
  //ReadPositions();
  bool doneX = false;
  bool doneY = false;
  if(!fMotor) {
    //std::cout << " I am here" << std::endl;
    int motx1=1, motx2=2, moty1=1, moty2=2, err =0;
    if(nstep==0) {
      err = gSystem->Exec("motor pos > Position_Data/movequery1.tmp");
      if(err>0) return;
      //std::cout << "READ 0" << std::endl;
      nstep = 1;
      return;
    } else if(nstep==1) {
      err = gSystem->Exec("motor pos > Position_Data/movequery2.tmp");
      if(err>0) return;
      //std::cout << "READ 1" << std::endl;
      nstep = 0;
    }
    if(nstep!=-1) {
      std::ifstream finL;
      finL.open( "Position_Data/movequery1.tmp" );
      finL >> motx1;
      if(!finL.good()) return;
      finL >> moty1;
      finL.close();
      //std::cout << " MOTOR1 readed" << std::endl;
      //sleep(1);
      //err = gSystem->Exec(Form("motor pos > %smovequery2.tmp",sPath.Data()));
      //if(err>0) return;
      finL.open( "Position_Data/movequery2.tmp" );
      finL >> motx2;
      if(!finL.good()) return;
      finL >> moty2;
      finL.close();
      //std::cout << " MOTOR2 readed" << std::endl;
    } else { //first call is free
      motx1=motx2=moty1=moty2=0;
      nstep = 0;
    }
    //std::cout << " " << motx1 << " " << motx2 << " " << moty1 << " " << moty2 << std::endl;
    if(motx1!=motx2 || moty1!=moty2) { // still working leave me alone
      //std::cout << "Still working. Leave me alone." << std::endl;
      //std::cout << " " << motx1 << " " << motx2 << " " << moty1 << " " << moty2 << std::endl;
      return;
    } else {
      //std::cout << " " << motx1 << " " << motx2 << " " << moty1 << " " << moty2 << std::endl;
      TString cmd;
      Double_t deltaX = fDXmust-fDXnow;
      if( TMath::Abs(deltaX) > kPrecX  ) {
	// issue a X move command
	std::cout << "  deltaX " << deltaX << "mm" << std::endl;
	cmd = Form("  motor rmove_x %.0f", deltaX*fSPMX );
	std::cout << cmd.Data() << std::endl;
	gSystem->Exec( cmd.Data() );
	return; //and do no more for now
      } else doneX = true;
      Double_t deltaY = fDYmust-fDYnow;
      if( TMath::Abs(deltaY) > kPrecY ) {
	// issue a Y move command
	std::cout << "  deltaY " << deltaY << "mm" << std::endl;
	cmd = Form("  motor rmove_y %.0f", -deltaY*fSPMY );
	std::cout << cmd.Data() << std::endl;
	gSystem->Exec( cmd.Data() );
	return; //and do no more for now
      } else doneY = true;
    }
  }
  
  if(doneX&&doneY) {
    fCallBusy->TurnOff();
    //fCallReadPositions->TurnOn();
    std::cout << "Moving operation completed. Taking picture..." << std::endl;
    gSystem->Exec("motion_cam_parse.pl http://cam0:8081 > Position_Data/currentShot.JPG");
    //fIcon->SetImage("Position_Data/currentShot.JPG");
    //fIcon->Resize(640*0.62,480*0.62);
    std::cout << "Done" << std::endl;
    fMovingOperation = kFALSE;
    moving = false;
    if(fYouMayFireWhenReady>-1) {
      // automatic mode was on
      std::cout << "*** Executing script ... " << std::endl;
      int err = gSystem->Exec("source Position_Data/Script.sh");
      if(err!=0) {
	std::cout << "!!!!!! script failure !!!!!!" << std::endl;
      } else {
	std::cout << "*** Script succesful " << std::endl;
      }
      // run another?
      if(fYouMayFireWhenReady<fCells.size()) {
	std::cout << std::endl << "**Moving to " << fCells[fYouMayFireWhenReady] << std::endl;
	ChangeCoordsFromCell(fCells[fYouMayFireWhenReady++].Data());
	MoveXY();
      } else {
	fYouMayFireWhenReady = -1;
	std::cout << "*** Automatic Operation FINISH ***" << std::endl;
	std::cout << "**********************************" << std::endl << std::endl;
      }
    }
  }
}
//====================
void XYTable::ReadPositions() {
  //HERE WE READ FROM SCALER
  Int_t xmicrons = (fXnow = fXmust = fDXnow = fDXmust)*1000;
  Int_t ymicrons = (fYnow = fYmust = fDYnow = fDYmust)*1000;
  double motx=0;
  double moty=0;
  static bool allGood = true;
  if(_TURN_ON_READER_) {
    //std::cout << "querying enconder..." << std::endl;
    TString fCellNow;
    if(fEncoder) {
      fEncoder->ReadXY(xmicrons,ymicrons);
      fXnow = fDXnow = xmicrons/1000.0 +kXYTable_OffX;
      fYnow = fDYnow = ymicrons/1000.0 +kXYTable_OffY;
    } else {
      std::ifstream finL;
      Int_t err=0;
      err = gSystem->Exec( Form("getpos > %sencquery1.tmp",sPath.Data()) );
      if(err!=0 && !fMovingOperation) {
	std::cout << "COULD NOT READ ENCODER. Please check if it is online!" << std::endl;
	allGood = false;
	return;
      }
      finL.open( Form("%sencquery1.tmp",sPath.Data()));
      finL >> fDXnow >> fDYnow;
      finL.close();
      err = gSystem->Exec( Form("motor pos > %sencquery2.tmp",sPath.Data()) );
      if(err!=0 && !fMovingOperation) {
	std::cout << "COULD NOT READ ENCODER. Please check if it is online!" << std::endl;
	allGood = false;
	return;
      }
      if(!allGood) {
	std::cout << "connection restablished." << std::endl;
	allGood = true;
      }
      finL.open( Form("%sencquery2.tmp",sPath.Data()));
      finL >> motx >> moty;
      finL.close();
      fXnow = fDXnow = fDXnow + kXYTable_OffX;
      fYnow = fDYnow = fDYnow + kXYTable_OffY;
      //std::cout << "I read " << fDXnow << " " << fDYnow << std::endl;
      finL.close();
    }
    //// ********* REMOVE ME AFTER YOU SWTICH THE AXIS
    fXnow = fDXnow = -fDXnow;
    fYnow = fDYnow = -fDYnow;
    //// **********************************************
    static bool first = true;
    if(first){
      //std::cout << "WTH" << std::endl;
      fXmust = fXobj = fDXobj = fDXmust = fDXnow;
      fYmust = fYobj = fDYobj = fDYmust = fDYnow;
      first = false;
    }
    fCellNow = WhereAmI();
    std::ofstream foutL( Form("%sLast.log",sPath.Data()));
    foutL << fCellNow.Data() << " " << fDXnow << " " << fDYnow << " ";
    foutL << motx << " " << moty << " ";
    foutL << fSPMX << " " << fSPMY << " " << fAngY << std::endl;
    foutL.close();
    //std::cout << "done..." << std::endl;
  }
  //std::cout << "Called" << std::endl;
  if(!_TURN_ON_DRIVER_) {
    fXmust = fXobj = fDXmust = fDXnow;
    fYmust = fYobj = fDYmust = fDYnow;
  }
  
    //gSystem->Exec("fswebcam -r 640x480 --jpeg 85 --font \"sans:26\" --timestamp \"%Y-%m-%d %H:%M:%S (%Z)\" -S 10 -q Position_Data/currentShot.JPG");
  UpdateXYState();
}
//====================
void XYTable::CreateTab1(TGCompositeFrame *tthis) {
  TGCompositeFrame *fMainFrames_R1;
  TGCompositeFrame *fMainFrames_R1C1;
  TGCompositeFrame *fMainFrames_R1C2;
  TGCompositeFrame *fMainFrames_R1C3;
  TGCompositeFrame *fMainFrames_R2;
  TGCompositeFrame *fMainFrames_R3;
  TGCompositeFrame *fMainFrames_R4;
  TGCompositeFrame *fMainFrames_R5;
  TGCompositeFrame *fMainFrames_R5C1;
  TGCompositeFrame *fMainFrames_R5C2;

  fMainFrames_R1 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R1C1 = new TGCompositeFrame(fMainFrames_R1, 170, 20, kVerticalFrame);
  fMainFrames_R1C2 = new TGCompositeFrame(fMainFrames_R1, 170, 20, kVerticalFrame);
  fMainFrames_R4 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5C1 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);
  fMainFrames_R5C2 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);

  tthis->AddFrame(fMainFrames_R1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R5, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  fMainFrames_R1C1->ChangeOptions( kRaisedFrame);  
  fMainFrames_R1C2->ChangeOptions( kRaisedFrame);  

  CreateStatusTextXY(fMainFrames_R1C1);
  CreateEyes(fMainFrames_R1C2);

  CreatePlot(fMainFrames_R5C1);
  CreateControl(fMainFrames_R5C2);


}
//====================
void XYTable::CreateTab2(TGCompositeFrame *tthis) {
  /*
  TGCompositeFrame *fMainFrames_R1;
  TGCompositeFrame *fMainFrames_R1C1;
  TGCompositeFrame *fMainFrames_R1C2;
  TGCompositeFrame *fMainFrames_R1C3;
  TGCompositeFrame *fMainFrames_R2;
  TGCompositeFrame *fMainFrames_R3;
  TGCompositeFrame *fMainFrames_R4;
  TGCompositeFrame *fMainFrames_R5;
  TGCompositeFrame *fMainFrames_R5C1;
  TGCompositeFrame *fMainFrames_R5C2;

  fMainFrames_R1 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R1C1 = new TGCompositeFrame(fMainFrames_R1, 170, 20, kVerticalFrame);
  fMainFrames_R1C2 = new TGCompositeFrame(fMainFrames_R1, 170, 20, kVerticalFrame);
  fMainFrames_R1C3 = new TGCompositeFrame(fMainFrames_R1, 170, 20, kVerticalFrame);
  fMainFrames_R2 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R3 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R4 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5C1 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);
  fMainFrames_R5C2 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);

  tthis->AddFrame(fMainFrames_R1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R5, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  fMainFrames_R1C1->ChangeOptions( kRaisedFrame);  
  fMainFrames_R1C2->ChangeOptions( kRaisedFrame);  
  fMainFrames_R5C1->ChangeOptions( kSunkenFrame);  

  CreateMotorInspection(fMainFrames_R1C1);

  CreateEyes(fMainFrames_R1C3);
  */
}
//====================
XYTable::XYTable(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {
  fApp = app;
  sPath = "./Position_Data/";
  fCanvasMap = NULL;
  fCanvasImg = NULL;
  fPointer = NULL;
  fPointerObj = NULL;
  fGTXobj = NULL;
  fGTYobj = NULL;
  fMovingOperation = kFALSE;
  fAngY = 0;
  //fAngY = 45;
  fYouMayFireWhenReady = -1;

  // ==== CONECTING DEVICES
  fMotor = new Velmex(devVelmex.Data());
  fSPMX = 126.032;//126.1402;
  fSPMY = 1259.25;//1259.1736;
  if(fMotor->Error()) {
    std::cout << "Velmex not plugged. Moving to remote control." << std::endl;
    delete fMotor;
    fMotor = NULL;
  } else {
    fMotor->Connect();
    fMotor->SetStepsPerMilimiter(kMotorY,fSPMY);//motorY 1259.1736;
    fMotor->SetStepsPerMilimiter(kMotorX,fSPMX); //motorX 126.1402;
  }
  fEncoder = new Mitutoyo(devMitutoyo.Data());
  if(fEncoder->Error()) {
    std::cout << "Mitutoyo not plugged. Moving to remote control." << std::endl;
    delete fEncoder;
    fEncoder = NULL;
  }

  gClient->GetColorByName("blue", fPixelBlue);
  gClient->GetColorByName("white",fPixelWhite);
  gClient->GetColorByName("red",  fPixelRed);
  gClient->GetColorByName("black",fPixelBlack);
  gClient->GetColorByName("green",fPixelGreen);
  
  fXmust = fYmust = 0;
  std::ifstream finL( Form("%sLast.log",sPath.Data()));
  Double_t xx, yy, trsh, ang;
  TString atmp;
  finL >> atmp >> xx >> yy >> trsh >> trsh >> trsh >> trsh >> ang;
  if(finL.good()) {
    fDXmust = xx;
    fDYmust = yy;
    fAngY = ang;
  }
  fXnow = fXobj = fXmust = fDXnow = fDXobj = fDXmust;
  fYnow = fYobj = fYmust = fDYnow = fDYobj = fDYmust;
  finL.close();
  fLock = kFALSE;
  Double_t desp = TMath::Sin(fAngY*TMath::Pi()/180.)*kRod;
  for(int r=0; r!=10; ++r)
    for(int c=0; c!=19; ++c) {
      fPreLoaded[r][c][0] = (-45 + 5*c)*TMath::Cos(fAngY*TMath::Pi()/180.) + desp;
      fPreLoaded[r][c][1] = -45 + 10*r;
    }
  
  TGTab *tabcontainer = new TGTab(this,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Master Controler");
  CreateTab1(tab1);
  //TGCompositeFrame *tab2 = tabcontainer->AddTab("Calibration");
  //CreateTab2(tab2);
  tabcontainer->SetTab(0);
  AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));

  fPixelDefaultBgr = fMove->GetBackground();

  SetWindowName("Remote Connection to XY-Table");
  MapSubwindows();
  Layout();
  MapWindow();
  Move(0,4000);
  
  ReadPositions();
  fCallReadPositions = new TTimer();
  fCallReadPositions->Connect("Timeout()", "XYTable", this, "ReadPositions()");
  fCallReadPositions->Start(2000, kFALSE);

  fCallBusy = new TTimer();
  fCallBusy->Connect("Timeout()", "XYTable", this, "ReadBusy()");
  fCallBusy->Start(0, kFALSE);
  fCallBusy->TurnOff();
  
  fCaptain = new TTimer();
  //fCaptain->Connect("Timeout()", "XYTable", this, "YouMayFireWhenReady()");
  //fCaptain->Start(500, kFALSE);
  fCaptain->TurnOff();
  
  PrepareMove();
}
//====================
XYTable::~XYTable() {
  Cleanup();
  fCallReadPositions->TurnOff();
  if(fMotor) delete fMotor;
  if(fEncoder) delete fEncoder;
  fApp->Terminate();
}
//====================
void XYTable::Log(TString msg) {
  
}
