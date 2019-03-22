all:
	rootcint -f Dict_datamonitor.cpp -c `root-config --cflags` -I${ONLINE_MAIN}/include -I${MAPPING}/include -p DataMonitor.h LinkDef.h
	g++ -o bin/datamonitor gui.cc DataMonitor.cxx Dict_datamonitor.cpp -I${ONLINE_MAIN}/include -I${MAPPING}/include -L${ONLINE_MAIN}/lib -L${MAPPING}/build -L/home/eic/T1429/2019-03/lib -lmapping -lpmonitor -lEvent -lNoRootEvent -lmessage `root-config --cflags --glibs` -fpermissive
	rm Dict_datamonitor.cpp
	cp Dict_datamonitor_rdict.pcm bin
