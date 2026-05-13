ROOT = ../..
LIBDIR = $(ROOT)/lib 
INCLUDE = $(ROOT)/include

OUT = pclcalib
LIBS = -lpcloud -lrelax -lba -lcalib -lgsl -lgslcblas -lfeatureslist -lrandom -lgeom -laux -lm
CXX = g++
CXXFLAGS = -I$(INCLUDE) -L$(LIBDIR) -o $(OUT) $(LIBS)

install:
	$(CXX) main.cxx $(CXXFLAGS)
	cp $(OUT) $(ROOT)/bin

clean:
	rm -f *~ *% core $(OUT)



