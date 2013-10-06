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



#include "sp_parmap.h"
#include "cl_amga_tools.h"
#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansamble=\n                 Given ansamble",
   "prog=\n                     Given program",
   "alphabet=SKI\n              Alphabet",
   "ignore=\n                   Symbols to ignore in result",
   "max_length=1000\n           Maximal length of result",   
   "ac_K=-1\n                   ac_K (if < 0 then log2(N_uniq_combinator + 0.5))",
   "ac_Kxi=-1\n                 ac_Kxi (if < 0 then H(symbols in ansamble))",   
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "postfix=none\n              postfix to every program",
   NULL
};

vector<string> ansamble;
string alphabet,ignore, prog;
size_t max_mem, max_steps;
int counter = 0;   //global counter (we use it in cl_term for assess memory usage)
string postfix;
double ac_K, ac_Kxi;
size_t max_length;


void init(int argc, char*argv[]);
void calc_for_prog();
void read_ansamble(string file, vector<string>& req);

int main(int argc, char*argv[])
{
   init(argc, argv);   
   calc_for_prog();
}
//                                                                          
void init(int argc, char*argv[])
{
   srandom(time(NULL));
   sp_parmap p(argc, argv, defv);
   prog       = p.get_s("prog");
   alphabet   = p.get_s("alphabet");
   ignore     = p.get_s("ignore");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   ac_K               = p.get_d("ac_K");
   ac_Kxi             = p.get_d("ac_Kxi");
   max_length         = p.get_i("max_length");
   
   
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
   
   read_ansamble(p.get_s("ansamble"), ansamble);
}
//                                                                           
void calc_for_prog()
{
   cl_term cl(prog, &counter);
   vector<string> rez(ansamble.size());  
   bool is_hit_limits = false;
   for (size_t i = 0 ; i < ansamble.size() ; i++)
     {
	cl_term cl_calc(cl);
	if (!postfix.empty())
	  cl_calc.add_postfix(postfix);
	cl_calc.add_postfix(ansamble[i]);
	
	cl_resultator_length resultator(max_length + 1, ignore);

	int r = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	      
	if (r == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (r < 0) //we hit some of limits 
	  is_hit_limits = true;
	
	rez[i] = resultator.get_rez();
	if (rez[i].size() >= max_length) //too long result
	  is_hit_limits = true;
     }
   
   for (size_t i = 0 ; i < ansamble.size() ; i++)     
     cout<<ansamble[i]<<" ----> "<<rez[i]<<endl; 
   
   
   if (!is_hit_limits) //ok
     {
	cl_amga_valuator valuator;
	double penalty =  valuator.evaluate(rez, prog, ac_K, ac_Kxi);
	cout<<"penalty="<<penalty<<endl;
	cout<<"part_L1="<<valuator.part_L1<<" part_L2="<<valuator.part_L2<<" part_H="<<valuator.part_H<<endl;
	cout<<"ac_K="<<valuator.used_ac_K<<" ac_Kxi="<<valuator.used_ac_Kxi<<endl;
     }
   else //we heat some limits (memory or number of steps)
     {
	cout<<"limit was hit! penalty=1e+100"<<endl;
     }
}
//                                                                              
void read_ansamble(string file, vector<string>& req)
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
