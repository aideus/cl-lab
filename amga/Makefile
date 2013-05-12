
OPT = -O2 -pedantic -Wall -Werror -g -I../

OBJS = cl_amga_tools.o

all: cl_amga

.cpp.o:
	c++ -c $(OPT) $*.cpp -o $*.o


cl_amga:cl_amga.cpp $(OBJS) cl_amga_tools.o
	c++ $(OPT) ../*.o  cl_amga.cpp -o cl_amga $(OBJS)

clean:
	rm -f *.o cl_amga
