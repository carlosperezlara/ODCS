all:
	rootcint -f Dict_xytable.cpp -c `root-config --cflags` -p XYTable.h Velmex.h Mitutoyo.h StandardDeviceConnection.h LinkDef.h
	g++ -o tc testConnection.cc XYTable.cxx Velmex.cxx Mitutoyo.cxx StandardDeviceConnection.cxx Dict_xytable.cpp `root-config --cflags --glibs`
	g++ -o bin/xytable gui.cc XYTable.cxx Velmex.cxx Mitutoyo.cxx StandardDeviceConnection.cxx Dict_xytable.cpp `root-config --cflags --glibs`
	rm Dict_xytable.cpp
	cp Dict_xytable_rdict.pcm bin
