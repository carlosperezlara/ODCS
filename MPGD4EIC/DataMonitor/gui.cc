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
  //Double_t mpv = gRandom->Gaus( kNumberOfSamples/4.0,  5.0 );
  //Double_t sig = gRandom->Gaus( kNumberOfSamples/16.0, 1.0 );
  Double_t mpv = gRandom->Gaus( 6.0, 0.5 );
  Double_t sig = gRandom->Gaus( 2.0, 0.3 );
  Double_t ped = gRandom->Gaus( 1.0, 0.1 );
  Double_t mod = TMath::Gaus(idx,0,5.0);
  Double_t hei = gRandom->Gaus( 2000, 100 );
  for(int i=0; i!=kNumberOfSamples; ++i) {
    Double_t adc = mod*hei*TMath::Landau(0.5+i,mpv,sig) + 250*ped;
    gHist1D->SetBinContent( i+1, adc );
  }
}
//======================
int pinit() {
  //pidentify(0);
  return 0;
}
//======================
int process_event (Event * e) {
  //std::cout << " [pmonitor::process_event called" << std::endl;
  // TOYMODEL
  if(gTOYMODEL) {
    static Int_t evnrTM = 0;
    if( evnrTM == 0 ) {
      std::cout << std::endl << " ***** running TOY MODEL ****** " << std::endl << std::endl;
      gCurrentRun = 0;
      gDM->NewRun(gCurrentRun);
    }
    if(evnrTM>10000) {
      evnrTM = 0;
      gCurrentRun++;
      gDM->NewRun(gCurrentRun);
      std::cout << std::endl << " ***** TM: new run ****** " << std::endl << std::endl;
      usleep(4000);
    }
    //std::cout << std::endl << " I AM HERE " << std::endl << std::endl;
    gDM->NewEvent( evnrTM );
    //std::cout << std::endl << " NOW HERE " << std::endl << std::endl;
    gHist1D = NULL;
    for(int bd=0; bd!=kNumberOfBoards; ++bd) {
      if(gDM->IsBoardNotInstalled(bd)) continue;
      //gHist2D = gDM->GetScan(bd);
      Int_t numch = gDM->GetDREAMChannels(bd);
      for(int ch=0; ch!=numch; ++ch) {
	gHist1D = gDM->GetChannel(bd,ch);
	if(gHist1D) {
	  FillRandom(ch-numch/2);
	}
	//if(gHist2D) {
	//  gHist2D->Fill();
	//}
      }
    }
    //std::cout << " pmonitor::process_event built ]. Launching merge..." << std::endl;
    gDM->Merge();
    evnrTM++;
    //usleep(500);
    //std::cout << " DONE" << std::endl << std::endl;
    return 0;
  }

  //REAL DATA
  Int_t run = e->getRunNumber();
  if(gCurrentRun!=run) {
    gDM->NewRun(run);
    gCurrentRun = run;
  }
  Int_t evnr = e->getEvtSequence();
  gDM->NewEvent( evnr );
  
  Packet *p = e->getPacket(3000);
  if(p) {
    int nfeu = p->iValue(0, "NR_FEU");
    if(nfeu>9) {
      std::cout << " [pmonitor] Ev" << evnr;
      std::cout << " NR_FEU reports to be greater than 9. Unreasonable. Skipping event." << std::endl;
    } else {
      for(int feu=0; feu<nfeu; ++feu) {
	if(gDM->IsBoardNotInstalled(feu)) continue;
	Int_t feuid = p->iValue(feu, "FEU_ID");
	Int_t samples =  p->iValue(feuid, "SAMPLES");
	Int_t nchips = p->iValue(feuid, "NR_DREAM");
	//std::cout << " Reading FEU ID " << feuid << " which has " << samples << " samples." <<  std::endl;
	if(samples>256) {
	  std::cout << " [pmonitor] Ev" << evnr <<  " FEU ID " << feuid;
	  std::cout << " reports number of samples greater than 256. Skipping FEU." << std::endl;
	  continue;
	}
	if(nchips>8) {
	  std::cout << " [pmonitor] Ev" << evnr << " FEU ID " << feuid;
	  std::cout << " reports number of chips greater than 8. Skipping FEU." << std::endl;
	  continue;
	}
	// ALL aboard
	for(int chip=0; chip<nchips; ++chip) {
	  if( !p->iValue(feuid,chip,"DREAM_ENABLED") ) continue;
	  for(int ch=0; ch!=64; ++ch) {
	    gHist2D = gDM->GetScan(feu);
	    for(int sa=0; sa!=samples; ++sa) {
	      Int_t dreamch = ch+64*chip;
	      Int_t adc = p->iValue( feuid, dreamch, sa);
	      gHist2D->Fill( double(dreamch), double(sa),  double(adc) );
	    }	  
	  }
	}
	int nch = gDM->GetDREAMChannels(feu);
	int minch = gDM->GetDREAMChannel(feu,0);
	int maxch = minch + nch;
	//std::cout << " Listening to " << minch << "--" << maxch << std::endl;
	for(int ch=minch; ch<maxch; ++ch) {
	  Int_t chipD = ch/54;
	  if( !p->iValue(feuid,chipD,"DREAM_ENABLED") ) continue;
	  gHist1D = gDM->GetChannel(feu,ch-minch);
	  for(int sa=0; sa<samples; ++sa) {
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
    ptestopen(); gTOYMODEL = true;
    //rcdaqopen();
  }
  
  TApplication *app = new TApplication("gui",0,0);
  gDM = new DataMonitor(app, 1600, 850);
  
  pstart();
  app->Run();
  return 0;
}
