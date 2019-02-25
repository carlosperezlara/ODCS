#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <TApplication.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEdit.h>
#include <TGNumberEntry.h>
#include <TGIcon.h>

#include <TH1D.h>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TImage.h>

class DataMonitor : public TGMainFrame {
 private:
  void CreatePlot(TGCompositeFrame *mf);
  TApplication *fApp;
  TH1D *fChannel;
  TCanvas *fCanvasMap;
  TTimer *fRefresh;
  
 public:
  DataMonitor(TApplication *app, UInt_t w, UInt_t h);
  virtual ~DataMonitor();
  void RefreshAll();
  TH1D *GetChannel() {return fChannel;}
  
  ClassDef(DataMonitor, 0)
};

#endif
