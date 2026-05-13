ROOT = ../..
LIBDIR = $(ROOT)/lib 
INCLUDE = $(ROOT)/include
LIBX11 = /usr/X11R6/lib

OUT = calib
LIBS = -lsequence -lransac -lransac -lba -lcalib -lgsl -lgslcblas -lfeatureslist -lrandom -lgeom -laux -lm -lceres -lglog -lgflags -pthread -std=c++17
CXX = g++
CXXFLAGS = -fopenmp -I$(INCLUDE) -L$(LIBDIR) -L$(LIBX11) -o $(OUT) $(LIBS)

install:
	$(CXX) main.cxx $(CXXFLAGS)
	cp $(OUT) $(ROOT)/bin

clean:
	rm -f *~ *% core $(OUT)



