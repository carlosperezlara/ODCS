#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "StandardDeviceConnection.h"

int main() {
  if(0) {
    StandardDeviceConnection test("/dev/stdout",O_WRONLY);
    test.Send("HOLA");
  }

  if(0) { // mitutoyo
    StandardDeviceConnection test("/dev/cu.usbserial",O_RDONLY);
    test.SetBPS(4800);
    TString str;
    for(int i=0; i!=180; ++i) {
      str = test.Receive(1);
      std::cout << str.Data();
    }
    std::cout << std::endl;
  }

  if(1) { // /dev/zero
    StandardDeviceConnection test("/dev/urandom",O_RDONLY);
    //test.SetBPS(4800);
    TString str;
    for(int i=0; i!=180; ++i) {
      str = test.Receive(1);
      std::cout << str.Data();
    }
    std::cout << std::endl;
  }

  return 0;
}

