LIBDIR = ../../lib
INCLUDE = ../../include

CXX = g++
CXXFLAGS = -fopenmp -g -I$(INCLUDE)

HEADERS = cameras.h homol_point.h rcontext.h relax_points.h relax_cameras.h 
OBJECTS = cameras.o homol_point.o rcontext.o relax_points.o relax_cameras.o 
LIBNAME = librelax.a


%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBNAME): $(HEADERS) $(OBJECTS)
	ar ruv $(LIBNAME) $(OBJECTS)
	ranlib $(LIBNAME)

install: $(LIBNAME)
	mv $(LIBNAME) $(LIBDIR)
	cp $(HEADERS) $(INCLUDE)

clean:
	rm -f *.o *~ *% $(LIBNAME)


