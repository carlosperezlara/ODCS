#include "StandardDeviceConnection.h"
#include "StandardDeviceConnection.cxx"
#include "MC_Velmex.h"
#include "MC_Velmex.cxx"
#include "MF_Position.h"
#include "MF_Position.cxx"

//====================
//====================
//====================
//====================
int Run_MF_Position() {
  // Popup the GUI...
  new MF_Position(new TApplication("gui",0,0), 900, 800);
  return 0;
}
