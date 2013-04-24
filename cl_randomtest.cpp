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
   "alphabet=SKI\n        alphabet",
   "max_steps=5000\n      Maximal reduction steps during a computation",
   "max_mem=100000\n      Maximal memory consumation during a computation (nterms)",
   "nrun=10000\n          number of runs",
   "len=15\n              Maximal program length",
   "seed=314\n            random seed",
   "is_steptest=0\n       is_steptest",
   NULL
};


int main(int argc, char *argv[])
{
   sp_parmap p(argc, argv, defv);
   
   string alphabet = p.get_s("alphabet");
   int max_steps  = p.get_i("max_steps");
   int max_mem    = p.get_i("max_mem");
   int len        = p.get_i("len");
   int nrun       = p.get_i("nrun");
   int is_steptest = p.get_i("is_steptest");
   if (p.get_i("seed") <= 0)
     srandom(time(NULL));
   else
     srandom(p.get_i("seed"));
   
   int counter     = 0;
   
   cl_brute_generator bg;
   bg.init(len, alphabet);
   int print_len = 10000;
   for (int i = 0 ; i < nrun ; i++)
     {
	string s = bg.create_random();
	cl_term term(s, &counter);
	cl_resultator_length res(print_len , "");
	int rez;
	if (is_steptest)
	  {
	     rez = -2; //max steps was hit
	     for (int ii = 0 ; ii < max_steps && rez == -2 ; ii++)
	       
	       {
		  rez = term.reduce_all(1, max_mem, &res);
	       }
	     cout<<s<<" "<<rez<<" <> "<<term.conv2str(print_len)<<endl;

	  }
	else
	  {
	     rez = term.reduce_all(max_steps, max_mem, &res);
	     cout<<s<<" "<<rez<<" <"<<res.get_rez()<<"> "<<term.conv2str(print_len)<<endl;
	  }
     }
}

