COMPILER=$(shell root-config --cxx)
FLAGS=$(shell root-config --cflags --libs)

trim: trim.cxx
		$(COMPILER) -g -O3 -Wall -Wextra -Wpedantic -o trim trim.cxx $(FLAGS) -lXMLIO -lquadmath

clean:
		rm -rf core  *.o trim
