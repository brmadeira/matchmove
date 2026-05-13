LIBDIR = ../../lib
INCLUDE = ../../include

CXX = g++
CXXFLAGS = -g -fopenmp -I$(INCLUDE)

HEADERS = sequence.h
OBJECTS = dlt.o ba.o subseq_alloc.o subseq.o seq_alloc.o seq.o inliers.o scale.o recalibrate.o
LIBNAME = libsequence.a

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


