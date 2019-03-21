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
  Double_t mod = TMath::Gaus(idx,0,5.0);
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
  //std::cout << " [pmonitor::process_event called" << std::endl;
  Int_t run = e->getRunNumber();
  if(gCurrentRun!=run) {
    gDM->NewRun(run);
    gCurrentRun = run;
  }
  Int_t evnr = e->getEvtSequence();
  gDM->NewEvent( evnr );
  Packet *p = e->getPacket(1003); // example packet
  //Packet *p = e->getPacket(1001); // test packet
  //if(p) {
    gHist1D = NULL;
    for(int i=0; i!=8; ++i) {
      for(int j=0; j!=26; ++j) {
	gHist1D = gDM->GetChannel(i,j);
	if(gHist1D) {
	  FillRandom(j-13);
	}
      }
    }
    //}
  delete p;
  //std::cout << " pmonitor::process_event built ]. Launching merge..." << std::endl;
  gDM->Merge();
  //sleep(1);
  //std::cout << " DONE" << std::endl << std::endl;
  return 0;
}
//======================
//======================
//======================
int main(int nn, char** arg) {
  if(nn>1) {
    //TString filename = "/data/purschke/mrwell/mrwell-00000363-0000.evt";
    TString filename = arg[1];
    std::cout << filename.Data() << std::endl;
    pfileopen( filename.Data() );
    //ptestopen();
  } else {
    rcdaqopen();
  }

  TApplication *app = new TApplication("gui",0,0);
  gDM = new DataMonitor(app, 1600, 850);

  pstart();
  app->Run();
  return 0;
}
