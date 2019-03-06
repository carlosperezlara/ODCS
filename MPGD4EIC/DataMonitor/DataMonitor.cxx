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
#include <TTimer.h>
#include <TH1D.h>
#include <TTimeStamp.h>
#include <TLatex.h>
#include <TImage.h>

#include <fstream>
#include <iostream>

#include "DataMonitor.h"

ClassImp(DataMonitor);

//====================
void DataMonitor::CreatePlot(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("canvasplot",mf,400,400,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  fCanvasMap->SetTopMargin(0.07);
  fCanvasMap->SetBottomMargin(0.08);
  fCanvasMap->SetLeftMargin(0.11);
  fCanvasMap->SetRightMargin(0.05);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetGridx(1);
  fCanvasMap->SetGridy(1);
  fChannel->SetStats(0);
  fChannel->Draw();
  fChannel->GetXaxis()->SetNdivisions(820);
  fChannel->GetYaxis()->SetNdivisions(820);
  //fCanvasMap->SetEditable(kFALSE);
}
//====================
void DataMonitor::RefreshAll() {
  if(fCanvasMap) {
    fCanvasMap->Modified();
    fCanvasMap->Update();
  }
}
//====================
DataMonitor::DataMonitor(TApplication *app, UInt_t w, UInt_t h) : TGMainFrame(gClient->GetRoot(), w, h) {
  fApp = app;
  SetWindowName("Data Monitor and Quality Assurance");
  fChannel = new TH1D("Channel0","Channel  0",256,-0.5,255.5);
  CreatePlot(this);
  MapSubwindows();
  Layout();
  MapWindow();
  fRefresh = new TTimer();
  fRefresh->Connect("Timeout()", "DataMonitor", this, "RefreshAll()");
  fRefresh->Start(100, kFALSE);
  fRefresh->TurnOn();
}
//====================
DataMonitor::~DataMonitor() {
  Cleanup();
  fRefresh->TurnOff();
  fApp->Terminate();
}
