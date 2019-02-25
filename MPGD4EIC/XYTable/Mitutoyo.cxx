#include "Mitutoyo.h"
#include "StandardDeviceConnection.h"

#include <fstream>
#include <iostream>

#include <TString.h>

ClassImp(Mitutoyo);

//====================
Mitutoyo::Mitutoyo(TString port) :
  StandardDeviceConnection(port,O_RDONLY)
{
  Set7E1at4800();
}
//====================
Mitutoyo::~Mitutoyo() {
}
//====================
void Mitutoyo::ReadXY(Int_t &x, Int_t &y) {
  if(fFileDescriptor<0) return;
  // returns in microns
  Flush();
  TString longstr = "";
  while(longstr.Length()<50) {
    longstr += Receive();
  }
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
}
