
OPT = -O2 -pedantic -Wall -Werror 

OBJS = cl_base.o cl_brute.o cl_ga_tools.o cl_resultator.o sp_parmap.o 

all: cl_ga cl_interactive cl_print cl_bruteforce cl_multbruteforce

.cpp.o:
	c++ -c $(OPT) $*.cpp -o $*.o

cl_ga:cl_ga.cpp $(OBJS)
	c++ $(OPT)  cl_ga.cpp -o cl_ga $(OBJS)

cl_interactive:cl_interactive.cpp $(OBJS)
	c++ $(OPT) cl_interactive.cpp -o cl_interactive $(OBJS)

cl_print:cl_print.cpp $(OBJS)
	c++ $(OPT)  cl_print.cpp -o cl_print $(OBJS)

cl_bruteforce:cl_bruteforce.cpp $(OBJS)
	c++ $(OPT)  cl_bruteforce.cpp -o cl_bruteforce $(OBJS)

cl_multbruteforce:cl_multbruteforce.cpp $(OBJS)
	c++ $(OPT)  cl_multbruteforce.cpp -o cl_multbruteforce $(OBJS)

clean:
	rm -f *.o cl_ga cl_interactive cl_print cl_bruteforce cl_multbruteforce
