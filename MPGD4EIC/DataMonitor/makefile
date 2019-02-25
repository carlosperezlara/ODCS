all:
	rootcint -f Dict_datamonitor.cpp -c `root-config --cflags` -I${ONLINE_MAIN}/include -p DataMonitor.h LinkDef.h
	g++ -o bin/datamonitor gui.cc DataMonitor.cxx Dict_datamonitor.cpp -I${ONLINE_MAIN}/include -L${ONLINE_MAIN}/lib -lpmonitor -lEvent -lNoRootEvent -lmessage `root-config --cflags --glibs`
	#g++ -o bin/datamonitor gui.cc -I${ONLINE_MAIN}/include -L${ONLINE_MAIN}/lib -lpmonitor -lEvent -lNoRootEvent -lmessage `root-config --cflags --glibs`
	rm Dict_datamonitor.cpp
	cp Dict_datamonitor_rdict.pcm bin
