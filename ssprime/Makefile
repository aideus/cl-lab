
OPT = -O2 -pedantic -Wall -Werror -g -I../

OBJS = cl_ssprimega_tools.o 

all: cl_ssprimega

.cpp.o:
	c++ -c $(OPT) $*.cpp -o $*.o


cl_ssprimega:cl_ssprimega.cpp $(OBJS) 
	c++ $(OPT) ../*.o  cl_ssprimega.cpp -o cl_ssprimega $(OBJS)


clean:
	rm -f *.o cl_ssprimega
