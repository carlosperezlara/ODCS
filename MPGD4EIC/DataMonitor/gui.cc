#include <TApplication.h>
#include <pmonitor/pmonitor.h>
#include "DataMonitor.h"
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TRandom3.h>

DataMonitor *gDM;
TH1D *gHist1D;
TH2D *gHist2D;
Int_t gCurrentRun;
Bool_t gTOYMODEL=false;

//======================
void FillRandom(Double_t idx) {
  Double_t mpv = gRandom->Gaus( 50.0, 5.0 );
  Double_t sig = gRandom->Gaus(  5.0, 1.0 );
  Double_t ped = gRandom->Gaus(  1.0, 0.1 );
  Double_t mod = TMath::Gaus(idx,0,5.0);
  Double_t hei = gRandom->Gaus( 3000, 300 );
  for(int i=0; i!=256; ++i) {
    Double_t adc = mod*hei*TMath::Landau(0.5+i,mpv,sig);// + 50*ped;
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
  Int_t evnr = e->getEvtSequence();
  gDM->NewEvent( evnr );

  if(gTOYMODEL) {
    gHist1D = NULL;
    for(int bd=0; bd!=kNumberOfBoards; ++bd) {
      Int_t numch = gDM->GetDREAMChannels(bd);
      for(int ch=0; ch!=numch; ++ch) {
	gHist1D = gDM->GetChannel(bd,ch);
	if(gHist1D) {
	  FillRandom(ch-numch/2);
	}
      }
    }
  } else {
    Packet *p = e->getPacket(3000);
    if(p) {
      for(int feu=0; feu!=8; ++feu) {
	Int_t feuid = p->iValue(feu, "FEU_ID");
	Int_t samples =  p->iValue(p->iValue(feu, "FEU_ID"), "SAMPLES");
	//for(int ch=0; ch!=512/*8x64*/; ++ch) {
	/*
	for(int ch=0; ch!=512; ++ch) {
          gHist2D = gDM->GetScan(feu);
	  for(int sa=0; sa!=samples; ++sa) {
	    Int_t adc = p->iValue( feuid, ch, sa);
	    gHist2D->Fill( double(ch), double(sa),  double(adc) );
	  }	  
	}
	*/
	//std::cout << " Reading FEU ID " << feuid << " which has " << samples << " samples." <<  std::endl;
	int nch = gDM->GetDREAMChannels(feu);
	int minch = gDM->GetDREAMChannel(feu,0);
	int maxch = minch + nch;
	//std::cout << " Listening to " << minch << "--" << maxch << std::endl;
	for(int ch=minch; ch!=maxch; ++ch) {
	  gHist1D = gDM->GetChannel(feu,ch-minch);
	  for(int sa=0; sa!=samples; ++sa) {
	    Int_t adc = p->iValue( feuid, ch, sa);
	    gHist1D->SetBinContent( sa+1, double(adc) );
	    //std::cout << adc << "|";
	  }
	}
	//std::cout << std::endl;
      }
    }
    delete p;
  }
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
  } else {
    //ptestopen(); gTOYMODEL = true;
    rcdaqopen();
  }

  TApplication *app = new TApplication("gui",0,0);
  gDM = new DataMonitor(app, 1600, 850);
  
  pstart();
  app->Run();
  return 0;
}
