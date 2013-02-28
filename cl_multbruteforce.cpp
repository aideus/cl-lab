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
#include <fstream>

const char* defv[] = {
   "req=\n                file with required strings",
   "alphabet=SKI\n        alphabet",
   "ignore=\n             Symbols to ignore in result",
   "max_steps=5000\n      Maximal reduction steps during a computation",
   "max_mem=100000\n      Maximal memory consumation during a computation (nterms)",
   "minl=2\n              Minimal program length",
   "maxl=15\n             Maximal program length",
   "postfix=none\n        postfix to every program",
   "min_nf=500\n          Minimal length of non-finish results to be printed",
   "verbose=f\n           Print programs which hits limits",
   NULL
};

void read_required(string file, vector<string>& req);
void remove_founds(vector<string>& required, const vector<string>& founds);

int main(int argc, char *argv[])
{
   sp_parmap p(argc, argv, defv);
   
   string alphabet = p.get_s("alphabet");
   string ignore   = p.get_s("ignore");
   int max_steps  = p.get_i("max_steps");
   int max_mem    = p.get_i("max_mem");
   int minl       = p.get_i("minl");
   int maxl       = p.get_i("maxl");
   size_t min_nf  = p.get_i("min_nf");
//   bool  verbose  = p.get_b("verbose");
   string postfix = p.get_s("postfix"); 
   int counter     = 0;
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
   
   vector<string> required;
   read_required(p.get_s("req"),required);
   
   
   for (size_t l = minl ; (int)l < maxl ; l++)
     {
	cout<<"l="<<l<<endl;
	cl_brute_generator bg;
	bg.init(l, alphabet);
	string s;
	while(bg.get_next(s))
	  {
	     s += postfix;
	     cl_term term(s, &counter);
	     cl_resultator_multexact rz(required, ignore);
	     int rez = term.reduce_all(max_steps, max_mem, &rz);
	     if (rz.get_founds().size() > 0) //we've found something!!!! Wow!
	       {
		  for (size_t i = 0 ; i < rz.get_founds().size() ; i++)
		    cout<<"FOUND: "<<s<<"  "<<rz.get_founds()[i]<<endl;
		  remove_founds(required, rz.get_founds());
		  if (required.size() == 0)
		    return 0;
	       }
	     //if we hit some limits, and have non-finished results
	     //we print them if they are big enougth
	     if (rez != 0 && rz.get_rez().size() >= max(min_nf,l + 1)) 
	       {
		  vector<string> nf;	     
		  rz.get_not_finished(nf);
		  for (size_t i = 0 ; i < nf.size() ; i++)
		    {
		       cout<<"NF: "<<s<<" "<<rz.get_rez()<<" "<<nf[i]<<endl;
		    }
	       }
	  }
     }
}

//                                                                          
void read_required(string file, vector<string>& req)
{
   ifstream in(file.c_str());
   req.clear();
   string s;
   while(in>>s)
     {
	req.push_back(s);
     }
}
//                                                              
void remove_founds(vector<string>& required, const vector<string>& founds)
{
   for (size_t i = 0 ; i < founds.size() ; i++)
     {
	int idx = -1;
	for (size_t j = 0 ; j < required.size() ; j++)
	  if (required[j] == founds[i]) //good
	    {
	       if (idx != -1)
		 {
		    cerr<<"Error | remove_founds | internal error"<<endl;
		    exit(EXIT_FAILURE);
		 }
	       idx = j;
	    }
	if (idx == -1)
	  {
	     cerr<<"Error | remove_founds | internal error"<<endl;
	     exit(EXIT_FAILURE);	     
	  }
	required.erase(required.begin() + idx);
     }
 /*  cout<<"RB:"<<endl;
   for (size_t i = 0 ; i < required.size() ; i++)
     {
	cout<<required[i]<<endl;
     }
   cout<<"RF:"<<endl;
*/
}
