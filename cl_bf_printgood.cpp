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
   "mult=\n               print only string which mult times longer than program ",
   "alphabet=SKI\n        alphabet",
   "ignore=I\n            Symbols to ignore in result",
   "max_steps=5000\n      Maximal reduction steps during a computation",
   "max_mem=100000\n      Maximal memory consumation during a computation (nterms)",
   "minl=2\n              Minimal program length",
   "maxl=15\n             Maximal program length",
   "postfix=none\n        postfix to every program",
   "max_print=50\n        length to print for good results",
   "verbose=f\n           Print programs which hits limits",
   NULL
};



int main(int argc, char *argv[])
{
   sp_parmap p(argc, argv, defv);
   
   int mult        = p.get_i("mult");
   string alphabet = p.get_s("alphabet");
   string ignore   = p.get_s("ignore");
   int max_steps  = p.get_i("max_steps");
   int max_mem    = p.get_i("max_mem");
   int minl       = p.get_i("minl");
   int maxl       = p.get_i("maxl");
   bool  verbose  = p.get_b("verbose");
   int max_print  = p.get_i("max_print");
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
	     cl_resultator_length rz(l * mult, ignore);
	     int rez = term.reduce_all(max_steps, max_mem + counter, &rz);
	     if (rez == 1)  //good, length was achived
	       {
		  cl_term term_long(s, &counter);
		  cl_resultator_length rz_long(max_print, ignore);
		  term_long.reduce_all(max_steps, max_mem + counter, &rz_long);		  
		  cout<<s<<"   "<<rz_long.get_rez()<<endl;
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
