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
#include <TFile.h>
#include <TMath.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TTimer.h>
#include <TH1D.h>
#include <TF1.h>
#include <TProfile.h>
#include <TH2D.h>
#include <TTimeStamp.h>
#include <TLatex.h>
#include <TImage.h>
#include <TRandom3.h>

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

#include "DataMonitor.h"
#include "V1742DATReader.h"
#include "LECROYTRCReader.h"
#include "WaveForm.h"

ClassImp(DataMonitor);
/*
//====================
void DataMonitor::Merge() {
  if(!fReady) return;
  std::vector<std::pair<unsigned, unsigned> > myhits;
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=fDREAMChannels[i]; ++j) {
      Int_t bpea = fChannel[i][j]->GetMaximumBin();
      Int_t bmin = bpea-fIntWindow[i];
      Int_t bmax = bpea+fIntWindow[i];
      if(bmin<0) bmin=0;
      if(bmax>kNumberOfSamples) bmax=kNumberOfSamples;
      Double_t sum = fChannel[i][j]->Integral( bmin, bmax  );
      sum -= (bmax-bmin)*fPedestals[i][j];

      //Double_t hei = fChannel[i][j]->GetMaximum() - fPedestals[i][j];
      Double_t hei = fChannel[i][j]->GetBinContent( bpea ) - fPedestals[i][j];
      fHeight[i][j]->Fill( hei );
      fWidth[i][j]->Fill( sum );
      for(int b=0; b!=kNumberOfSamples; ++b) {
	Double_t xc = fChannel[i][j]->GetXaxis()->GetBinCenter(b+1);
	Double_t yc = fChannel[i][j]->GetBinContent(b+1);
	fSignal[i][j]->Fill(xc,yc,hei);
	fTimeSummary[i]->Fill(double(b), yc );
      }
      fHitSummary[i]->Fill(double(j-fDREAMChannels[i]/2),sum);
      myhits.push_back( std::make_pair( fDREAMChannel[i][j], sum ) );
    }
    //std::cout << " myhits " << myhits.size() << " || ";
    //std::cout << std::endl;
    
    std::vector<Cluster> myclusters = mplane->GetClusters( myhits );
      
    //std::cout << " BOARD " << i << " || myclusters " << myclusters.size() << " || WD: ";
    fClusters_Num[i]->Fill( double(myclusters.size()) );
    for(unsigned kk=0; kk!=myclusters.size(); ++kk) {
      fClusters_Wid[i]->Fill( myclusters[kk].mWidth );
      fClusters_Amp[i]->Fill( myclusters[kk].mAmplitude );
      fClusters_xCE[i]->Fill( myclusters[kk].mCentroid );
      //std::cout << myclusters[kk].mWidth << " AMP: ";
      //std::cout << myclusters[kk].mAmplitude << " xCENT: ";
      //std::cout << myclusters[kk].mCentroid << " ";
      //std::cout << " || ";
    }
    //std::cout << std::endl;
  }
  if(fNoEventsSampled%kMergeRefresh==0) {
    RefreshAll();
  }
}
//====================
TString DataMonitor::GetLocalCell(Int_t bd) {
  Double_t x, y, xp, yp;
  GetXYFromCell(fClosestCell,x,y);
  Double_t cos = TMath::Cos( fBoardAngle[bd]/TMath::PiOver2()  ); //convention fAng is 
  Double_t sin = TMath::Sin( fBoardAngle[bd]/TMath::PiOver2()  ); //postive counterclockwise
  xp = cos*x - sin*y;
  yp = sin*x + cos*y;
  TString cell = GetCellName(xp,yp);
  //std::cout << "**=> " << x << " " << y << std::endl;
  //std::cout << "**=> " << xp << " " << yp << " ==> "<< cell.Data() << std::endl;
  return cell;
}
//====================
void DataMonitor::StampRun(TGCompositeFrame *mf) {
  TGLabel *labs;
  fThisRun = new TGLabel(mf, "Run : -1" );
  //TGFont *font = fThisRun->GetFont();
  mf->AddFrame(fThisRun, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(mf, Form("Position : ( %.1f, %.1f )", fPosX, fPosY) );
  mf->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  labs = new TGLabel(mf, Form("Closest Cell : %s", fClosestCell.Data()) );
  mf->AddFrame(labs, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fEventsSampled = new TGLabel(mf, "Events sampled: ---" );
  mf->AddFrame(fEventsSampled, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  fSamplingFraction = new TGLabel(mf, "Sampling fraction: ----" );
  mf->AddFrame(fSamplingFraction, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
}
//====================
void DataMonitor::NewRun(Int_t run) {
  // FIRST SAVE
  static bool awaking = true;
  if(!awaking) {
    // saving info
    TTimeStamp timestamp;
    TString ts = timestamp.AsString("s");;
    std::ofstream fout("Run_Data/history.log",std::ofstream::out|std::ofstream::app);
    fout << ts.Data() << "  " << kNumberOfBoards << "  ";
    for(int bd=0;bd!=kNumberOfBoards; ++bd) {
      Double_t SUM = 0;
      for(int i=0; i!=fDREAMChannels[bd]; ++i) {
	SUM += fWidth[bd][i]->Integral();
      }
      fout << Form("  %e",SUM);
    }
    fout << std::endl;
    fout.close();
  } else {
    awaking = false;
  }

  // SECOND CLEAR
  for(int i=0; i!=kNumberOfBoards; ++i) {
    fHitSummary[i]->Reset();
    for(int j=0; j!=kNumberOfChannels; ++j) {
      fHeight[i][j]->Reset();
      fWidth[i][j]->Reset();
    }
  }
  fNoEventsSampled = 0;
  ReadPosition();
  ReadConfig();
  ConfigureChannels();

  //THIRD REFRESH
  if(fThisRun) fThisRun->SetText( Form("Run : %d",run) );
  std::ifstream fin("Run_Data/history.log");
  TString timeS1, timeS2;
  Int_t numb;
  Double_t ddd[kNumberOfBoards];
  for(;;) {
    fin >> timeS1 >> timeS2 >> numb;
    if(!fin.good()) break;
    for(int i=0; i<numb; ++i) {
      fin >> ddd[i];
    }
    if(!fin.good()) break;
    for(int i=0; i<numb; ++i) {
      fHistory[i]->SetPoint( fHistory[i]->GetN(), fHistory[i]->GetN(), ddd[i] );
    }
  }
  fin.close();
}
//====================
void DataMonitor::NewEvent(Int_t evr) {
  fReady = true;
  for(int i=0; i!=kNumberOfBoards; ++i) {
    for(int j=0; j!=kNumberOfChannels; ++j) {
      if(!fChannel[i][j]) {
	fReady = false;
	break;
      }
      fChannel[i][j]->Reset();
    }
    if(!fReady) break;
  }
  if(!fReady) {
    std::cout << "DM: not ready to take data now." << std::endl;
    return;
  }
  fThisEvent = evr;
  fEventsReaded += 1;
  fNoEventsSampled += 1;
  static bool __up__ = false;
  //std::cout << "AA " << evr << std::endl;
  if(fNoEventsSampled%kNewEventRefresh==0) {
    TString tmp = Form("Events sampled : %d",fNoEventsSampled);
    fEventsSampled->SetText( tmp.Data() );
    if(fThisEvent>100) {
      TString tmp = Form("Sampling fraction : %.3f",fNoEventsSampled/double(fThisEvent));
      fSamplingFraction->SetText( tmp.Data() );
    }
    if(!__up__) {
      __up__ = true;
      std::cout << "DM: I am ready."<< std::endl;
    }
  }
  //std::cout << "BB " << evr << std::endl;
}
//====================
void DataMonitor::CreateClusters(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("clustersplot",mf,850,900,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapCL = new TCanvas("CanvasMapCL", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapCL);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapCL->SetTopMargin(0.1);
  fCanvasMapCL->SetBottomMargin(0.1);
  fCanvasMapCL->SetLeftMargin(0.2);
  fCanvasMapCL->SetRightMargin(0.1);
  fCanvasMapCL->Divide(4,kNumberOfBoards);
  for(int bd=0; bd!=kNumberOfBoards; ++bd) {
    TVirtualPad *tmp1 = fCanvasMapCL->cd(bd*4+1);
    fClusters_Num[bd]->Draw("HIST");
    TVirtualPad *tmp2 = fCanvasMapCL->cd(bd*4+2);
    fClusters_Amp[bd]->Draw("HIST");
    TVirtualPad *tmp3 = fCanvasMapCL->cd(bd*4+3);
    fClusters_Wid[bd]->Draw("HIST");
    TVirtualPad *tmp4 = fCanvasMapCL->cd(bd*4+4);
    fClusters_xCE[bd]->Draw("HIST");
  }
  fCanvasMapCL->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateChannels(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("channelsplot",mf,1800,930,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMap = new TCanvas("CanvasMap", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMap);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMap->SetTopMargin(0.1);
  fCanvasMap->SetBottomMargin(0.1);
  fCanvasMap->SetLeftMargin(0.2);
  fCanvasMap->SetRightMargin(0.1);
  fCanvasMap->Divide(kNumberOfChannels,kNumberOfBoards,0,0);
  //TH2D *axis = new TH2D("axis","",100,-15,270,100,-50,1099);
  TH2D *axis = new TH2D("axis","",100,-kNumberOfSamples*0.1,kNumberOfSamples*1.1,100,-50,1099);
  axis->SetStats(0);
  axis->GetYaxis()->SetLabelSize(0.1);
  axis->GetXaxis()->SetLabelSize(0.1);
  //axis->SetTitleSize(50);
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
void DataMonitor::ReadPosition() {
  std::ifstream fin("./Position_Data/Last.log");
  fin >> fClosestCell;
  fin >> fPosX;
  fin >> fPosY;
  fin.close();
  std::cout << " * Position Read: " << fPosX << " " << fPosY << " " << fClosestCell.Data() << std::endl;
}
//====================
void DataMonitor::ReadConfig() {
  std::ifstream fin( "./Aquarium_Data/AquaConf.txt" );
  std::string line;
  for(int nlines=0;nlines!=9;++nlines) {
    std::getline(fin,line);
    std::istringstream iss(line);
    TString tmp;
    Int_t bid,ang;
    iss >> tmp >> bid >> ang;
    //bid -= 1;// ignoring first board label TO BE REMOVED
    if(bid<0||bid>kNumberOfBoards) continue;
    fBoardCode[bid] = tmp;
    fBoardAngle[bid] = ang;
    iss >> tmp >> fBoardTech[bid];
  }
  for(int i=0; i!=kNumberOfBoards; ++i) {
    std::cout << " | " << i << " " << fBoardCode[i] << " " << fBoardTech[i] << " " << fBoardAngle[i];
  }
  std::cout << std::endl;
}
//====================
void DataMonitor::ConfigureChannels() {
  for(int bd=0; bd!=kNumberOfBoards; ++bd) {
    TString cell = GetLocalCell(bd);
    fBoardCELL[bd] = cell;
    if(fDiagrams[bd])
      fDiagrams[bd]->SetTitle( Form("[%d]%s  %s;X [mm];Y [mm]",bd,fBoardTech[bd].Data(),cell.Data()) );
    //std::cout << std::endl;
  }
}
//====================
void DataMonitor::ModelPads(Int_t bd) {
  double x[100];
  double y[100];
  double gx = -fPitchX[bd][0]*fDREAMChannels[bd]/2; //0;
  double gy = -fPitchX[bd][0]*fDREAMChannels[bd]/2; //0;
  int widY = fPitchX[bd][0]*fDREAMChannels[bd];
  for(int str=0; str!=fDREAMChannels[bd]; ++str) {
    double xp = fPitchX[bd][str];
    double wd = 0.8*xp;
    double yp = fPeriodY[bd][str];
    double st = fStretch[bd][str];
    int nvert = (widY/yp*2+1)*2;
    for(int i=0; i!=nvert; ++i) {
      double sx, sy;
      if(i<(nvert/2)) {
	sx = (i%2)*st;
	sy = i*yp/2;
      } else {
	sx = ((i+1)%2)*st;
	sy = (nvert-i-1)*yp/2;
      }
      sx += (i/(nvert/2))*wd;
      x[i] = gx + sx;
      y[i] = sy + gy;
      //cout << x[i] << " | " << y[i] << endl;
    }
    if( fBoardAngle[bd]==0 ) {
      fDiagrams[bd]->AddBin(nvert,x,y);
    } else {
      fDiagrams[bd]->AddBin(nvert,y,x);
    }
    gx += xp;
  }
}
//====================
void DataMonitor::GetXYFromCell(TString mycell, Double_t &x, Double_t &y) {
  char cellstr[10] = {'A','B','C','D','E',
		      'F','G','H','I','J'};
  const char tmpC = mycell[0];
  TString tmpR = mycell[1];
  int row = tmpR.Atoi();
  int col = 0;
  for(int i=0; i!=10; ++i) {
    if(tmpC==cellstr[i])
      col = i;
  }
  x = -45 + 10*col;
  y = -45 + 10*row;
  return;
}
//====================
TString DataMonitor::GetCellName(Double_t x, Double_t y) {
  TString cell;
  char cellstr[10] = {'A','B','C','D','E',
		      'F','G','H','I','J'};
  int xx = x + 50;
  int yy = y + 50;
  if(xx<0) xx=0;
  if(yy<0) yy=0;
  xx = xx/10;
  yy = yy/10;
  if(xx>9) xx=9;
  if(yy>9) yy=9;
  cell = Form("%c%d",cellstr[xx],yy);
  return cell;
}
//====================
void DataMonitor::CreateDisplayConfiguration(TGCompositeFrame *mf) {
  TGCompositeFrame *mfR1 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
  mf->AddFrame(mfR1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  TGTextEdit *fConfigurationFile = new TGTextEdit(mfR1,300,250);
  mfR1->AddFrame(fConfigurationFile, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  TString sConfFile = "./Aquarium_Data/AquaConf.txt";
  fConfigurationFile->LoadFile(sConfFile.Data());
  fConfigurationFile->GetText();
  fConfigurationFile->Goto( fConfigurationFile->GetText()->RowCount() , 0 );

  TGCompositeFrame *mfR2 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
  mf->AddFrame(mfR2, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("config file",mfR2,1900,300,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapCF = new TCanvas("CanvasMapCF", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapCF);
  mfR2->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapCF->Divide(kNumberOfBoards,1,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    TVirtualPad *tmp = fCanvasMapCF->cd(i+1);
    ModelPads(i);
    fDiagrams[i]->Draw("COL");
  }

  TGCompositeFrame *mfR3 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
  mf->AddFrame(mfR3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  CreateHistory(mfR3);
}
//====================
void DataMonitor::CreateScans(TGCompositeFrame *mf) {
  TGCompositeFrame *mfR3 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
  mf->AddFrame(mfR3, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  TRootEmbeddedCanvas *embeddedCanvas2 = new TRootEmbeddedCanvas("overall scan",mfR3,1900,400,kSunkenFrame);
  Int_t cId2 = embeddedCanvas2->GetCanvasWindowId();
  fCanvasMapSC = new TCanvas("CanvasMapSC", 10, 10, cId2);
  embeddedCanvas2->AdoptCanvas(fCanvasMapSC);
  mfR3->AddFrame(embeddedCanvas2, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapSC->Divide(kNumberOfBoards,1,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    TVirtualPad *tmp = fCanvasMapSC->cd(i+1);
    fScan[i]->Draw("COL");
  }
}
//====================
void DataMonitor::CreateSignals(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("signalsplot",mf,1800,930,kSunkenFrame);
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
      fSignal[i][j]->Draw("HIST");
    }
  }
  fCanvasMapS->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateHeights(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("heightsplot",mf,1800,930,kSunkenFrame);
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
      fHeight[i][j]->Draw("HIST");
    }
  }
  fCanvasMapH->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateWidths(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("heightsplot",mf,1800,930,kSunkenFrame);
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
      fWidth[i][j]->Draw("HIST");
    }
  }
  fCanvasMapW->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateHitSummary(TGCompositeFrame *mf) {
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("hitsummaryplot",mf,1000,1000,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  fCanvasMapHS = new TCanvas("CanvasMapHS", 10, 10, cId);
  embeddedCanvas->AdoptCanvas(fCanvasMapHS);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  fCanvasMapHS->Divide(2,kNumberOfBoards/2,0,0);
  for(int i=0; i!=kNumberOfBoards; ++i) {
    TVirtualPad *tmp = fCanvasMapHS->cd(i+1);
    tmp->SetTopMargin(0.1);
    tmp->SetBottomMargin(0.1);
    tmp->SetLeftMargin(0.2);
    tmp->SetRightMargin(0.1);
    //fHitSummary[i]->Draw("HIST");
    fHitSummary[i]->Draw("E");
  }
  fCanvasMapHS->SetEditable(kFALSE);
}
*/
//====================
void DataMonitor::CreateRawWaves(TGCompositeFrame *mf) {
  TGTab *tabContainer = new TGTab(mf,96,26);
  mf->AddFrame(tabContainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));
  TGCompositeFrame *tab0;
  TGCompositeFrame *mf0;
  TGLabel *lab;
  for(int ibar=0; ibar!=4; ++ibar) {
    tab0 = tabContainer->AddTab( Form("BAR %d",ibar) );
    lab = new TGLabel(tab0, Form("BAR  %d",ibar) );
    tab0->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    mf0 = new TGCompositeFrame(tab0, 170, 20, kHorizontalFrame);
    tab0->AddFrame(mf0, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
    CreateRawWavesBAR(mf0,ibar);
  }
}
//====================
void DataMonitor::CreateRawWavesBAR(TGCompositeFrame *mf, Int_t ibar) {
  TGLabel *lab;
  TGCompositeFrame *mf0;
  TGCompositeFrame *mf1;
  for(int ihdr=0; ihdr!=2; ++ihdr) {
    mf0 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
    mf->AddFrame(mf0, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
    lab = new TGLabel(mf0, Form("HDR2  %d",ihdr) );
    mf0->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    mf1 = new TGCompositeFrame(mf0, 170, 20, kHorizontalFrame);
    mf0->AddFrame(mf1, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
    lab = new TGLabel(mf1, "LOW Gain" );
    mf1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    lab = new TGLabel(mf1, "HIGH Gain" );
    mf1->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
    CreateRawWavesHDR(mf0,ibar,ihdr);
  }
  mf0 = new TGCompositeFrame(mf, 170, 20, kVerticalFrame);
  mf->AddFrame(mf0, new TGLayoutHints(kLHintsTop | kLHintsExpandX,5,5,2,2));
  lab = new TGLabel(mf0, "HDR  0   ::   HDR  1" );
  mf0->AddFrame(lab, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX, 5, 5, 2, 2));
  CreateBarCorrelations(mf0,ibar);
}
//====================
void DataMonitor::CreateRawWavesHDR(TGCompositeFrame *mf, Int_t ibar, Int_t ihdr) {
  int idxCanvas = ibar*2 +ihdr;
  TCanvas *canv = fCanvasRawWaves[ibar][ihdr];
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("RawWaves2D",mf,640,480,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  canv = new TCanvas(Form("CanvasRawWavesHDR_%d",idxCanvas), 10, 10, cId);
  embeddedCanvas->AdoptCanvas(canv);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  canv->Divide(2,4,0,0);
  for(int i=0; i!=8; ++i) {
    TVirtualPad *tmp = canv->cd(i+1);
    tmp->SetTopMargin(0.1);
    tmp->SetBottomMargin(0.1);
    tmp->SetLeftMargin(0.2);
    tmp->SetRightMargin(0.1);
  }
  int idx0 = ibar*4 + ihdr*2 + 0;
  int idx1 = ibar*4 + ihdr*2 + 1;
  canv->cd(1); fSiPM_WaveRaw_2D[idx0]->Draw("COL");
  canv->cd(2); fSiPM_WaveRaw_2D[idx1]->Draw("COL");
  canv->cd(3); fSiPM_WaveRaw_WaProf[idx0]->Draw();
  canv->cd(4); fSiPM_WaveRaw_WaProf[idx1]->Draw();
  canv->cd(5); fSiPM_WaveRaw_PedMea[idx0]->Draw();
  canv->cd(6); fSiPM_WaveRaw_PedMea[idx1]->Draw();
  canv->cd(7); fSiPM_WaveRaw_PedSig[idx0]->Draw();
  canv->cd(8); fSiPM_WaveRaw_PedSig[idx1]->Draw();
  canv->Update();
  canv->SetEditable(kFALSE);

  canv = fCanvasSensors[ibar][ihdr];
  embeddedCanvas = new TRootEmbeddedCanvas("RawWaves2D",mf,640,210,kSunkenFrame);
  cId = embeddedCanvas->GetCanvasWindowId();
  canv = new TCanvas(Form("CanvasSensorsHDR_%d",idxCanvas), 10, 10, cId);
  embeddedCanvas->AdoptCanvas(canv);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  canv->Divide(1,2);
  canv->cd(1); fAmperimeter[ibar][ihdr]->Draw("A*L");
  canv->cd(2); fTermistor[ibar][ihdr]->Draw("A*L");
  fAmperimeter[ibar][ihdr]->SetTitle("Current through HDR2");
  fTermistor[ibar][ihdr]->SetTitle("Temperature of HDR2");
  canv->Update();
  canv->SetEditable(kFALSE);
}
//====================
void DataMonitor::CreateBarCorrelations(TGCompositeFrame *mf, Int_t ibar) {
  TCanvas *canv = fCanvasBarCorrelations[ibar];
  TRootEmbeddedCanvas *embeddedCanvas = new TRootEmbeddedCanvas("RawWaves2D",mf,640,720,kSunkenFrame);
  Int_t cId = embeddedCanvas->GetCanvasWindowId();
  canv = new TCanvas(Form("CanvasBarCorre_%d",ibar), 10, 10, cId);
  embeddedCanvas->AdoptCanvas(canv);
  mf->AddFrame(embeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,2,2));
  canv->Divide(1,3,0,0);
  for(int i=0; i!=3; ++i) {
    TVirtualPad *tmp = canv->cd(i+1);
    tmp->SetTopMargin(0.1);
    tmp->SetBottomMargin(0.1);
    tmp->SetLeftMargin(0.2);
    tmp->SetRightMargin(0.1);
  }
  canv->cd(1); fBar_CorrEner[ibar]->Draw("col");
  canv->cd(2); fBar_CorrTime[ibar]->Draw("col");
  canv->cd(3);
  fBar_ITimeRes[ibar]->Draw("H");
  fBar_ITimeRes10[ibar]->Draw("HSAME");
  fBar_ITimeRes30[ibar]->Draw("HSAME");
  fBar_ITimeRes50[ibar]->Draw("HSAME");
  fBar_ITimeRes[ibar]->Draw("HSAME");
  TLegend *leg = new TLegend(0.2,0.5,0.4,0.9);
  leg->AddEntry( fBar_ITimeResFit[ibar][0], Form("t_{10}  %.0f ps",fBar_ITimeResFit[ibar][0]->GetParameter(2)*1e3) );
  leg->AddEntry( fBar_ITimeResFit[ibar][1], Form("t_{20}  %.0f ps",fBar_ITimeResFit[ibar][1]->GetParameter(2)*1e3) );
  leg->AddEntry( fBar_ITimeResFit[ibar][2], Form("t_{30}  %.0f ps",fBar_ITimeResFit[ibar][2]->GetParameter(2)*1e3) );
  leg->AddEntry( fBar_ITimeResFit[ibar][3], Form("t_{50}  %.0f ps",fBar_ITimeResFit[ibar][3]->GetParameter(2)*1e3) );
  leg->Draw();
  canv->Update();
  canv->SetEditable(kFALSE);
}
//====================
void DataMonitor::RefreshAll() {
  for(int ibar=0; ibar!=4; ++ibar) {
    for(int ihdr=0; ihdr!=2; ++ihdr) {
      int idxCanvas = ibar*2 +ihdr;
      //fCanvasRawWaves[ibar][ihdr]->SetEditable(kTRUE);
      //fCanvasRawWaves[ibar][ihdr]->Modified();
      //fCanvasRawWaves[ibar][ihdr]->Update();
      //fCanvasRawWaves[ibar][ihdr]->SetEditable(kFALSE);
    }
  }
  /*
  if(fTabContainer->GetCurrent()==5&&fCanvasMapHI) {
    //std::cout << "   current canvas 6" << std::endl;
    fCanvasMapHI->SetEditable(kTRUE);
    for(int i=0; i!=kNumberOfBoards; ++i) {
      if(fNotInstalled[i]) continue;
      fCanvasMapHI->cd(i+1)->Modified();
      fCanvasMapHI->cd(i+1)->Update();
    }
    fCanvasMapHI->Modified();
    fCanvasMapHI->Update();
    fCanvasMapHI->SetEditable(kFALSE);
  }
  */
}
//====================
void DataMonitor::StyleH1(TH1 *tmp,Int_t col,Bool_t fill) {
  if(fill) tmp->SetFillColor(col);
  tmp->SetLineColor(col);
  tmp->SetStats(1);
  tmp->GetYaxis()->SetLabelSize(0.1);
  tmp->GetXaxis()->SetLabelSize(0.1);
  tmp->GetXaxis()->SetNdivisions(504);
  tmp->GetYaxis()->SetNdivisions(505);
}
//====================
void DataMonitor::ReCreateHistograms() {
  std::cout << "ReCreateHistograms INIT" << std::endl;
  //====== HISTOGRAMS
  Double_t x[1] = {0};
  Double_t y[1] = {0};
  for(int ibar=0; ibar!=4; ++ibar) {
    for(int ihdr=0; ihdr!=2; ++ihdr) {
      int idxHDR = ibar*4 + ihdr;
      for(int ichn=0; ichn!=2; ++ichn) {
	int idx = ibar*4 + ihdr*2 + ichn;
	//	fSiPM_WaveRaw_2D[idx] = new TH2D(Form("WaveRaw2D_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 Form("WaveRaw2D_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 100,0,200,100,-500,+500);
	//	fSiPM_WaveRaw_WaProf[idx] = new TProfile(Form("WaveRawWaProf_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 Form("WaveRawWaProf_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 100,-500,+500);
	fSiPM_WaveRaw_WaProf[idx] = fDRSChannel[idx]->CreateProfile();
	StyleH1(fSiPM_WaveRaw_WaProf[idx]);
	fSiPM_WaveRaw_PedMea[idx] = new TH1D(Form("WaveRawPedMea_B%d_H%d_C%d",ibar,ihdr,ichn),
					     Form("WaveRawPedMea_B%d_H%d_C%d",ibar,ihdr,ichn),
					     100,-500,+500);
	StyleH1(fSiPM_WaveRaw_PedMea[idx]);
	fSiPM_WaveRaw_PedSig[idx] = new TH1D(Form("WaveRawPedSig_B%d_H%d_C%d",ibar,ihdr,ichn),
					     Form("WaveRawPedSig_B%d_H%d_C%d",ibar,ihdr,ichn),
					     100,0,+20);
	StyleH1(fSiPM_WaveRaw_PedSig[idx]);
      }
      fAmperimeter[ibar][ihdr] = new TGraph(1,x,y);
      fTermistor[ibar][ihdr] = new TGraph(1,x,y);
    }
    fBar_CorrEner[ibar] = new TH2D(Form("BarCorrEner_B%d",ibar),
				   Form("BarCorrEner_B%d;HDR0    mV;HDR1    mV",ibar),
				   100,0,1000,100,0,1000);
    StyleH1(fBar_CorrEner[ibar]);
    fBar_CorrTime[ibar] = new TH2D(Form("BarCorrTime_B%d",ibar),
				   Form("BarCorrTime_B%d;HDR0    ns;HDR1    ns",ibar),
				   100,0,100,100,0,100);
    StyleH1(fBar_CorrTime[ibar]);
    fBar_ITimeRes[ibar] = new TH1D(Form("BarITimeRes_B%d",ibar),
				   Form("BarITimeRes_B%d;HDR1  -  HDR0    ns",ibar),
				   100,-2,2);
    StyleH1(fBar_ITimeRes[ibar],kBlue-3,false);
    fBar_ITimeRes10[ibar] = new TH1D(Form("BarITimeRes10_B%d",ibar),
				     Form("BarITimeRes10_B%d;HDR1  -  HDR0    ns",ibar),
				     100,-2,2);
    StyleH1(fBar_ITimeRes10[ibar],kCyan-3,false);
    fBar_ITimeRes30[ibar] = new TH1D(Form("BarITimeRes30_B%d",ibar),
				     Form("BarITimeRes30_B%d;HDR1  -  HDR0    ns",ibar),
				     100,-2,2);
    StyleH1(fBar_ITimeRes30[ibar],kOrange-3,false);
    fBar_ITimeRes50[ibar] = new TH1D(Form("BarITimeRes50_B%d",ibar),
				     Form("BarITimeRes50_B%d;HDR1  -  HDR0    ns",ibar),
				     100,-2,2);
    StyleH1(fBar_ITimeRes50[ibar],kMagenta-3,false);
    fBar_ITimeRes10[ibar]->Sumw2();
    fBar_ITimeRes[ibar]->Sumw2();
    fBar_ITimeRes30[ibar]->Sumw2();
    fBar_ITimeRes50[ibar]->Sumw2();
  }

  //fBinWidth = fSiPM_WaveRaw_2D[0]->GetBinLowEdge( fSiPM_WaveRaw_2D[0]->GetNbinsX() ) - fSiPM_WaveRaw_2D[0]->GetBinLowEdge( 1 );
  //fBinWidth /= fDRSChannel[0]->GetNbinsX();

  std::cout << "ReCreateHistograms DONE" << std::endl;
}
//====================
void DataMonitor::LinkData(Int_t run) {
  fDRS=NULL;
  fOSC[0]=NULL;
  fOSC[1]=NULL;
  fOSC[2]=NULL;
  fOSC[3]=NULL;
  //LinkV1742(run);
  LinkOSC(run);
}
//====================
void DataMonitor::LinkV1742(Int_t run) {
  //fDRS = new V1742DATReader("Run19461.dat");
  fDRS = new V1742DATReader("Run19435.dat");
  fDRS->ReadHeader();
  for(int i=0; i!=16; ++i) {
    fSiPM_WaveRaw_2D[i] = fDRS->GetSummaryPlot(i);
    fDRSChannel[i] = fDRS->GetTrace(i);
  }
}
//====================
void DataMonitor::LinkOSC(Int_t run) {
  //TString base = "XX-40V-9.0--00000.trc";
  //TString base = "XX-40V-7.0--00000.trc";
  //TString base = "XX-45V-7.0--00000.trc";
  //TString base = "XX-48V-7.0--00000.trc";

  //TString base = "47V-5.6--00000.trc";
  //TString base = "47V-5.8--00000.trc";
  //TString base = "47V-6.0--00000.trc";
  //TString base = "47V-6.2--00000.trc";
  TString base = "47V-6.4--00000.trc";
  //TString base = "47V-6.6--00000.trc";
  //TString base = "47V-6.8--00000.trc";
  //TString base = "47V-7.0--00000.trc";
  //TString base = "47V-7.5--00000.trc";
  //TString base = "47V-8.0--00000.trc";
  //TString base = "47V-8.5--00000.trc";
  fOSC[0] = new LECROYTRCReader(Form("/Users/cperez/Desktop/03-04-2021-NonIrradLASERSetup/C2--%s",base.Data()),-1000,+1000); //LOW
  fOSC[1] = new LECROYTRCReader(Form("/Users/cperez/Desktop/03-04-2021-NonIrradLASERSetup/C1--%s",base.Data()),-1000,+1000); //HIGH 
  fOSC[2] = new LECROYTRCReader(Form("/Users/cperez/Desktop/03-04-2021-NonIrradLASERSetup/C4--%s",base.Data()),-1000,+1000); //LOW
  fOSC[3] = new LECROYTRCReader(Form("/Users/cperez/Desktop/03-04-2021-NonIrradLASERSetup/C3--%s",base.Data()),-1000,+1000); //HIGH
  for(int i=0; i!=4; ++i) {
    fOSC[i]->ReadHeader();
    for(int j=0; j!=4; ++j) {
      fSiPM_WaveRaw_2D[j*4+i] = fOSC[i]->GetSummaryPlot(0);
      fDRSChannel[j*4+i] = fOSC[i]->GetTrace(0);
    }
  }
}
//====================
void DataMonitor::LinkSimulation() {
  for(int ibar=0; ibar!=4; ++ibar) {
    for(int ihdr=0; ihdr!=2; ++ihdr) {
      int idxHDR = ibar*4 + ihdr;
      for(int ichn=0; ichn!=2; ++ichn) {
	int idx = ibar*4 + ihdr*2 + ichn;
	fSiPM_WaveRaw_2D[idx] = new TH2D(Form("WaveRaw2D_B%d_H%d_C%d",ibar,ihdr,ichn),
					 Form("WaveRaw2D_B%d_H%d_C%d",ibar,ihdr,ichn),
					 100,0,200,100,-500,+500);
	//fSiPM_WaveRaw_WaProf[idx] = new TProfile(Form("WaveRawWaProf_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 Form("WaveRawWaProf_B%d_H%d_C%d",ibar,ihdr,ichn),
	//					 100,-500,+500);
	fDRSChannel[idx] = new WaveForm( Form("Simm_B%d_H%d_C%d",ibar,ihdr,ichn),
					 Form("Simm_B%d_H%d_C%d",ibar,ihdr,ichn),
					 1024,-0.5,1024-0.5 );
      }
    }
  }
}
//====================
void DataMonitor::OneSimEvent() {
  for(int ibar=0; ibar!=4; ++ibar) {
    for(int ihdr=0; ihdr!=2; ++ihdr) {
      int idx0 = ibar*4 + ihdr*2 + 0;
      int idx1 = ibar*4 + ihdr*2 + 1;
      Double_t errXLG = gRandom->Gaus(0,200); //ps
      Double_t errXHG = gRandom->Gaus(0,200); //ps
      for(int ibin=0; ibin!=1024; ++ibin) {
	Double_t errYLG = gRandom->Gaus(0,3); //mV
	Double_t errYHG = gRandom->Gaus(0,3); //mV
	Double_t x, y;
	Double_t HeightLG = 800;
	Double_t HeightHG = HeightLG*12;//fCalib_HLR[ibar][ihdr];
	x = ibin*fBinWidth + errXLG/1000.;
	y = HeightLG*TMath::Landau(x,50,13,kFALSE) + errYLG;
	y = 400-y;
	fDRSChannel[idx0]->SetBinContent( ibin+1, y );
	fSiPM_WaveRaw_2D[idx0]->Fill(x,y);
	x = ibin*fBinWidth + errXHG/1000.;
	y = HeightHG*TMath::Landau(x,50,5,kFALSE) + errYHG;
	y = 400-y;
	if(y>500) y=500;
	if(y<-500) y=-500;
	fSiPM_WaveRaw_2D[idx1]->Fill(x,y);
	fDRSChannel[idx1]->SetBinContent( ibin+1, y );
      }
    }
  }
}
//====================
void DataMonitor::EventLoop() {
  std::cout << "EVENT LOOP " << std::endl;
  Int_t nsamples = fDRSChannel[0]->GetNbinsX(); //1024 for DRS || dynamic for OSC
  Double_t fullrange = fDRSChannel[0]->GetBinLowEdge(nsamples) - fDRSChannel[0]->GetBinLowEdge(1);
  Int_t nsamplesIn20ns = 20*nsamples/fullrange;
  
  Double_t pedmea[16], pedsig[16];
  Double_t ampLG, thrHG, thrHG10, thrHG20, thrHG30, thrHG50;
  Int_t    timeLG;
  Double_t energy[4][2];
  Double_t time[4][2], time10[4][2], time30[4][2], time50[4][2];
  for(int nev=0;nev!=3000;++nev) {
    //for(int nev=0;;++nev) {
    if(fDRS) {
      if(!fDRS->ReadEvent()) break;
    } else if(fOSC[0]) {
      if(!fOSC[0]->ReadEvent()) break;
      if(!fOSC[1]->ReadEvent()) break;
      if(!fOSC[2]->ReadEvent()) break;
      if(!fOSC[3]->ReadEvent()) break;
    } else OneSimEvent();
    
    if(nev%500==0)
      std::cout << "Events so far: " << nev << std::endl;
    for(int i=0; i!=16; ++i) {
      fDRSChannel[i]->FillProfile();
      fDRSChannel[i]->ComputePedestal(1,50,pedmea[i],pedsig[i]);
      fSiPM_WaveRaw_PedMea[i]->Fill(pedmea[i]);
      fSiPM_WaveRaw_PedSig[i]->Fill(pedsig[i]);
    }
    for(int ibar=0; ibar!=4; ++ibar) {
      for(int ihdr=0; ihdr!=2; ++ihdr) {
	//std::cout << " HDR " << ihdr;
	int idx0 = ibar*4 + ihdr*2 + 0;
	int idx1 = ibar*4 + ihdr*2 + 1;
	// Look for min in low gain channel
	fDRSChannel[idx0]->ComputeMin(1,nsamples,ampLG,timeLG);
	//std::cout << " ampA:" << ampLG;
	//std::cout << " timeA:" << timeLG;
	ampLG = pedmea[idx0] - ampLG;
	//std::cout << " ampB:" << ampLG;
	energy[ibar][ihdr] = ampLG;
	thrHG = ampLG*fCalib_HLR[ibar][ihdr];
	//std::cout << " THRA:" << thrHG;
	thrHG10 = pedmea[idx1] - thrHG*0.1;
	thrHG20 = pedmea[idx1] - thrHG*0.2;
	thrHG30 = pedmea[idx1] - thrHG*0.3;
	thrHG50 = pedmea[idx1] - thrHG*0.5;
	//std::cout << " THRB:" << thrHG20;
	time[ibar][ihdr] = fDRSChannel[idx1]->FindCrossing(timeLG-3*nsamplesIn20ns,timeLG+nsamplesIn20ns,thrHG20,false);
	//std::cout << " TIME_SAMPLES:" << time[ibar][ihdr];
	time[ibar][ihdr] *= fullrange / nsamples;
	//std::cout << " E:" << energy[ibar][ihdr] << " T:" << time[ibar][ihdr] << std::endl;

	time10[ibar][ihdr] = fDRSChannel[idx1]->FindCrossing(timeLG-3*nsamplesIn20ns,timeLG+nsamplesIn20ns,thrHG10,false);
	time30[ibar][ihdr] = fDRSChannel[idx1]->FindCrossing(timeLG-3*nsamplesIn20ns,timeLG+nsamplesIn20ns,thrHG30,false);
	time50[ibar][ihdr] = fDRSChannel[idx1]->FindCrossing(timeLG-3*nsamplesIn20ns,timeLG+nsamplesIn20ns,thrHG50,false);
	time10[ibar][ihdr] *= fullrange / nsamples;
	time30[ibar][ihdr] *= fullrange / nsamples;
	time50[ibar][ihdr] *= fullrange / nsamples;
      }
      fBar_CorrEner[ibar]->Fill( energy[ibar][0], energy[ibar][1] );
      fBar_CorrTime[ibar]->Fill( time[ibar][0], time[ibar][1] );
      fBar_ITimeRes[ibar]->Fill( time[ibar][0] - time[ibar][1] );
      fBar_ITimeRes10[ibar]->Fill( time10[ibar][0] - time10[ibar][1] );
      fBar_ITimeRes30[ibar]->Fill( time30[ibar][0] - time30[ibar][1] );
      fBar_ITimeRes50[ibar]->Fill( time50[ibar][0] - time50[ibar][1] );
    }
  }

  for(int ibar=0; ibar!=4; ++ibar) {
    fBar_ITimeResFit[ibar][0] = CreateDeltaFit("fit10",fBar_ITimeRes10[ibar]);
    fBar_ITimeResFit[ibar][1] = CreateDeltaFit("fit20",fBar_ITimeRes[ibar]);
    fBar_ITimeResFit[ibar][2] = CreateDeltaFit("fit30",fBar_ITimeRes30[ibar]);
    fBar_ITimeResFit[ibar][3] = CreateDeltaFit("fit50",fBar_ITimeRes50[ibar]);

    fBar_ITimeRes10[ibar]->Fit("fit10","IEMNQ");
    fBar_ITimeRes[ibar]->Fit("fit20","IEMNQ");
    fBar_ITimeRes30[ibar]->Fit("fit30","IEMNQ");
    fBar_ITimeRes50[ibar]->Fit("fit50","IEMNQ");
  }

  
}
//====================
TF1* DataMonitor::CreateDeltaFit(TString name, TH1D *hist) {
  TF1 *ret = new TF1( name.Data(), "[0]*TMath::Gaus(x,[1],[2])"  );
  ret->SetLineColor( hist->GetLineColor() );
  ret->SetParameter(1,0);
  ret->SetParLimits(1,-1,+1);
  ret->SetParameter(2,0.1);
  ret->SetParLimits(2,0.001,2.0);
  return ret;
}
//====================
DataMonitor::DataMonitor(TApplication *app, Int_t run, UInt_t w, UInt_t h) {
  for(int ibar=0; ibar!=4; ++ibar)
    for(int ihdr=0; ihdr!=2; ++ihdr)
      fCalib_HLR[ibar][ihdr] = 430./100.;
  
  fApp = app;
  gStyle->SetTitleFontSize(0.1);
  fRun = run;
  fBinWidth = 0.2;
  LinkData(0);
  //if(fRun>0) LinkData(fRun);
  //else LinkSimulation();
  ReCreateHistograms();
  EventLoop();
  std::cout << "AAA" << std::endl;
  
  //====== WINDOWS
  fWindowSummary = new TGMainFrame(gClient->GetRoot(), 1900, 850);
  fWindowSummary->SetWindowName("General QA");
  fWindowSummary->SetWMPosition(0,0);

  CreateRawWaves(fWindowSummary);

  /*
  TGTab *tabContainer = new TGTab(fWindowSummary,96,26);
  fWindowSummary->AddFrame(tabContainer, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));
  TGCompositeFrame *tab0 = tabContainer->AddTab("Raw Waves");
  TGCompositeFrame *tab1 = tabContainer->AddTab("Wave Health Indicators");
  TGCompositeFrame *tab2 = tabContainer->AddTab("Correlations");
  std::cout << "AAA" << std::endl;
  CreateRawWaves(tab0);
  //CreateWaveIndicator(tab1);
  //CreateCorrelations(tab2);
  */
  
  fWindowSummary->MapSubwindows();
  fWindowSummary->Layout();
  fWindowSummary->MapWindow();
  fWindowSummary->Move(0,0);

  //tabContainer->SetTab(0);

  
  std::cout << "AAA" << std::endl;
  RefreshAll();
  std::cout << " DM: Please wait few seconds. Starting engine..." << std::endl;
}
//====================
DataMonitor::~DataMonitor() {
  fWindowSummary->Cleanup();
  fApp->Terminate();
}
