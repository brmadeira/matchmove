ROOT = ../..
LIBDIR = $(ROOT)/lib 
INCLUDE = $(ROOT)/include

OUT = debug
LIBS = -lpcloud -lrelax -lba -lcalib -lgsl -lgslcblas -lfeatureslist -lrandom -lgeom -laux -lm
CXX = g++
CXXFLAGS = -I$(INCLUDE) -L$(LIBDIR) -o $(OUT) $(LIBS) `pkg-config --cflags --libs opencv4`

install:
	$(CXX) main.cxx $(CXXFLAGS)
	cp $(OUT) $(ROOT)/bin

clean:
	rm -f *~ *% core $(OUT)



