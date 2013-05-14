
OPT = -O2 -pedantic -Wall -Werror -g -I../

OBJS = cl_amga_tools.o

all: cl_amga cl_amga_givenprog

.cpp.o:
	c++ -c $(OPT) $*.cpp -o $*.o


cl_amga:cl_amga.cpp $(OBJS) cl_amga_tools.o
	c++ $(OPT) ../*.o  cl_amga.cpp -o cl_amga $(OBJS)

cl_amga_givenprog:cl_amga_givenprog.cpp $(OBJS) cl_amga_tools.o
	c++ $(OPT) ../*.o  cl_amga_givenprog.cpp -o cl_amga_givenprog $(OBJS)

cl_amga_givenrez:cl_amga_givenrez.cpp $(OBJS) cl_amga_tools.o
	c++ $(OPT) ../*.o  cl_amga_givenrez.cpp -o cl_amga_givenrez $(OBJS)

clean:
	rm -f *.o cl_amga cl_amga_givenprog
