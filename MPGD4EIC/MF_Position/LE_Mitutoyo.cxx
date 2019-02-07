#include "MC_Velmex.h"
#include "StandardDeviceConnection.h"

#include <fstream>
#include <iostream>

ClassImp(MC_Velmex);

//====================
void MC_Velmex::Execute(TString cmd) {
  if(!fIsConnected) return;
  // C : Clear all commands from currently selected program
  // R : Run currently selected program
  TString fullCommand = "C,"+cmd+",R";
  fDevice->Send(fullCommand);
}
//====================
void MC_Velmex::Connect() {
  // E : Enable On-Line mode with echo "on"
  // F : Enable On-Line mode with echo "off”
  if(fIsConnected) return;
  fDevice->Send("F");
  fIsConnected = kTRUE;
}
//====================
void MC_Velmex::Disconnect() {
  if(!fIsConnected) return;
  // Q : Quit On-Line mode (return to Local mode)
  fDevice->Send("Q");
  fIsConnected = kFALSE;
}
//====================
void MC_Velmex::Abort() {
  if(!fIsConnected) return;
  // D: Decelerate to a stop (interrupts current index/program in progress
  fDevice->Send("D");
}
//====================
void MC_Velmex::ExecuteMoveRelative(Int_t midx,Int_t nsteps) {
  // IAmMx: Set Absolute Index distance, m=motor# (1-4), x=+-1 to +-16777215 steps
  // IAmM0: Index motor to Absolute zero position, m=motor# (1-4)
  // IAmM-0: Zero motor position for motor# m (1-4)
  TString cmdstr;
  //cmdstr = Form("IA%dM-0,",fMotorIndex);
  //cmdstr += Form("IA%dM%d",fMotorIndex,nsteps);
  cmdstr += Form("I%dM%d",midx,nsteps);
  Execute(cmdstr);
}

//====================
MC_Velmex::MC_Velmex(TString port) {
  fDevice = new StandardDeviceConnection(port,2/*2=O_RDWR, 1=O_WRONLY*/);
  fIsConnected = kFALSE;
}
//====================
MC_Velmex::~MC_Velmex() {
  delete fDevice;
  fDevice = 0;
}