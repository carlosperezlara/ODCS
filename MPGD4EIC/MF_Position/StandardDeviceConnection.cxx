#include <iostream>
#include <TString.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "StandardDeviceConnection.h"

ClassImp(StandardDeviceConnection);

//======
StandardDeviceConnection::StandardDeviceConnection() {
  fDevice = "/dev/NULL";
  fFileDescriptor = -1;
  fMode = O_RDWR;
}
//======
StandardDeviceConnection::StandardDeviceConnection(TString dev, Int_t mode) {
  fDevice = dev;
  fFileDescriptor = -1;
  fMode = mode;
  Init();
}
//======
void StandardDeviceConnection::Init() {
  fFileDescriptor = open(fDevice.Data(), fMode);
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Init() cannot open device!" << std::endl;
  }
}
//======
void StandardDeviceConnection::Send(TString word) {
  if(fFileDescriptor<0) {
    std::cout << "StandardDeviceConnection::Send() no valid file descriptor, forgot to call Init()?" << std::endl;
    return;
  }
  ssize_t len = write(fFileDescriptor, word, word.Length());
  if(len!=word.Length()) {
    std::cout << "StandardDeviceConnection::Send() was interrupted!" << std::endl;
  }
}

/*
int main() {
  //StandardDeviceConnection test;
  //test.SetDeviceAddress("/dev/stdout");
  //test.SetDeviceAccessMode(O_WRONLY);
  //test.Init();

  StandardDeviceConnection test("/dev/stdout",O_WRONLY);
  test.Send("HOLA");

  std::cout << O_WRONLY << std::endl;

  return 0;
  char byte;
  int fFileDescriptor = open("/dev/cu.usbserial", O_RDWR);
  std::cout << fFileDescriptor << std::endl;
  write(fFileDescriptor, "A", 1);
  ssize_t size = read(fFileDescriptor, &byte, 1);
  printf("Read byte %c\n", byte);
  return 0;
}
*/
