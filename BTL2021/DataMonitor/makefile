all:
	rootcint -f Dict_datamonitor.cpp -c `root-config --cflags` -p DataMonitor.h WaveForm.h Reader.h V1742DATReader.h LECROYTRCReader.h LinkDef.h
	g++ -o bin/datamonitor gui.cc DataMonitor.cxx WaveForm.cxx Reader.cxx V1742DATReader.cxx LECROYTRCReader.cxx Dict_datamonitor.cpp `root-config --cflags --glibs` -fpermissive
	rm Dict_datamonitor.cpp
	cp Dict_datamonitor_rdict.pcm bin
