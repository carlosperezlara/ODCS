#include "Mitutoyo.h"
#include "StandardDeviceConnection.h"

#include <fstream>
#include <iostream>

#include <TString.h>

ClassImp(Mitutoyo);

//====================
Mitutoyo::Mitutoyo(TString port) {
  fDevice = new StandardDeviceConnection(port,2/*2=O_RDWR, 1=O_WRONLY*/);
  fDevice->Set7E1at4800();
}
//====================
Mitutoyo::~Mitutoyo() {
  delete fDevice;
  fDevice = 0;
}
//====================
void Mitutoyo::ReadXY(Int_t &x, Int_t &y) {
  // returns in microns
  fDevice->Flush();
  TString longstr = "";
  while(longstr.Length()<50) {
    longstr += fDevice->Receive(1);
  }
  //std::cout << longstr << std::endl;

  int st;

  st=longstr.First('X')+2;
  TString sx = "";
  for(int i=0; i!=9; ++i) {
    sx += longstr[st+i];
  }

  st=longstr.First('Y')+2;
  TString sy = "";
  for(int i=0; i!=9; ++i) {
    sy += longstr[st+i];
  }
  Double_t dx = sx.Atof();
  Double_t dy = sy.Atof();
  x = dx*1000;
  y = dy*1000;
  //std::cout << " XXX " << x << " YYY " << y << std::endl;
}
