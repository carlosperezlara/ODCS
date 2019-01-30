#include <TGClient.h>
#include "MF_Position.h"
#include "MF_Position.cxx"

//====================
//====================
//====================
//====================
int main() {
  // Popup the GUI...
  new MF_Position(gClient->GetRoot(), 900, 800);
  return 0;
}
