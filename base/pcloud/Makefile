LIBDIR = ../../lib
INCLUDE = ../../include

CXX = g++
CXXFLAGS = -g -fopenmp -I$(INCLUDE)

HEADERS = pcloud.h 
OBJECTS = pcloud.o pccalib.o pcnlin.o
LIBNAME = libpcloud.a

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBNAME): $(HEADERS) $(OBJECTS)
	ar ruv $(LIBNAME) $(OBJECTS)
	ranlib $(LIBNAME)

install: $(LIBNAME)
	mv $(LIBNAME) $(LIBDIR)
	cp $(HEADERS) $(INCLUDE)

clean:
	rm -f *.o *~ $(LIBNAME)


