all:
	rootcint -f Dict_daqcontrol.cpp -c `root-config --cflags` -p DAQControl.h LinkDef.h
	g++ -o bin/daqcontrol gui.cc DAQControl.cxx Dict_daqcontrol.cpp `root-config --cflags --glibs`
	rm Dict_daqcontrol.cpp
	cp Dict_daqcontrol_rdict.pcm bin
