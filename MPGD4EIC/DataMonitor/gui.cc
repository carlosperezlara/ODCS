#include <TApplication.h>
#include <pmonitor/pmonitor.h>
#include "DataMonitor.h"
#include <TH1D.h>
#include <TMath.h>
#include <TRandom3.h>

DataMonitor *gDM;
DataMonitor *gDM2;
TH1D *gHist1D;
Int_t gCurrentRun;

//======================
void FillRandom(Double_t idx) {
  Double_t mpv = gRandom->Gaus( 50.0, 5.0 );
  Double_t sig = gRandom->Gaus(  5.0, 1.0 );
  Double_t ped = gRandom->Gaus(  1.0, 0.1 );
  Double_t mod = TMath::Gaus(idx-9.5,0,5.0);
  Double_t hei = gRandom->Gaus( 3000, 300 );
  for(int i=0; i!=256; ++i) {
    Double_t adc = mod*hei*TMath::Landau(0.5+i,mpv,sig) + 50*ped;
    gHist1D->SetBinContent( i+1, adc );
  }
}
//======================
int pinit() {
  return 0;
}
//======================
int process_event (Event * e) {
  std::cout << " [pmonitor::process_event called" << std::endl;
  Int_t run = e->getRunNumber();
  if(gCurrentRun!=run) {
    gDM->NewRun(run);
    gCurrentRun = run;
  }
  gDM->NewEvent();
  Packet *p = e->getPacket(1003); // example packet
  //Packet *p = e->getPacket(1001); // test packet
  if(p) {
    gHist1D = NULL;
    for(int i=0; i!=8; ++i) {
      for(int j=0; j!=20; ++j) {
	gHist1D = gDM->GetChannel(i,j);
	if(gHist1D) {
	  FillRandom(j);
	}
      }
    }
  }
  delete p;
  std::cout << " pmonitor::process_event built ]. Launching merge..." << std::endl;
  gDM->Merge();
  //sleep(1);
  std::cout << " DONE" << std::endl << std::endl;
  return 0;
}
//======================
//======================
//======================
int main() {
  TApplication *app = new TApplication("gui",0,0);
  gDM = new DataMonitor(app, 1600, 850);
  ptestopen();
  //poncsopen("test/rcdaq-00000001-0000.evt");
  pstart();
  app->Run();
  return 0;
}
