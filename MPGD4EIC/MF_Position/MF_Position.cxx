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

#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TH2F.h>
#include <TTimeStamp.h>
#include <TLatex.h>

#include <fstream>
#include <iostream>

#include "MF_Position.h"
#include "MC_Velmex.h"
#include "LE_Mitutoyo.h"

ClassImp(MF_Position);

void MF_Position::CreateControl(TGCompositeFrame *mf) {
  TGTab *tabcontainer = new TGTab(mf,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Manual");
  CreateManualControl(tab1);
  TGCompositeFrame *tab2 = tabcontainer->AddTab("Automatic");
  CreateScriptControl(tab2);
  tabcontainer->SetTab(0);
  mf->AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));
}
//====================
void MF_Position::CreatePreLoadedTable(TGCompositeFrame *mf) {
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
      fCell[r][c]->Connect("Clicked()", "MF_Position", this, Form("ChangeCoordsFromCell(=\"%s\")",mycell.Data()));
      fCell[r][c]->SetToolTipText( Form("set to ( x = %d, y = %d)",fPreLoaded[9-r][c][0],fPreLoaded[9-r][c][1]) );
      fFCell[r]->AddFrame(fCell[r][c], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
    }
  }
  fFCell[10] = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(fFCell[10], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextButton *fOutOfWay = new TGTextButton(fFCell[10], "out of the beam" );
  fOutOfWay->Connect("Clicked()", "MF_Position", this, "SetOutOfWay()");
  fOutOfWay->SetToolTipText( "set to ( x = 0, y = -80)" );
  fFCell[10]->AddFrame(fOutOfWay, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
}
//====================
void MF_Position::CreateManualControl(TGCompositeFrame *mf) {
  CreatePreLoadedTable(mf);
  TGCompositeFrame *r1 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  TGCompositeFrame *r2 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(r1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(r2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  TGLabel *labs;
  labs = new TGLabel(r1,"X [mm] :");
  r1->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTXobj = new TGNumberEntry(r1,0,9,9999,
			      TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,0,999);
  fGTXobj->Connect("ValueSet(Long_t)", "MF_Position", this, "SetObj()");
  (fGTXobj->GetNumberEntry())->Connect("ReturnPressed()", "MF_Position", this,"SetObj()");
  r1->AddFrame(fGTXobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(r1,"Y [mm] :");
  r1->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYobj = new TGNumberEntry(r1,0,9,999,
			      TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,0,999);
  fGTYobj->Connect("ValueSet(Long_t)", "MF_Position", this, "SetObj()");
  (fGTYobj->GetNumberEntry())->Connect("ReturnPressed()", "MF_Position", this,"SetObj()");
  r1->AddFrame(fGTYobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  
  fMove = new TGTextButton(r2, "&Move ");
  fReset = new TGTextButton(r2, "&Reset ");
  fCancel = new TGTextButton(r2, "&Cancel ");
  fMove->Connect("Clicked()", "MF_Position", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "MF_Position", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "MF_Position", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  r2->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void MF_Position::ChangeCoordsFromCell(const char* rc) {
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
  std::cout << tmpC << "(" << col << ") " << row << std::endl;
  fCell[9-row][col]->SetState(kButtonDown);
  fXobj = fPreLoaded[row][col][0];
  fYobj = fPreLoaded[row][col][1];
  PrepareMove();
  fCell[9-row][col]->SetState(kButtonUp);
}
//====================
void MF_Position::SetOutOfWay() {
  fXobj = 0;
  fYobj = -80;
  PrepareMove();
}
//====================
void MF_Position::SetObj() {
  fXobj = fGTXobj->GetNumberEntry()->GetNumber( );
  fYobj = fGTYobj->GetNumberEntry()->GetNumber( );
  PrepareMove();
}
//====================
void MF_Position::SetXY() {
  //fXobj = fGTXobj->GetNumberEntry()->GetNumber( );
  //fYobj = fGTYobj->GetNumberEntry()->GetNumber( );
  std::cout << "CLICKED!!!" << std::endl;
  PrepareMove();
}
//====================
void MF_Position::LoadLogX() {
  fGTXlog->LoadFile(Form("%sPositionX.log",sPath.Data()));
  std::cout << "  " << Form("%sPositionX.log",sPath.Data()) << std::endl;
  fGTXlog->SetReadOnly(kTRUE);
  fGTXlog->GetText();
  fGTXlog->Goto( fGTXlog->GetText()->RowCount() , 0 );
}
//====================
void MF_Position::LoadLogY() {
  fGTYlog->LoadFile(Form("%sPositionY.log",sPath.Data()));
  std::cout << "  " << Form("%sPositionY.log",sPath.Data()) << std::endl;
  fGTYlog->SetReadOnly(kTRUE);
  fGTYlog->GetText();
  fGTYlog->Goto( fGTYlog->GetText()->RowCount() , 0 );
}
//====================
void MF_Position::LoadLogs() {
  LoadLogX();
  LoadLogY();
}
//====================
void MF_Position::CreateMotorInspection(TGCompositeFrame *mf) {
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
void MF_Position::ReadRawPositions() {
  TTimeStamp timestamp;
  TString respX = fMotor->GetCurrentPosition(1);
  if(fMotorX&&respX!="") {
    fMotorX->SetText( respX.Data() );
    TString ts = timestamp.AsString("s");
    fMotorXLU->SetText( ts.Data() );
  }
}
//====================
void MF_Position::CreateControlTextX(TGCompositeFrame *mf) {
  TGLabel *lab = new TGLabel(mf,"X  [mm]");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGCompositeFrame *tExpX = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fGLXnow = new TGLabel(tExpX,"0");
  fGLXnow->SetBackgroundColor(fPixelBlue);
  fGLXnow->SetForegroundColor(fPixelGreen);
  tExpX->AddFrame(fGLXnow, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGLXsta = new TGLabel(tExpX," reached ");
  tExpX->AddFrame(fGLXsta, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));


  mf->AddFrame(tExpX, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTXlog = new TGTextEdit(mf,102,236);
  mf->AddFrame(fGTXlog, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadLogX();
}
//====================
void MF_Position::CreateControlTextY(TGCompositeFrame *mf) {
  TGLabel *lab = new TGLabel(mf,"Y  [mm]");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGCompositeFrame *tExpY = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fGLYnow = new TGLabel(tExpY,"0");
  fGLYnow->SetBackgroundColor(fPixelBlue);
  fGLYnow->SetForegroundColor(fPixelGreen);
  tExpY->AddFrame(fGLYnow, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGLYsta = new TGLabel(tExpY," reached ");
  tExpY->AddFrame(fGLYsta, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));


  mf->AddFrame(tExpY, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYlog = new TGTextEdit(mf,102,236);
  mf->AddFrame(fGTYlog, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadLogY();
}
//====================
void MF_Position::CreateScriptControl(TGCompositeFrame *mf) {
  TGLabel *lab;
  //--------
  TGCompositeFrame *line0 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line0, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextEdit *fScriptInit = new TGTextEdit(line0,300,100);
  line0->AddFrame(fScriptInit, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  //--------
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
  //--------
  TGCompositeFrame *line3 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line3, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextEdit *fScriptBody = new TGTextEdit(line3,300,100);
  line3->AddFrame(fScriptBody, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  fScriptBody->LoadFile("./Position_Data/ScriptBody.sh");
  fScriptBody->GetText();
  fScriptBody->Goto( fScriptBody->GetText()->RowCount() , 0 );
  fScriptInit->LoadFile("./Position_Data/ScriptInit.sh");
  fScriptInit->GetText();
  fScriptInit->Goto( fScriptInit->GetText()->RowCount() , 0 );
}
//====================
void MF_Position::CreateControlButtons(TGCompositeFrame *mf,TGCompositeFrame *mf2) {
  fMove = new TGTextButton(mf, "&Move ");
  fReset = new TGTextButton(mf, "&Reset ");
  fCancel = new TGTextButton(mf2, "&Cancel ");
  //fClose = new TGTextButton(mf2, "&Close ","gApplication->Terminate(0)");
  fMove->Connect("Clicked()", "MF_Position", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "MF_Position", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "MF_Position", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  mf->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //mf2->AddFrame(fClose, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void MF_Position::CreateEyes(TGCompositeFrame *mf) {
  fIcon = new TGIcon(mf,Form("%scurrentShot.JPG",sPath.Data()));
  mf->AddFrame(fIcon, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fIcon->Resize(100,300);
}
//====================
void MF_Position::CreatePlot(TGCompositeFrame *mf) {
  char cellstr[19] = {'A','L','B','M','C','N','D','O','E','P',
		      'F','Q','G','R','H','S','I','T','J'};
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas(0,mf,400,400,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  fCanvasMap->SetTopMargin(0.03);
  fCanvasMap->SetBottomMargin(0.08);
  fCanvasMap->SetLeftMargin(0.11);
  fCanvasMap->SetRightMargin(0.02);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetGridx(1);
  fCanvasMap->SetGridy(1);
  //fCanvasMap->SetEditable(kFALSE);
  TH2F *axis = new TH2F("axis",";X  [mm];Y  [mm]",100,-55,+55,100,-55,+55);
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
	tex->DrawLatex(fPreLoaded[r][c][0]-3.5,fPreLoaded[r][c][1]-2, Form("%c%d",cellstr[c],r) );
      }
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
  UpdateXYState();
}
//====================
void MF_Position::UpdatePointer() {
  if(fPointer) fPointer->SetPoint(0,fXnow,fYnow);
  if(fPointerObj) fPointerObj->SetPoint(0,fXobj,fYobj);
  if(fPointerMust) fPointerMust->SetPoint(0,fXmust,fYmust);
  if(fCanvasMap) fCanvasMap->Update();
}
//====================
void MF_Position::UpdateXYState() {
  if(fGTXobj) fGTXobj->GetNumberEntry()->SetNumber( fXobj );
  if(fGTYobj) fGTYobj->GetNumberEntry()->SetNumber( fYobj );
  if(fGLXnow) fGLXnow->SetText( Form("%d",fXnow) );
  if(fGLYnow) fGLYnow->SetText( Form("%d",fYnow) );
  UpdatePointer();
}
//====================
void MF_Position::PrepareMove() {
  if(!fMotor->IsReady()) {
    fMove->SetEnabled(kFALSE);
    fCancel->SetEnabled(kTRUE);
    fReset->SetEnabled(kFALSE);
    //fClose->SetEnabled(kFALSE);
    return;
  }

  fMove->SetEnabled(kFALSE);
  fCancel->SetEnabled(kFALSE);
  fReset->SetEnabled(kFALSE);
  //fClose->SetEnabled(kTRUE);

  bool dosomething = false;
  if(fXobj!=fXmust) {
    fGLXsta->SetText( " setting " );
    fGLXsta->SetForegroundColor(fPixelRed);
    fGTXobj->GetNumberEntry()->SetForegroundColor(fPixelRed);
    dosomething = true;
  }
  if(fYobj!=fYmust) {
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
  UpdateXYState();
}
//====================
void MF_Position::ResetXY() {
  fXobj = fXnow;
  fYobj = fYnow;
  PrepareMove();
}
//====================
void MF_Position::CancelXY() {
  fMotor->Abort();
  PrepareMove();
}
//====================
void MF_Position::MoveXY() {
  fCallReadPositions->TurnOff();
  std::cout << "CallMoveXY" << std::endl;
  if( (fXmust==fXobj) && (fYmust==fYobj) ) return;

  std::cout << "Something to do" << std::endl;
  fMove->SetEnabled(kFALSE);
  fReset->SetEnabled(kFALSE);
  fCancel->SetEnabled(kTRUE);
  //fClose->SetEnabled(kFALSE);

  TTimeStamp timestamp;
  TString ts = timestamp.AsString("s");;
  if(fXmust!=fXobj) {
    fXmust = fXobj;
    fGLXsta->SetText( " moving " );
    fGLXsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutX( Form("%sPositionX.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutX << Form("%s => %d mm",ts.Data(),fXmust) << std::endl;
    foutX.close();
    LoadLogX();
    std::cout << "XXX" << fXmust-fXnow << std::endl;
  }
  if(fYmust!=fYobj) {
    fYmust = fYobj;
    fGLYsta->SetText( " moving " );
    fGLYsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutY( Form("%sPositionY.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutY << Form("%s => %d mm",ts.Data(),fYmust) << std::endl;
    foutY.close();
    LoadLogY();
    std::cout << "YYY" << fYmust-fYnow << std::endl;
  }
  fMotor->MoveRelative(2,-1*(fXmust-fXnow),1,fYmust-fYnow); // motorx is reversed

  PrepareMove();
  fCallReadPositions->TurnOn();
}
//====================
void MF_Position::ReadPositions() {
  //HERE WE READ FROM SCALER
  //std::cout << " ==> ";
  //std::cout << fXnow << " " << fXmust << " " << fXobj << " | ";
  //std::cout << fYnow << " " << fYmust << " " << fYobj << std::endl;
  fXnow = fXmust;
  fYnow = fYmust;
  std::ofstream foutL( Form("%sLast.log",sPath.Data()));
  foutL << fXnow << " " << fYnow << std::endl;
  foutL.close();
  UpdatePointer();
  UpdateXYState();
  //ReadRawPositions();

  if((fXnow==fXmust)&&(fXmust==fXobj)) {
    fGLXsta->SetText( " reached " );
    fGLXsta->SetForegroundColor(fPixelBlack);
    fGTXobj->GetNumberEntry()->SetForegroundColor(fPixelBlack);
  }
  if((fYnow==fYmust)&&(fYmust==fYobj)) {
    fGLYsta->SetText( " reached " );
    fGLYsta->SetForegroundColor(fPixelBlack);
    fGTYobj->GetNumberEntry()->SetForegroundColor(fPixelBlack);
  }
  //std::cout << "called ReadPositions" << std::endl;
}
//====================
void MF_Position::CreateTab1(TGCompositeFrame *tthis) {
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
  fMainFrames_R4 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  fMainFrames_R5C1 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);
  fMainFrames_R5C2 = new TGCompositeFrame(fMainFrames_R5, 170, 20, kVerticalFrame);

  tthis->AddFrame(fMainFrames_R1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R5, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  tthis->AddFrame(fMainFrames_R4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R5->AddFrame(fMainFrames_R5C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  fMainFrames_R1C1->ChangeOptions( kRaisedFrame);  
  fMainFrames_R1C2->ChangeOptions( kRaisedFrame);  

  CreateControlTextX(fMainFrames_R1C1);
  CreateControlTextY(fMainFrames_R1C2);
  CreateEyes(fMainFrames_R1C3);

  CreatePlot(fMainFrames_R5C1);
  CreateControl(fMainFrames_R5C2);


}
//====================
void MF_Position::CreateTab2(TGCompositeFrame *tthis) {
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
}
//====================
MF_Position::MF_Position(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {
  fMotor = new MC_Velmex();
  fMotor->Connect();
  fMotor->SetStepsPerUnit(1,1262);//motorX 1262
  fMotor->SetStepsPerUnit(2,126); //motorY 126
  fApp = app;
  sPath = "./Position_Data/";
  fCanvasMap = NULL;
  fPointer = NULL;
  fPointerObj = NULL;
  fGTXobj = NULL;
  fGTYobj = NULL;
  
  gClient->GetColorByName("blue", fPixelBlue);
  gClient->GetColorByName("red", fPixelRed);
  gClient->GetColorByName("black", fPixelBlack);
  gClient->GetColorByName("green", fPixelGreen);

  for(int r=0; r!=10; ++r)
    for(int c=0; c!=19; ++c) {
      fPreLoaded[r][c][0] = -45 + 5*c;
      fPreLoaded[r][c][1] = -45 + 10*r;
    }
  
  fXmust = fYmust = 0;
  std::ifstream finL( Form("%sLast.log",sPath.Data()));
  int xx, yy;
  finL >> xx >> yy;
  if(finL.good()) {
    fXmust = xx;
    fYmust = yy;
  }
  fXnow = fXobj = fXmust;
  fYnow = fYobj = fYmust;
  finL.close();
  fLock = kFALSE;
  //ReadPositions();

  
  TGTab *tabcontainer = new TGTab(this,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Master Controler");
  CreateTab1(tab1);
  TGCompositeFrame *tab2 = tabcontainer->AddTab("Calibration");
  CreateTab2(tab2);
  tabcontainer->SetTab(0);
  AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));

  fPixelDefaultBgr = fMove->GetBackground();

  SetWindowName("Remote Connection to XY-Table");
  MapSubwindows();
  Layout();
  MapWindow();

  fCallReadPositions = new TTimer();
  fCallReadPositions->Connect("Timeout()", "MF_Position", this, "ReadPositions()");
  fCallReadPositions->Start(1000, kFALSE);

  PrepareMove();
}
//====================
MF_Position::~MF_Position() {
  Cleanup();
  fCallReadPositions->TurnOff();
  if(fMotor) delete fMotor;
  fApp->Terminate();
}
//====================
void MF_Position::Log(TString msg) {
  
}
