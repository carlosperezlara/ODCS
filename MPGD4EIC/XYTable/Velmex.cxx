#include "Velmex.h"
#include "StandardDeviceConnection.h"

#include <fstream>
#include <iostream>

ClassImp(Velmex);

//====================
void Velmex::Execute(TString cmd) {
  if(!fIsConnected) return;
  // C : Clear all commands from currently selected program
  // R : Run currently selected program
  TString fullCommand = "C,"+cmd+",R";
  Send(fullCommand);
  std::cout << "Execute: " << fullCommand.Data() << std::endl;
}
//====================
void Velmex::Connect() {
  // E : Enable On-Line mode with echo "on"
  // F : Enable On-Line mode with echo "off”
  if(fIsConnected) return;
  Send("F");
  fIsConnected = kTRUE;
}
//====================
void Velmex::Disconnect() {
  if(!fIsConnected) return;
  // Q : Quit On-Line mode (return to Local mode)
  Send("Q");
  fIsConnected = kFALSE;
}
//====================
void Velmex::Abort() {
  if(!fIsConnected) return;
  // D: Decelerate to a stop (interrupts current index/program in progress
  Send("D");
}
//====================
void Velmex::ExecuteMoveRelative(Int_t midx,Double_t nsteps) {
  // IAmMx: Set Absolute Index distance, m=motor# (1-4), x=+-1 to +-16777215 steps
  // IAmM0: Index motor to Absolute zero position, m=motor# (1-4)
  // IAmM-0: Zero motor position for motor# m (1-4)
  TString cmdstr;
  //cmdstr = Form("IA%dM-0,",fMotorIndex);
  //cmdstr += Form("IA%dM%d",fMotorIndex,nsteps);
  if(nsteps!=0)
    cmdstr += Form("I%dM%.3f",midx,nsteps);
  Execute(cmdstr);
}
//====================
void Velmex::ExecuteMoveRelative(Int_t midx,Double_t nsteps,Int_t midx2,Double_t nsteps2) {
  // IAmMx: Set Absolute Index distance, m=motor# (1-4), x=+-1 to +-16777215 steps
  // IAmM0: Index motor to Absolute zero position, m=motor# (1-4)
  // IAmM-0: Zero motor position for motor# m (1-4)
  TString cmdstr;
  //cmdstr = Form("IA%dM-0,",fMotorIndex);
  //cmdstr += Form("IA%dM%d",fMotorIndex,nsteps);
  if(nsteps!=0)
    cmdstr += Form("I%dM%.3f,",midx,nsteps);
  if(nsteps2!=0)
    cmdstr += Form("I%dM%.3f",midx2,nsteps2);
  Execute(cmdstr);
}
//====================
Velmex::Velmex(TString port) :
  StandardDeviceConnection(port,O_RDWR),
  fIsConnected(kFALSE)
{
}
//====================
Velmex::~Velmex() {
}
//====================
void Velmex::MoveRelative(Int_t midx, Double_t units) {
  ExecuteMoveRelative(midx,units*fStepsPerUnit[midx]);
}
//====================
void Velmex::MoveRelative(Int_t midx, Double_t units, Int_t midx2, Double_t units2) {
  ExecuteMoveRelative(midx, units*fStepsPerUnit[midx],
		      midx2,units2*fStepsPerUnit[midx2]);
}
//====================
TString Velmex::GetStatus() {
  if(!fIsConnected) return "";
  //Send("V");
  //TString ret = Receive(1);
  //return ret;
  return "";
}
//====================
TString Velmex::GetCurrentPosition(Int_t midx) {
  if(!fIsConnected) return "";
  switch(midx) {
  case(1):
    Send("X");
    break;
  case(2):
    Send("Y");
    break;
  case(3):
    Send("Z");
    break;
  case(4):
    Send("T");
    break;
  }
  TString ret = Receive();
  return ret;
}
