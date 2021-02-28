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

#include "DAQControl.h"

ClassImp(DAQControl);
const int kMotorX=1;
const int kMotorY=2;
const TString devVelmex="/dev/ttyUSB1110JUNK"; //leave insane values so that
const TString devMitutoyo="/dev/ttyUSB1111JUNK"; //it does not attempt to ping
const bool _TURN_ON_READER_ = true;
const bool _TURN_ON_DRIVER_ = true;
const Double_t kPrecX = 0.1;
const Double_t kPrecY = 0.1;
const Double_t kDAQControl_OffX = -6.0+2.5+2.5;
const Double_t kDAQControl_OffY = -9.0;
const Double_t kRod = 10;

/*
void DAQControl::CreateControl(TGCompositeFrame *mf) {
  TGTab *tabcontainer = new TGTab(mf,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Manual");
  CreateManualControl(tab1);
  TGCompositeFrame *tab2 = tabcontainer->AddTab("Automatic");
  CreateScriptControl(tab2);
  tabcontainer->SetTab(0);
  mf->AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));
}
//====================
void DAQControl::CreatePreLoadedTable(TGCompositeFrame *mf) {
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
      fCell[r][c]->Connect("Clicked()", "DAQControl", this, Form("ChangeCoordsFromCell(=\"%s\")",mycell.Data()));
      fCell[r][c]->SetToolTipText( Form("set to ( x = %.1f, y = %.1f)",fPreLoaded[9-r][c][0],fPreLoaded[9-r][c][1]) );
      fFCell[r]->AddFrame(fCell[r][c], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
    }
  }
  fFCell[10] = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(fFCell[10], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextButton *fOutOfWay = new TGTextButton(fFCell[10], "out of the beam" );
  fOutOfWay->Connect("Clicked()", "DAQControl", this, "SetOutOfWay()");
  fOutOfWay->SetToolTipText( "set to ( x = 0, y = -80)" );
  fFCell[10]->AddFrame(fOutOfWay, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 1, 1, 1, 1));
}
//====================
void DAQControl::CreateManualControl(TGCompositeFrame *mf) {
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
  fGTXobj->Connect("ValueSet(Long_t)", "DAQControl", this, "SetObj()");
  (fGTXobj->GetNumberEntry())->Connect("ReturnPressed()", "DAQControl", this,"SetObj()");
  r1->AddFrame(fGTXobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(r1,"Y [mm] :");
  r1->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fGTYobj = new TGNumberEntry(r1,10,9,999,
			      TGNumberFormat::kNESRealThree,
			      TGNumberFormat::kNEAAnyNumber,
			      TGNumberFormat::kNELLimitMinMax,-50,+50);
  fGTYobj->Connect("ValueSet(Long_t)", "DAQControl", this, "SetObj()");
  (fGTYobj->GetNumberEntry())->Connect("ReturnPressed()", "DAQControl", this,"SetObj()");
  r1->AddFrame(fGTYobj, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  
  fMove = new TGTextButton(r2, "&Move ");
  fReset = new TGTextButton(r2, "&Reset ");
  fCancel = new TGTextButton(r2, "&Cancel ");
  fMove->Connect("Clicked()", "DAQControl", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "DAQControl", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "DAQControl", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  r2->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  r2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void DAQControl::ChangeCoordsFromCell(const char* rc) {
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
void DAQControl::SetOutOfWay() {
  fXobj = fDXobj = 0;
  fYobj = fDYobj = -80;
  PrepareMove();
}
//====================
void DAQControl::SetObj() {
  fXobj = fDXobj = fGTXobj->GetNumberEntry()->GetNumber( );
  fYobj = fDYobj = fGTYobj->GetNumberEntry()->GetNumber( );
  PrepareMove();
}
*/
//====================
void DAQControl::SetV(int i) {
  int ibar = i%4;
  int ihdr = i/4;
  double vbr = fTGSiPM_Vbr[ibar][ihdr]->GetNumberEntry()->GetNumber( );
  double vov = fTGSiPM_OV[ibar][ihdr]->GetNumberEntry()->GetNumber( );
  double volt = vbr + vov;
  //fSiPM_Vbr[ibar][ihdr] = vbr;
  //fSiPM_OV[ibar][ihdr]  = vov;
  //fTGVoltage[ibar][ihdr]->SetText( Form("%.2f V",volt)  );
  fSiPM_Vbr[ibar][0] = vbr;
  fSiPM_OV[ibar][0]  = vov;
  fTGVoltage[ibar][0]->SetText( Form("%.2f V",volt)  );
  fSiPM_Vbr[ibar][1] = vbr;
  fSiPM_OV[ibar][1]  = vov;
  fTGVoltage[ibar][1]->SetText( Form("%.2f V",volt)  );
  fTGSiPM_Vbr[ibar][0]->GetNumberEntry()->SetNumber(vbr);
  fTGSiPM_OV[ibar][0]->GetNumberEntry()->SetNumber(vov);
  fTGSiPM_Vbr[ibar][1]->GetNumberEntry()->SetNumber(vbr);
  fTGSiPM_OV[ibar][1]->GetNumberEntry()->SetNumber(vov);
}
//====================
void DAQControl::Lock() {
  for(int i=0; i!=4; ++i) { // BAR
    fTGAngle[i]->SetState(kFALSE);
    fTGSiPM_Vbr[i][0]->SetState(kFALSE);
    fTGSiPM_OV[i][0]->SetState(kFALSE);
    fTGSiPM_Vbr[i][1]->SetState(kFALSE);
    fTGSiPM_OV[i][1]->SetState(kFALSE);
  }
  fUnlock->SetEnabled(kTRUE);
  fLock->SetEnabled(kFALSE);
  fStartRun->SetEnabled(kTRUE);
  fStopRun->SetEnabled(kFALSE);
}
//====================
void DAQControl::Unlock() {
  for(int i=0; i!=4; ++i) { // BAR
    fTGAngle[i]->SetState(kTRUE);
    fTGSiPM_Vbr[i][0]->SetState(kTRUE);
    fTGSiPM_OV[i][0]->SetState(kTRUE);
    //fTGSiPM_Vbr[i][1]->SetState(kTRUE);
    //fTGSiPM_OV[i][1]->SetState(kTRUE);
  }
  fUnlock->SetEnabled(kFALSE);
  fLock->SetEnabled(kTRUE);
  fStartRun->SetEnabled(kFALSE);
  fStopRun->SetEnabled(kFALSE);
}
//====================
void DAQControl::StartRun() {
  fStartRun->SetEnabled(kFALSE);
  //KickRun();
  fStopRun->SetEnabled(kTRUE);
}
//====================
void DAQControl::StopRun() {
  //WrapUp()
  SaveConfigFile();
  LoadConfigFile();
  Unlock();
}
//====================
void DAQControl::SetAngle(int i) {
  fAngle[i] = fTGAngle[i]->GetNumberEntry()->GetNumber( );

  double x0 = (i+1)*10 - 2.6 * sin(fAngle[i]*TMath::Pi()/180);
  double x1 = (i+1)*10 + 2.6 * sin(fAngle[i]*TMath::Pi()/180);
  double y0 = -2.6 * cos(fAngle[i]*TMath::Pi()/180);
  double y1 = +2.6 * cos(fAngle[i]*TMath::Pi()/180);
  fBarDrawing[i]->SetPoint(0,x0,y0);
  fBarDrawing[i]->SetPoint(1,x1,y1);
  if(fCanvasMap) fCanvasMap->Update();
}
//====================
void DAQControl::ShowConfigFile() {
  if(!fConfigFile) return;
  TString lastrun = Form("%sConfigFile.txt",sPath.Data());
  fConfigFile->LoadFile( lastrun.Data() );
  fConfigFile->SetReadOnly(kTRUE);
  fConfigFile->GetText();
  fConfigFile->Goto( fConfigFile->GetText()->RowCount() , 0 );
}
//====================
void DAQControl::LoadConfigFile() {
  TString lastrun = Form("%sConfigFile.txt",sPath.Data());
  std::ifstream fin(lastrun.Data());
  fin >> fRunNumber;
  for(int i=0; i!=4; ++i)
    fin >> fAngle[i];
  for(int j=0; j!=2; ++j) {
    for(int i=0; i!=4; ++i)
      fin >> fSiPM_Vbr[i][j] >> fSiPM_OV[i][j];
  }
  TString tmp;
  for(int j=0; j!=2; ++j)
    for(int i=0; i!=4; ++i) {
      fin >> tmp;
      fTermistor[i][j] = tmp;
      fin >> tmp;
      fTermistor[i][j] += " " + tmp;
    }
  for(int j=0; j!=2; ++j)
    for(int i=0; i!=4; ++i) {
      fin >> tmp;
      fCurrent[i][j] = tmp;
      fin >> tmp;
      fCurrent[i][j] += " " + tmp;
    }
  ShowConfigFile();
  if(fLabRunNumber) fLabRunNumber->SetText( Form("%05d",fRunNumber+1) );
}
//====================
void DAQControl::SaveConfigFile() {
  TString lastrun = Form("%sConfigFile.txt",sPath.Data());
  std::ofstream fout(lastrun.Data());
  fout << Form("%05d",fRunNumber+1) << std::endl;
  for(int i=0; i!=4; ++i)
    fout << Form("    %3.1f      ",fAngle[i]);
  fout << std::endl;
  for(int j=0; j!=2; ++j) {
    for(int i=0; i!=4; ++i) 
      fout << Form("%2.2f  %2.2f   ",fSiPM_Vbr[i][j],fSiPM_OV[i][j]);
    fout << std::endl;
  }
  for(int j=0; j!=2; ++j) {
    for(int i=0; i!=4; ++i) {
      //TString temp = fTGTermistor[i][j]->GetText()->GetString();
      fout << Form(" %s    ",fTermistor[i][j].Data());
    }
    fout << std::endl;
  }
  for(int j=0; j!=2; ++j) {
    for(int i=0; i!=4; ++i) {
      //TString curr = fTGCurrent[i][j]->GetText()->GetString();
      fout << Form(" %s    ",fCurrent[i][j].Data());
    }
    fout << std::endl;
  }
  fout << std::endl;
  fout.close();
}
/*
//====================
void DAQControl::CreateMotorInspection(TGCompositeFrame *mf) {
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
void DAQControl::CreateStatusTextXY(TGCompositeFrame *mf) {
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
*/
/*
//====================
void DAQControl::CreateScriptControl(TGCompositeFrame *mf) {
  TGLabel *lab;
  //--------
  TGCompositeFrame *line0 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  mf->AddFrame(line0, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGTextEdit *fScriptInit = new TGTextEdit(line0,300,110);
  line0->AddFrame(fScriptInit, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  //--------
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
  fMoveS->Connect("Clicked()", "DAQControl", this, "MoveStart()");
  fMoveS->SetToolTipText( "Run in automatic mode" );
  mf->AddFrame(fMoveS, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void DAQControl::CreateControlButtons(TGCompositeFrame *mf,TGCompositeFrame *mf2) {
  fMove = new TGTextButton(mf, "&Move ");
  fReset = new TGTextButton(mf, "&Reset ");
  fCancel = new TGTextButton(mf2, "&Cancel ");
  fMove->Connect("Clicked()", "DAQControl", this, "MoveXY()");
  fMove->SetToolTipText( "Execute move to new coordinates" );
  fReset->Connect("Clicked()", "DAQControl", this, "ResetXY()");
  fReset->SetToolTipText( "Reset to previous position" );
  fCancel->Connect("Clicked()", "DAQControl", this, "CancelXY()");
  fCancel->SetToolTipText( "Abort movement and stays wherever it stops" );
  mf->AddFrame(fMove, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(fReset, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf2->AddFrame(fCancel, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
*/
//====================
void DAQControl::CreateEyes(TGCompositeFrame *mf) {
  for(int i=0; i!=4; ++i) {
    fIcon[i] = new TGIcon(mf,Form("%scurrentShot%d.JPG",sPath.Data(),i));
    mf->AddFrame(fIcon[i], new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
    fIcon[i]->Resize(640*0.61,480*0.61*0.5);
  }
}
//====================
void DAQControl::CreateVoltageControl(TGCompositeFrame *mf, Int_t i, Bool_t up) {
  TGLabel *lab;

  TGCompositeFrame *line2 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  lab = new TGLabel(line2,"Voltage:");
  line2->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fTGVoltage[i%4][i/4] = new TGLabel(line2,Form("%.2f V",fSiPM_Vbr[i%4][i/4]+fSiPM_OV[i%4][i/4]));
  line2->AddFrame(fTGVoltage[i%4][i/4], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  if(up) {
    mf->AddFrame(line2, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  }
  
  TGCompositeFrame *line0 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  lab = new TGLabel(line0,Form("Bar%d H%d",i%4,i/4));
  line0->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line0,"V_bdown:");
  line0->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fTGSiPM_Vbr[i%4][i/4] = new TGNumberEntry(line0,fSiPM_Vbr[i%4][i/4],0,50,
					    TGNumberFormat::kNESRealTwo,
					    TGNumberFormat::kNEAAnyNumber,
					    TGNumberFormat::kNELLimitMinMax,0,+50);
  fTGSiPM_Vbr[i%4][i/4]->Connect("ValueSet(Double_t)", "DAQControl", this, Form("SetV(=%d)",i) );
  (fTGSiPM_Vbr[i%4][i/4]->GetNumberEntry())->Connect("ReturnPressed()", "DAQControl", this, Form("SetV(=%d)",i) );
  //entry->SetBackgroundColor(fPixelBlue);
  //entry->SetForegroundColor(fPixelGreen); 
  line0->AddFrame(fTGSiPM_Vbr[i%4][i/4], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(line0, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  TGCompositeFrame *line1 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  lab = new TGLabel(line1,Form("Bar%d H%d", i%4,i/4));
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  lab = new TGLabel(line1,"OverVolt:");
  line1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fTGSiPM_OV[i%4][i/4] = new TGNumberEntry(line1,fSiPM_OV[i%4][i/4],0,10,
					   TGNumberFormat::kNESRealTwo,
					   TGNumberFormat::kNEAAnyNumber,
					   TGNumberFormat::kNELLimitMinMax,0,+10);
  fTGSiPM_OV[i%4][i/4]->Connect("ValueSet(Double_t)", "DAQControl", this, Form("SetV(=%d)",i) );
  (fTGSiPM_OV[i%4][i/4]->GetNumberEntry())->Connect("ReturnPressed()", "DAQControl", this, Form("SetV(=%d)",i) );
  line1->AddFrame(fTGSiPM_OV[i%4][i/4], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(line1, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));

  if(!up) {
    mf->AddFrame(line2, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  }
  
}
//====================
void DAQControl::CreateVoltageControler(TGCompositeFrame *mf) {
  TGCompositeFrame *line0 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  for(int i=0; i!=4; ++i) {
    TGCompositeFrame *formA = new TGCompositeFrame(line0, 170, 20, kVerticalFrame);
    CreateVoltageControl(formA,i,false);
    //formA->ChangeOptions( kRaisedFrame);
    formA->ChangeOptions( kDoubleBorder );
    line0->AddFrame(formA, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  }
  mf->AddFrame(line0, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  TGCompositeFrame *line1 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  CreatePlot(line1);
  mf->AddFrame(line1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  
  TGCompositeFrame *line3 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  for(int i=4; i!=8; ++i) {
    TGCompositeFrame *formA = new TGCompositeFrame(line3, 170, 20, kVerticalFrame);
    CreateVoltageControl(formA,i);
    //formA->ChangeOptions( kRaisedFrame);
    formA->ChangeOptions( kDoubleBorder );
    line3->AddFrame(formA, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  }
  mf->AddFrame(line3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));

  TGCompositeFrame *line4 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fLock = new TGTextButton(line4, "&Set & Lock");
  fUnlock = new TGTextButton(line4, "&Unlock");
  fLock->Connect("Clicked()", "DAQControl", this, "Lock()");
  fLock->SetToolTipText( "Set voltages and lock" );
  fUnlock->Connect("Clicked()", "DAQControl", this, "Unlock()");
  fUnlock->SetToolTipText( "Unlock" );
  line4->AddFrame(fUnlock, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  line4->AddFrame(fLock, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(line4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  
}
//====================
void DAQControl::CreateRunControl(TGCompositeFrame *mf) {
  TGLabel *lab;
  TGCompositeFrame *line4 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  lab = new TGLabel(line4,"Run Number:");
  line4->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fLabRunNumber = new TGLabel(line4,Form("%05d",fRunNumber+1));
  line4->AddFrame(fLabRunNumber, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  mf->AddFrame(line4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  fStartRun = new TGTextButton(mf, "Start &Run");
  fStopRun = new TGTextButton(mf, "&STOP");
  fStartRun->Connect("Clicked()", "DAQControl", this, "StartRun()");
  fStartRun->SetToolTipText( "Start Run" );
  fStopRun->Connect("Clicked()", "DAQControl", this, "StopRun()");
  fStopRun->SetToolTipText( "Stop Run" );
  mf->AddFrame(fStartRun, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  mf->AddFrame(fStopRun, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
void DAQControl::CreateConfigFile(TGCompositeFrame *mf) {
  TGLabel *lab;
  lab = new TGLabel(mf,"Last ConfigFile");
  mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TGCompositeFrame *tExpX = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  fConfigFile = new TGTextEdit(mf,102,150);
  fConfigFile->SetEditDisabled();
  fConfigFile->SetEditable(kFALSE);
  mf->AddFrame(fConfigFile, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  LoadConfigFile();
}
//====================
void DAQControl::CreateTermistor(TGCompositeFrame *mf) {
  TGLabel *lab;
  TGCompositeFrame *line3 = new TGCompositeFrame(mf, 170, 20, kHorizontalFrame);
  for(int i=0; i!=4; ++i) {
    TGCompositeFrame *col3 = new TGCompositeFrame(line3, 170, 20, kVerticalFrame);
    col3->ChangeOptions( kRaisedFrame);
    for(int j=0; j!=2; ++j) {
      TGCompositeFrame *subcol3 = new TGCompositeFrame(col3, 170, 20, kHorizontalFrame);
      lab = new TGLabel(subcol3, Form("Bar%d H%d:",i,j) );
      subcol3->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
      fTGTermistor[i][j] = new TGLabel(subcol3, fTermistor[i][j].Data() );
      subcol3->AddFrame(fTGTermistor[i][j], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
      fTGCurrent[i][j] = new TGLabel(subcol3, fCurrent[i][j].Data() );
      subcol3->AddFrame(fTGCurrent[i][j], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
      col3->AddFrame(subcol3, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    }
    line3->AddFrame(col3, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  }
  mf->AddFrame(line3, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
}
//====================
void DAQControl::CreateAngle(TGCompositeFrame *mf) {
  TGLabel *lab;
  for(int i=0; i!=4; ++i) {
    lab = new TGLabel(mf,Form("Angle%d:",i%4));
    mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    fTGAngle[i] = new TGNumberEntry(mf,fAngle[i],0,80,
				    TGNumberFormat::kNESRealOne,
				    TGNumberFormat::kNEAAnyNumber,
				    TGNumberFormat::kNELLimitMinMax,0,+80);
    fTGAngle[i]->Connect("ValueSet(Double_t)", "DAQControl", this, Form("SetAngle(=%d)",i) );
    (fTGAngle[i]->GetNumberEntry())->Connect("ReturnPressed()", "DAQControl", this, Form("SetAngle(=%d)",i) );
    mf->AddFrame(fTGAngle[i], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    lab = new TGLabel(mf,"deg");
    mf->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  }
}
//====================
void DAQControl::CreatePlot(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("canvasplot",mf,1020,100,kDoubleBorder);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  fCanvasMap->SetTopMargin(0.05);
  fCanvasMap->SetBottomMargin(0.08);
  fCanvasMap->SetLeftMargin(0.02);
  fCanvasMap->SetRightMargin(0.02);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetGridx(1);
  fCanvasMap->SetGridy(1);
  TH2F *axis = new TH2F("axis",";Y  [mm];X  [mm]",100,5,+45,100,-3,+3);
  axis->SetStats(0);
  axis->Draw();
  axis->GetXaxis()->SetNdivisions(110);
  axis->GetYaxis()->SetNdivisions(102);
  Double_t x[4][2];
  Double_t y[4][2];
  for(int i=0; i!=4; ++i) {
    x[i][0] = (i+1)*10 - 2.6 * sin(fAngle[i]*TMath::Pi()/180);
    x[i][1] = (i+1)*10 + 2.6 * sin(fAngle[i]*TMath::Pi()/180);
    y[i][0] = -2.6 * cos(fAngle[i]*TMath::Pi()/180);
    y[i][1] = +2.6 * cos(fAngle[i]*TMath::Pi()/180);
    fBarDrawing[i] = new TGraph(2,x[i],y[i]);
    fBarDrawing[i]->SetLineWidth(4);
    fBarDrawing[i]->SetLineColor( fBarColor[i] );
    fBarDrawing[i]->Draw("lsame");
  }
  fCanvasMap->SetEditable(kFALSE);
}
/*
//====================
void DAQControl::UpdatePointer() {
  if(fPointer) fPointer->SetPoint(0,fDXnow,fDYnow);
  if(fPointerObj) fPointerObj->SetPoint(0,fDXobj,fDYobj);
  if(fPointerMust) fPointerMust->SetPoint(0,fDXmust,fDYmust);
  if(fCanvasMap) fCanvasMap->Update();
}
//====================
void DAQControl::UpdateXYState() {
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
void DAQControl::PrepareMove() {
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
void DAQControl::ResetXY() {
  fXobj = fXnow = fDXobj = fDXnow;
  fYobj = fYnow = fDYobj = fDYnow;
  PrepareMove();
}
//====================
void DAQControl::CancelXY() {
  if(fMotor) {
    fMotor->Abort();
    PrepareMove();
  }
}
//====================
void DAQControl::MoveXY() {
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
void DAQControl::MoveStart() {
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
TString DAQControl::WhereAmI() {
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
void DAQControl::ReadBusy() {
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
void DAQControl::ReadPositions() {
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
      fXnow = fDXnow = xmicrons/1000.0 +kDAQControl_OffX;
      fYnow = fDYnow = ymicrons/1000.0 +kDAQControl_OffY;
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
      fXnow = fDXnow = fDXnow + kDAQControl_OffX;
      fYnow = fDYnow = fDYnow + kDAQControl_OffY;
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
*/
//====================
void DAQControl::CreateTab1(TGCompositeFrame *tthis) {
  //
  TGCompositeFrame *fMainFrames_R0 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  CreateAngle(fMainFrames_R0);
  tthis->AddFrame(fMainFrames_R0, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //
  TGCompositeFrame *fMainFrames_R1 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  CreateEyes(fMainFrames_R1);
  tthis->AddFrame(fMainFrames_R1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //
  TGCompositeFrame *fMainFrames_R2 = new TGCompositeFrame(tthis, 170, 20, kVerticalFrame);
  CreateVoltageControler(fMainFrames_R2);
  fMainFrames_R2->ChangeOptions( kSunkenFrame);
  tthis->AddFrame(fMainFrames_R2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //
  TGCompositeFrame *fMainFrames_R3 = new TGCompositeFrame(tthis, 170, 20, kHorizontalFrame);
  tthis->AddFrame(fMainFrames_R3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  TGCompositeFrame *fMainFrames_R3C1 = new TGCompositeFrame(fMainFrames_R3, 170, 20, kVerticalFrame);
  fMainFrames_R3C1->ChangeOptions( kRaisedFrame);
  CreateConfigFile(fMainFrames_R3C1);
  fMainFrames_R3->AddFrame(fMainFrames_R3C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  TGCompositeFrame *fMainFrames_R3C2 = new TGCompositeFrame(fMainFrames_R3, 170, 20, kVerticalFrame);
  fMainFrames_R3C2->ChangeOptions( kRaisedFrame);
  CreateRunControl(fMainFrames_R3C2);
  fMainFrames_R3->AddFrame(fMainFrames_R3C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //
  TGCompositeFrame *fMainFrames_R4 = new TGCompositeFrame(tthis, 170, 20, kVerticalFrame);
  CreateTermistor(fMainFrames_R4);
  fMainFrames_R4->ChangeOptions( kSunkenFrame);
  tthis->AddFrame(fMainFrames_R4, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
}
//====================
DAQControl::DAQControl(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {
  fApp = app;
  sPath = "./Status/";
  fStartRun = NULL;
  fStopRun = NULL;
  fFileName = NULL;
  fFileSize = NULL;
  fFileTS = NULL;
  fLabRunNumber = NULL;
  fConfigFile = NULL;
  
  fTemperature = 0;
  for(int i=0; i!=4; ++i) {
    fAngle[i] = 0;
    fIcon[i] = NULL;
    for(int j=0; j!=2; ++j) {
      fSiPM_Vbr[i][j] = 0;
      fSiPM_OV[i][j] = 0;
    }
  }

  fBarColor[0] = kOrange-3;
  fBarColor[1] = kCyan-3;
  fBarColor[2] = kBlue-9;
  fBarColor[3] = kMagenta-3;
  
  gClient->GetColorByName("blue", fPixelBlue);
  gClient->GetColorByName("white",fPixelWhite);
  gClient->GetColorByName("red",  fPixelRed);
  gClient->GetColorByName("black",fPixelBlack);
  gClient->GetColorByName("green",fPixelGreen);

  std::cout << " HHHHH1 " << std::endl;
  LoadConfigFile();
  
  TGTab *tabcontainer = new TGTab(this,96,26);
  TGCompositeFrame *tab1 = tabcontainer->AddTab("Master Controler");
  CreateTab1(tab1);
  tabcontainer->SetTab(0);
  AddFrame(tabcontainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));

  std::cout << " HHHHH2 " << std::endl;

  //fPixelDefaultBgr = fStart->GetBackground();
  
  SetWindowName("DAQ Control");
  MapSubwindows();
  Layout();
  MapWindow();
  Move(0,100);
  std::cout << " HHHHH3 " << std::endl;
  Lock();
  Unlock();
}
//====================
DAQControl::~DAQControl() {
  Cleanup();
  fApp->Terminate();
}
