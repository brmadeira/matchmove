LIBDIR = ../../lib
INCLUDE = ../../include

CXX = g++
CXXFLAGS = -g -fopenmp -I$(INCLUDE) -I/usr/include/eigen3

HEADERS = ba.h ba_gsl.h
OBJECTS = ba_gsl.o ba_ext.o ba_ext_coder.o rotations.o
LIBNAME = libba.a

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBNAME): $(HEADERS) $(OBJECTS)
	ar ruv $(LIBNAME) $(OBJECTS)
	ranlib $(LIBNAME)

install: $(LIBNAME)
	mv $(LIBNAME) $(LIBDIR)
	cp $(HEADERS) $(INCLUDE)

clean:
	rm -f *.o *~ *% $(LIBNAME)


