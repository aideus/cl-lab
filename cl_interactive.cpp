//cl_interactive.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (pas.aicv@gmail.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_base.h"
#include <sstream>

int counter = 0;

int main(int argc, char*argv[])
{
   while (1)
     {
	cout<<"> ";
	string str;
	std::getline(std::cin, str);
	cout<<str<<endl;
	int steps = 10000000;
	cl_term term(str, &counter);
	int rez = term.reduce_all(steps, 10000000);	
	cout<<"return code = "<<rez<<endl;
	ostringstream out;
	term.rec_print(out);
	cout<<out.str().substr(0, 1000)<<endl;
     }
}
