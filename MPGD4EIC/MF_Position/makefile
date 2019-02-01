all:
	rootcint -f Dict.cpp -c `root-config --cflags` -p MF_Position.h MC_Velmex.h StandardDeviceConnection.h LinkDef.h
	g++ -o gui gui.cc MF_Position.cxx MC_Velmex.cxx StandardDeviceConnection.cxx Dict.cpp `root-config --cflags --glibs`
	rm Dict.cpp
