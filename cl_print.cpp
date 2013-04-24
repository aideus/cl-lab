//cl_ga.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

//the first version of genetic algorithms


#include "sp_parmap.h"
#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>

const char* defv[] = {
   "in=\n                   Input string",
   "ignore=I\n              Symbols to ignore in result",
   "lout=1000\n             number of symbols to print",
   "max_mem=10000000\n      Maximal memory consumation during a computation (nterms)",
   "max_steps=10000000\n    Maximal reduction steps during a computation",
   "is_stepmode=0\n         Step by step mode",
   NULL
};


int main(int argc, char*argv[])
{
   sp_parmap p(argc, argv, defv);
   size_t max_steps = p.get_i("max_steps");
   size_t max_mem   = p.get_i("max_mem");
   
   int counter = 0; //global counter
   cl_term term(p.get_s("in"), &counter);
   
   if (p.get_b("is_stepmode"))
     {	
	for (size_t i = 0 ; i < max_steps ; i++)
	  {
	     int c = term.reduce_all(1, max_mem);	     
	     cout<<term.conv2str()<<endl<<endl;
	     if (c != -2) //not a step limit
	       return 0;
	  }
	return 0;
     }
   
   int code = term.reduce_all(p.get_i("max_steps"), p.get_i("max_mem"));
   
   cout<<"return code="<<code<<endl;
   
   string rez = term.conv2str(p.get_i("lout"));
   string rezi;
   string ignore = p.get_s("ignore");
   for (size_t i = 0 ; i < rez.size() ; i++)
     if (ignore.find(rez[i]) == string::npos )
       if (rez[i] != '(' && rez[i] != ')')
	 rezi.push_back(rez[i]);
   
   cout<<rez<<endl<<endl;
   cout<<rezi<<endl<<endl;
}
//                                                                          
