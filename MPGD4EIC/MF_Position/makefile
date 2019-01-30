all:
	rootcint -f Dict.cpp -c LinkDef.h
	g++ -o position MF_Position.cc MF_Position.cxx Dict.cpp `root-config --cflags --glibs`
	rm Dict*
