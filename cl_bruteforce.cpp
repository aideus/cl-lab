//cl_bruteforce.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_brute.h"
#include "sp_parmap.h"

const char* defv[] = {
   "req=\n                required string",
   "alphabet=SKI\n        alphabet",
   "ignore=I\n            Symbols to ignore in result",
   "max_steps=5000\n      Maximal reduction steps during a computation",
   "max_mem=100000\n      Maximal memory consumation during a computation (nterms)",
   "minl=2\n              Minimal program length",
   "maxl=15\n             Maximal program length",
   "postfix=none\n        postfix to every program",
   "verbose=f\n           Print programs which hits limits",
   NULL
};



int main(int argc, char *argv[])
{
   sp_parmap p(argc, argv, defv);
   
   string alphabet = p.get_s("alphabet");
   string ignore   = p.get_s("ignore");
   string required = p.get_s("req");
   int max_steps  = p.get_i("max_steps");
   int max_mem    = p.get_i("max_mem");
   int minl       = p.get_i("minl");
   int maxl       = p.get_i("maxl");
   bool  verbose  = p.get_b("verbose");
   string postfix = p.get_s("postfix"); 
   int counter     = 0;
   
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
   
   
   for (int l = minl ; l < maxl ; l++)
     {
	cout<<"l="<<l<<endl;
	cl_brute_generator bg;
	bg.init(l, alphabet);
	string s;
	while(bg.get_next(s))
	  {
	     s += postfix;
	     cl_term term(s, &counter);
	     cl_resultator_exact rz(required, ignore);
	     int steps_left = max_steps;
	     int rez = term.reduce_all(steps_left, max_mem, &rz);
	     if (rez == 1)
	       {
		  cout<<"found!!!!"<<endl<<s<<endl;
		  exit(0);
	       }
	     else if (rez == -2)
	       {
		  if (verbose)
		    cout<<"max steps limit:  "<<s<<endl;
	       }
	     else if (rez == -3)
	       {
		  if (verbose)
		    cout<<"max mem limit: "<<s<<endl;
	       }
	  }
     }
}
