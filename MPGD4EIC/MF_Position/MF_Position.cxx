#include <TApplication.h>
#include <TGFrame.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>
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

ClassImp(MF_Position);

void MF_Position::CreateCellArray(TGCompositeFrame *mf) {
  TGCompositeFrame *fFCell[10];
  TGLabel *lab = new TGLabel(mf,"Pre-Loaded Values");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  char cellstr[10] = {'A','B','C','D','E','F','G','H','I','J'};
  for(int r=0; r!=10; ++r) {
    fFCell[r] = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
    for(int c=0; c!=10; ++c) {
      TString mycell = Form("%c%d",cellstr[c],9-r);
      fCell[r][c] = new TGTextButton(fFCell[r], mycell.Data() );
      fCell[r][c]->Connect("Clicked()", "MF_Position", this, Form("ChangeCoordsFromCell(=\"%s\")",mycell.Data()));
      fCell[r][c]->SetToolTipText( Form("set to ( x = %d, y = %d)",fPreLoaded[9-r][c][0],fPreLoaded[9-r][c][1]) );
      fFCell[r]->AddFrame(fCell[r][c], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
    }
    mf->AddFrame(fFCell[r], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  }
}
//====================
void MF_Position::SetObj() {
  fXobj = fGTXobj->GetNumberEntry()->GetNumber( );
  fYobj = fGTYobj->GetNumberEntry()->GetNumber( );
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
  fGTXobj = new TGNumberEntry(tExpX,0,9,9999,
			      TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,0,999);
  fGTXobj->Connect("ValueSet(Long_t)", "MF_Position", this, "SetObj()");
  (fGTXobj->GetNumberEntry())->Connect("ReturnPressed()", "MF_Position", this,"SetObj()");
  tExpX->AddFrame(fGTXobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
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
  fGTYobj = new TGNumberEntry(tExpY,0,9,999,
			      TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,0,999);
  fGTYobj->Connect("ValueSet(Long_t)", "MF_Position", this, "SetObj()");
  (fGTYobj->GetNumberEntry())->Connect("ReturnPressed()", "MF_Position", this,"SetObj()");
  tExpY->AddFrame(fGTYobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(tExpY, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYlog = new TGTextEdit(mf,102,236);
  mf->AddFrame(fGTYlog, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadLogY();
}
//====================
void MF_Position::CreateControlButtons(TGCompositeFrame *mf,TGCompositeFrame *mf2) {
  fMove = new TGTextButton(mf, "&Move ");
  fReset = new TGTextButton(mf, "&Reset ");
  fCancel = new TGTextButton(mf2, "&Cancel ");
  fClose = new TGTextButton(mf2, "&Close ","gApplication->Terminate(0)");
  fMove->Connect("Clicked()", "MF_Position", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "MF_Position", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "MF_Position", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  mf->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf2->AddFrame(fClose, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  PrepareMove();
}
//====================
void MF_Position::CreateEyes(TGCompositeFrame *mf) {
  char cellstr[10] = {'A','B','C','D','E','F','G','H','I','J'};

  fIcon = new TGIcon(mf,Form("%scurrentShot.JPG",sPath.Data()));
  mf->AddFrame(fIcon, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fIcon->Resize(100,400);
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas(0,mf,400,400,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  fCanvasMap->SetTopMargin(0.03);
  fCanvasMap->SetBottomMargin(0.08);
  fCanvasMap->SetLeftMargin(0.1);
  fCanvasMap->SetRightMargin(0.01);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetGridx(1);
  fCanvasMap->SetGridy(1);
  //fCanvasMap->SetEditable(kFALSE);
  TH2F *axis = new TH2F("axis",";mm;mm",100,-5,+105,100,-5,+105);
  axis->SetStats(0);
  axis->Draw();
  axis->GetXaxis()->SetNdivisions(820);
  axis->GetYaxis()->SetNdivisions(820);
  TLatex *tex = new TLatex();
  tex->SetTextSize(0.05);
  tex->SetTextColor(kGray);
  for(int r=0; r!=10; ++r) 
    for(int c=0; c!=10; ++c) {
      tex->DrawLatex(fPreLoaded[r][c][0]-3.5,fPreLoaded[r][c][1]-2, Form("%c%d",cellstr[c],r) );
    }
  Double_t x[1] = {0};
  Double_t y[1] = {0};
  fPointer = new TGraph(1,x,y);
  fPointer->SetMarkerStyle(20);//4);
  //fPointer->SetMarkerSize(2);
  fPointer->SetMarkerColor(kBlue-3);
  UpdatePointer();
  fPointer->Draw("psame");
}
//====================
void MF_Position::UpdatePointer() {
  if(fPointer) {
    fPointer->SetPoint(0,fXnow,fYnow);
    fCanvasMap->Update();
  }
}
//====================
void MF_Position::UpdateXYState() {
  fGTXobj->GetNumberEntry()->SetNumber( fXobj );
  fGTYobj->GetNumberEntry()->SetNumber( fYobj );
  fGLXnow->SetText( Form("%d",fXnow) );
  fGLYnow->SetText( Form("%d",fYnow) );
}
//====================
void MF_Position::PrepareMove() {
  if(!fMotor->IsReady()) {
    fMove->SetEnabled(kFALSE);
    fCancel->SetEnabled(kTRUE);
    fReset->SetEnabled(kFALSE);
    fClose->SetEnabled(kFALSE);
    return;
  }

  fMove->SetEnabled(kFALSE);
  fCancel->SetEnabled(kFALSE);
  fReset->SetEnabled(kFALSE);
  fClose->SetEnabled(kTRUE);

  bool dosomething = false;
  if(fXobj!=fXmust) {
    fGLXsta->SetText( " setting to " );
    fGLXsta->SetForegroundColor(fPixelRed);
    fGTXobj->GetNumberEntry()->SetForegroundColor(fPixelRed);
    dosomething = true;
  }
  if(fYobj!=fYmust) {
    fGLYsta->SetText( " setting to " );
    fGLYsta->SetForegroundColor(fPixelRed);
    fGTYobj->GetNumberEntry()->SetForegroundColor(fPixelRed);
    dosomething = true;
  }
  if(dosomething) {
    fMove->SetEnabled(kTRUE);
    fReset->SetEnabled(kTRUE);
    fMove->SetBackgroundColor(fPixelGreen);
  }
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
  fClose->SetEnabled(kFALSE);

  TTimeStamp timestamp;
  TString ts = timestamp.AsString("s");;
  if(fXmust!=fXobj) {
    fXmust = fXobj;
    fGLXsta->SetText( " moving to " );
    fGLXsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutX( Form("%sPositionX.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutX << Form("%s => %d mm",ts.Data(),fXmust) << std::endl;
    foutX.close();
    LoadLogX();
    std::cout << "XXX" << fXmust-fXnow << std::endl;
  }
  if(fYmust!=fYobj) {
    fYmust = fYobj;
    fGLYsta->SetText( " moving to " );
    fGLYsta->SetForegroundColor(fPixelGreen);
    std::ofstream foutY( Form("%sPositionY.log",sPath.Data()),std::ofstream::out|std::ofstream::app);
    foutY << Form("%s => %d mm",ts.Data(),fYmust) << std::endl;
    foutY.close();
    LoadLogY();
    std::cout << "YYY" << fYmust-fYnow << std::endl;
  }
  fMotor->MoveRelative(1,fXmust-fXnow,2,fYmust-fYnow);

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
void MF_Position::ChangeCoordsFromCell(const char* rc) {
  // Slot connected to the Clicked() signal.
  char cellstr[10] = {'A','B','C','D','E','F','G','H','I','J'};
  TString mycell = rc;
  const char tmpC = mycell[0];
  TString tmpR = mycell[1];
  int row = tmpR.Atoi();
  int col = 0;
  for(int i=0; i!=10; ++i) {
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
MF_Position::MF_Position(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {
  fMotor = new MC_Velmex();
  fMotor->Connect();
  fMotor->SetStepsPerUnit(1,300);
  fMotor->SetStepsPerUnit(2,30);
  fApp = app;
  sPath = "./Position_Data/";
  fPointer = NULL;
  gClient->GetColorByName("blue", fPixelBlue);
  gClient->GetColorByName("red", fPixelRed);
  gClient->GetColorByName("black", fPixelBlack);
  gClient->GetColorByName("green", fPixelGreen);

  for(int r=0; r!=10; ++r)
    for(int c=0; c!=10; ++c) {
      fPreLoaded[r][c][0] = 5 + 10*c;
      fPreLoaded[r][c][1] = 5 + 10*r;
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
  //ReadPositions();

  TGCompositeFrame *fMainFrames_R1;
  TGCompositeFrame *fMainFrames_R1C1;
  TGCompositeFrame *fMainFrames_R1C2;
  TGCompositeFrame *fMainFrames_R1C3;
  TGCompositeFrame *fMainFrames_R2;
  TGCompositeFrame *fMainFrames_R3;
  TGCompositeFrame *fMainFrames_R4;
  TGCompositeFrame *fMainFrames_R5;

  fMainFrames_R1 =  new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  fMainFrames_R1C1 =  new TGCompositeFrame(this, 170, 20, kVerticalFrame);
  fMainFrames_R1C2 =  new TGCompositeFrame(this, 170, 20, kVerticalFrame);
  fMainFrames_R1C3 =  new TGCompositeFrame(this, 170, 20, kVerticalFrame);
  fMainFrames_R2 =  new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  fMainFrames_R3 =  new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  fMainFrames_R4 =  new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  fMainFrames_R5 =  new TGCompositeFrame(this, 170, 20, kHorizontalFrame);

  AddFrame(fMainFrames_R1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  AddFrame(fMainFrames_R2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  AddFrame(fMainFrames_R3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  AddFrame(fMainFrames_R4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  AddFrame(fMainFrames_R5, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fMainFrames_R1->AddFrame(fMainFrames_R1C3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  fMainFrames_R1C1->ChangeOptions( kRaisedFrame);  
  fMainFrames_R1C2->ChangeOptions( kRaisedFrame);  
  fMainFrames_R1C3->ChangeOptions( kRaisedFrame);  

  CreateControlTextX(fMainFrames_R1C1);
  CreateControlTextY(fMainFrames_R1C2);
  CreateCellArray(fMainFrames_R1C3);
  CreateControlButtons(fMainFrames_R2,fMainFrames_R3);

  CreateEyes(fMainFrames_R5);

  SetWindowName("Position");
  MapSubwindows();
  Layout();
  MapWindow();

  fCallReadPositions = new TTimer();
  fCallReadPositions->Connect("Timeout()", "MF_Position", this, "ReadPositions()");
  fCallReadPositions->Start(2000, kFALSE);
}
//====================
MF_Position::~MF_Position() {
  Cleanup();
  fCallReadPositions->TurnOff();
  if(fMotor) delete fMotor;
  fApp->Terminate();
}
