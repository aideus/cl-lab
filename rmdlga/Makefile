
OPT = -O2 -pedantic -Wall -Werror -g -I../

OBJS = cl_rmdlga_tools.o cl_strga.o cl_rmdlga2_tools.o

all: cl_rmdlga cl_rmdlga2

.cpp.o:
	c++ -c $(OPT) $*.cpp -o $*.o


cl_rmdlga:cl_rmdlga.cpp $(OBJS) 
	c++ $(OPT) ../*.o  cl_rmdlga.cpp -o cl_rmdlga $(OBJS)

cl_rmdlga2:cl_rmdlga2.cpp $(OBJS) 
	c++ $(OPT) ../*.o  cl_rmdlga2.cpp -o cl_rmdlga2 $(OBJS)

clean:
	rm -f *.o cl_rmdlga cl_rmdlga2
