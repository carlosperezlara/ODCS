#include <TApplication.h>
#include <TGFrame.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TStyle.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>
#include <TGTab.h>
#include <TRootEmbeddedCanvas.h>

#include <TSystem.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TH2D.h>
#include <TTimeStamp.h>
#include <TLatex.h>
#include <TImage.h>

#include <fstream>
#include <iostream>

#include "DataMonitor.h"

ClassImp(DataMonitor);

//====================
void DataMonitor::Merge() {
  //fRefresh->TurnOff();
  for(int i=0; i!=kNumberOfBoards; ++i) {
    //fTimeSummaryTemp->Reset();
    for(int j=0; j!=kNumberOfChannels; ++j) {
      Double_t hei = fChannel[i][j]->GetMaximum() - fPedestals[i][j];
      fHeight[i][j]->Fill( hei );
      //fTimeSummaryTemp->Add( fChannel[i][j], sum );
      Double_t sum = fChannel[i][j]->Integral() -kNumberOfSamples*fPedestals[i][j];
      fWidth[i][j]->Fill( sum );
      for(int b=0; b!=fChannel[i][j]->GetXaxis()->GetNbins(); ++b) {
	Double_t xc = fChannel[i][j]->GetXaxis()->GetBinCenter(b+1);
	Double_t yc = fChannel[i][j]->GetBinContent(b+1);
	fSignal[i][j]->Fill(xc,yc,hei);
	fTimeSummary[i]->Fill(double(b), yc );
      }
      fHitSummary[i]->Fill(double(j),sum);
    }
  }
  if(fNoEventsSampled%kMergeRefresh==0) {
    RefreshAll();
  }
  //fRefresh->TurnOn();
}
//====================
void DataMonitor::StampRun(TGCompositeFrame *mf) {
  TGLabel *labs;
  fThisRun = new TGLabel(mf, "Run : -1" );
  mf->AddFrame(fThisRun, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(mf, Form("Position : ( %.1f, %.1f )", fPosX, fPosY) );
  mf->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(mf, Form("Closest Cell : %s", fClosestCell.Data()) );
  mf->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fEventsSampled = new TGLabel(mf, "Events sampled: 0.0k" );
  mf->AddFrame(fEventsSampled, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
}
//====================
void DataMonitor::NewRun(Int_t run) {
  if(fThisRun) fThisRun->SetText( Form("Run : %d",run) );
  for(int i=0; i!=kNumberOfBoards; ++i) {
    fHitSummary[i]->Reset();
    for(int j=0; j!=kNumberOfChannels; ++j) {
      fHeight[i][j]->Reset();
      fWidth[i][j]->Reset();
    }
  }
  fNoEventsSampled = 0;
}
//====================
void DataMonitor::NewEvent() {
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      fChannel[i][j]->Reset();
    }
  }
  fEventsReaded += 1;
  fNoEventsSampled += 1;
  if(fNoEventsSampled%kNewEventRefresh==0) {
    //TString tmp = Form("Events sampled : %.1fk",fEventsReaded*1e-3);
    TString tmp = Form("Events sampled : %.1fk",fNoEventsSampled*1e-3);
    fEventsSampled->SetText( tmp.Data() );
  }
}
//====================
void DataMonitor::CreateChannels(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("channelsplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetTopMargin(0.1);
  fCanvasMap->SetBottomMargin(0.1);
  fCanvasMap->SetLeftMargin(0.2);
  fCanvasMap->SetRightMargin(0.1);
  fCanvasMap->Divide(kNumberOfChannels,kNumberOfBoards,0,0);
  TH2D *axis = new TH2D("axis","",100,-15,270,100,-50,1099);
  axis->SetStats(0);
  axis->GetYaxis()->SetLabelSize(0.1);
  axis->GetXaxis()->SetLabelSize(0.1);
  axis->SetTitleSize(50);
  axis->GetXaxis()->SetNdivisions(508);
  axis->GetYaxis()->SetNdivisions(508);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      TVirtualPad *tmp = fCanvasMap->cd(i*kNumberOfChannels+j+1);
      axis->SetTitle( Form("S%dP%d",i,j) );
      axis->DrawCopy();
      fChannel[i][j]->Draw("BSAME");
    }
  }
  fCanvasMap->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateSignals(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("signalsplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapS = new TCanvas("CanvasMapS", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapS);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapS->Divide(kNumberOfChannels,kNumberOfBoards,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      TVirtualPad *tmp = fCanvasMapS->cd(i*kNumberOfChannels+j+1);
      tmp->SetTopMargin(0.1);
      tmp->SetBottomMargin(0.1);
      tmp->SetLeftMargin(0.2);
      tmp->SetRightMargin(0.1);
      fSignal[i][j]->Draw("B");
    }
  }
  fCanvasMapS->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateHeights(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("heightsplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapH = new TCanvas("CanvasMapH", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapH);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapH->Divide(kNumberOfChannels,kNumberOfBoards,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      TVirtualPad *tmp = fCanvasMapH->cd(i*kNumberOfChannels+j+1);
      tmp->SetTopMargin(0.1);
      tmp->SetBottomMargin(0.1);
      tmp->SetLeftMargin(0.2);
      tmp->SetRightMargin(0.1);
      fHeight[i][j]->Draw("B");
    }
  }
  fCanvasMapH->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateWidths(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("heightsplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapW = new TCanvas("CanvasMapW", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapW);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapW->Divide(kNumberOfChannels,kNumberOfBoards,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      TVirtualPad *tmp = fCanvasMapW->cd(i*kNumberOfChannels+j+1);
      tmp->SetTopMargin(0.1);
      tmp->SetBottomMargin(0.1);
      tmp->SetLeftMargin(0.2);
      tmp->SetRightMargin(0.1);
      fWidth[i][j]->Draw("B");
    }
  }
  fCanvasMapW->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateHitSummary(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("hitsummaryplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapHS = new TCanvas("CanvasMapHS", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapHS);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapHS->Divide(kNumberOfBoards/2,2,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    TVirtualPad *tmp = fCanvasMapHS->cd(i+1);
    tmp->SetTopMargin(0.1);
    tmp->SetBottomMargin(0.1);
    tmp->SetLeftMargin(0.2);
    tmp->SetRightMargin(0.1);
    fHitSummary[i]->Draw("B");
  }
  fCanvasMapHS->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateTimeSummary(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("hitsummaryplot",mf,1600,800,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapTS = new TCanvas("CanvasMapTS", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapTS);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapTS->Divide(kNumberOfBoards/2,2,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    TVirtualPad *tmp = fCanvasMapTS->cd(i+1);
    tmp->SetTopMargin(0.1);
    tmp->SetBottomMargin(0.1);
    tmp->SetLeftMargin(0.2);
    tmp->SetRightMargin(0.1);
    fTimeSummary[i]->Draw("B");
  }
  fCanvasMapTS->SetEditable(kFALSE);
}
//====================
void DataMonitor::RefreshAll() {
  std::cout << "  >> RefreshAll called" << std::endl;
  //fRefresh->TurnOff();
  if(fTabContainer->GetCurrent()==0&&fCanvasMap) {
    std::cout << "   current canvas 0" << std::endl;
    fCanvasMap->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfChannels*kNumberOfBoards; ++i) {
      fCanvasMap->cd(i)->Modified();
      fCanvasMap->cd(i)->Update();
    }
    fCanvasMap->Modified();
    fCanvasMap->Update();
    fCanvasMap->SetEditable(kFALSE);
  }
  if(fTabContainer->GetCurrent()==1&&fCanvasMapS) {
    std::cout << "   current canvas 1" << std::endl;
    fCanvasMapS->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfChannels*kNumberOfBoards; ++i) {
      fCanvasMapS->cd(i)->Modified();
      fCanvasMapS->cd(i)->Update();
    }
    fCanvasMapS->Modified();
    fCanvasMapS->Update();
    fCanvasMapS->SetEditable(kFALSE);
  }
  if(fTabContainer->GetCurrent()==2&&fCanvasMapH) {
    std::cout << "   current canvas 2" << std::endl;
    fCanvasMapH->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfChannels*kNumberOfBoards; ++i) {
      fCanvasMapH->cd(i)->Modified();
      fCanvasMapH->cd(i)->Update();
    }
    fCanvasMapH->Modified();
    fCanvasMapH->Update();
    fCanvasMapH->SetEditable(kFALSE);
  }
  if(fTabContainer->GetCurrent()==3&&fCanvasMapW) {
    std::cout << "   current canvas 3" << std::endl;
    fCanvasMapW->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfChannels*kNumberOfBoards; ++i) {
      fCanvasMapW->cd(i)->Modified();
      fCanvasMapW->cd(i)->Update();
    }
    fCanvasMapW->Modified();
    fCanvasMapW->Update();
    fCanvasMapW->SetEditable(kFALSE);
  }
  if(fTabContainer->GetCurrent()==4&&fCanvasMapHS) {
    std::cout << "   current canvas 4" << std::endl;
    fCanvasMapHS->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfBoards; ++i) {
      fCanvasMapHS->cd(i+1)->Modified();
      fCanvasMapHS->cd(i+1)->Update();
    }
    fCanvasMapHS->Modified();
    fCanvasMapHS->Update();
    fCanvasMapHS->SetEditable(kFALSE);
  }
  if(fTabContainer->GetCurrent()==5&&fCanvasMapTS) {
    std::cout << "   current canvas 5" << std::endl;
    fCanvasMapTS->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfBoards; ++i) {
      fCanvasMapTS->cd(i+1)->Modified();
      fCanvasMapTS->cd(i+1)->Update();
    }
    fCanvasMapTS->Modified();
    fCanvasMapTS->Update();
    fCanvasMapTS->SetEditable(kFALSE);
  }
  std::cout << "  << RefreshAll called" << std::endl;
  //fRefresh->TurnOn();
}
//====================
DataMonitor::DataMonitor(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {

  std::cout << "DM: I am awaking" << std::endl;
  fApp = app;
  gStyle->SetTitleFontSize(0.2);
  fNoEventsSampled = 0;
  SetWindowName("Data Monitor and Quality Assurance");
  fTimeSummaryTemp = new TH1D( "TST","TST",kNumberOfSamples,-0.5,kNumberOfSamples-0.5);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    fHitSummary[i] = new TH1D( Form("HS%d",i),Form("HS%d",i),kNumberOfChannels,-0.5,kNumberOfChannels-0.5);
    fHitSummary[i]->SetFillColor(kBlue-3);
    fHitSummary[i]->SetLineColor(kBlue-3);
    fHitSummary[i]->SetStats(0);
    fHitSummary[i]->GetYaxis()->SetLabelSize(0.1);
    fHitSummary[i]->GetXaxis()->SetLabelSize(0.1);
    fHitSummary[i]->GetXaxis()->SetNdivisions(504);
    fHitSummary[i]->GetYaxis()->SetNdivisions(508);
    fTimeSummary[i] = new TH1D( Form("TS%d",i),Form("TS%d",i),kNumberOfSamples,-0.5,kNumberOfSamples-0.5);
    fTimeSummary[i]->SetFillColor(kBlue-3);
    fTimeSummary[i]->SetLineColor(kBlue-3);
    fTimeSummary[i]->SetStats(0);
    fTimeSummary[i]->GetYaxis()->SetLabelSize(0.1);
    fTimeSummary[i]->GetXaxis()->SetLabelSize(0.1);
    fTimeSummary[i]->GetXaxis()->SetNdivisions(504);
    fTimeSummary[i]->GetYaxis()->SetNdivisions(508);
    for(int j=0; j!=kNumberOfChannels; ++j) {
      fChannel[i][j] = NULL;
      fSignal[i][j] = NULL;
      fHeight[i][j] = NULL;
      fWidth[i][j] = NULL;
      fChannel[i][j] = new TH1D( Form("C%dP%d",i,j),Form("C%dP%d",i,j),kNumberOfSamples,-0.5,kNumberOfSamples-0.5);
      fSignal[i][j] = new TProfile( Form("S%dP%d",i,j),Form("S%dP%d",i,j),kNumberOfSamples,-0.5,kNumberOfSamples-0.5);
      fHeight[i][j] = new TH1D( Form("H%dP%d",i,j),Form("H%dP%d",i,j),100,0,1100);
      fWidth[i][j] = new TH1D( Form("W%dP%d",i,j),Form("W%dP%d",i,j),100,0,50000);
      fChannel[i][j]->SetFillColor(kBlue-3);
      fChannel[i][j]->SetLineColor(kBlue-3);
      fSignal[i][j]->SetFillColor(kBlue-3);
      fSignal[i][j]->SetLineColor(kBlue-3);
      fHeight[i][j]->SetFillColor(kBlue-3);
      fHeight[i][j]->SetLineColor(kBlue-3);
      fWidth[i][j]->SetFillColor(kBlue-3);
      fWidth[i][j]->SetLineColor(kBlue-3);
      fSignal[i][j]->SetStats(0);
      fSignal[i][j]->GetYaxis()->SetLabelSize(0.1);
      fSignal[i][j]->GetXaxis()->SetLabelSize(0.1);
      fSignal[i][j]->GetXaxis()->SetNdivisions(504);
      fSignal[i][j]->GetYaxis()->SetNdivisions(508);
      fHeight[i][j]->SetStats(0);
      fHeight[i][j]->GetYaxis()->SetLabelSize(0.1);
      fHeight[i][j]->GetXaxis()->SetLabelSize(0.1);
      fHeight[i][j]->GetXaxis()->SetNdivisions(504);
      fHeight[i][j]->GetYaxis()->SetNdivisions(508);
      fWidth[i][j]->SetStats(0);
      fWidth[i][j]->GetYaxis()->SetLabelSize(0.1);
      fWidth[i][j]->GetXaxis()->SetLabelSize(0.1);
      fWidth[i][j]->GetXaxis()->SetNdivisions(504);
      fWidth[i][j]->GetYaxis()->SetNdivisions(508);
      fPedestals[i][j] = 50;
    }
  }
  fThisRun = NULL;
  fEventsSampled = NULL;
  fThisEvent = -1;
  fEventsReaded = 0;
  fEventsCataloged = 0;
  fClosestCell = "XX";
  fPosX = 0;
  fPosY = 0;

  TGCompositeFrame *mfR1   = new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  //TGCompositeFrame *mfR2   = new TGCompositeFrame(this, 170, 20, kHorizontalFrame);
  //TGCompositeFrame *mfR2C1 = new TGCompositeFrame(mfR2, 170, 20, kVerticalFrame);
  //TGCompositeFrame *mfR2C2 = new TGCompositeFrame(mfR2, 170, 20, kVerticalFrame);
  //mfR2->AddFrame(mfR2C1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //mfR2->AddFrame(mfR2C2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  this->AddFrame(mfR1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //this->AddFrame(mfR2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  //CreatePlot(mfR2);
  StampRun(mfR1);

  fTabContainer = new TGTab(this,96,26);
  TGCompositeFrame *tab1 = fTabContainer->AddTab("Last Event");
  CreateChannels(tab1);
  TGCompositeFrame *tab2 = fTabContainer->AddTab("Signal");
  CreateSignals(tab2);
  TGCompositeFrame *tab3 = fTabContainer->AddTab("Height");
  CreateHeights(tab3);
  TGCompositeFrame *tab4 = fTabContainer->AddTab("Pulse");
  CreateWidths(tab4);
  TGCompositeFrame *tab5 = fTabContainer->AddTab("Hit Summary");
  CreateHitSummary(tab5);
  TGCompositeFrame *tab6 = fTabContainer->AddTab("Time Summary");
  CreateTimeSummary(tab6);
  fTabContainer->SetTab(4);
  this->AddFrame(fTabContainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));

  MapSubwindows();
  Layout();
  MapWindow();

  //std::cout << "DM: All windows are ready now. I will start updating every " << kLoopTime/1000.0 << " seconds. " << std::endl;

  //fRefresh = new TTimer();
  //fRefresh->Connect("Timeout()", "DataMonitor", this, "RefreshAll()");
  //fRefresh->Start(kLoopTime, kFALSE);
}
//====================
DataMonitor::~DataMonitor() {
  Cleanup();
  //fRefresh->TurnOff();
  fApp->Terminate();
}
