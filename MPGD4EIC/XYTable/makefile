all:
	rootcint -f Dict_xytable.cpp -c `root-config --cflags` -p MF_Position.h MC_Velmex.h StandardDeviceConnection.h LinkDef.h
	g++ -o bin/xytable gui.cc MF_Position.cxx MC_Velmex.cxx StandardDeviceConnection.cxx Dict_xytable.cpp `root-config --cflags --glibs`
	rm Dict_xytable.cpp
	mv Dict_xytable_rdict.pcm bin
