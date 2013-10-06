//cl_tamga_test.cpp
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
#include "cl_tamga_tools.h"
#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "in=\n                       string to test",
   "ansambles=\n                Given ansambles (ans1.txt, ans2.txt)",
   "max_length=\n               Maximal length of result",
   "alphabet=SKI\n              Alphabet",
   "ignore=\n                   Symbols to ignore in result",
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "pen_progsize=1\n            Penalty for program length",
   "pen_rezgood=0.1\n           Penalty for good results (for minimise length)",
   "pen_rezbad=10\n             Penalty for bad results (should be > pen_rezgood)",
   "postfix=none\n              postfix to every program",
   "valuator_type=a\n           Valuator type (a,s)",
   NULL
};

vector< vector<string> > ansambles; //ansambles
string alphabet,ignore;
size_t max_length;
size_t max_mem, max_steps;
int counter = 0 ;   //global counter (we use it in cl_term for assess memory usage)
string postfix;
cl_ga_uniqchecker     uniqchecker;
string valuator_type;
double pen_progsize, pen_rezgood, pen_rezbad;
string in_str;


void init(int argc, char*argv[]);

void read_ansamble(string file, vector<string>& req);

int main(int argc, char*argv[])
{
   init(argc, argv);
   
   //almost make_cl_tamga_member but with some modifications:
   
   cl_tamga_member* member = new cl_tamga_member;
   
   member->term = new cl_term(in_str, &counter);
   member->term_str = member->term->conv2str();
   member->rez.resize(ansambles.size());
   for (size_t i = 0 ; i < ansambles.size() ; i++)
     member->rez[i].resize(ansambles[i].size());
   
   int num_hit_limits = 0;
   for (size_t i = 0 ; i < ansambles.size()  ; i++)
     for (size_t j = 0 ; j < ansambles[i].size()  ; j++)
     {
	cl_term cl_calc(*member->term);
	if (!postfix.empty())
	  cl_calc.add_postfix(postfix);
	cl_calc.add_postfix(ansambles[i][j]);
	
	cl_resultator_length resultator(max_length + 1, ignore);

	int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	
      
	if (rez == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (rez < 0) //we hit some of limits 
	  num_hit_limits++;
	else
	  {
	     member->rez[i][j] = resultator.get_rez();
	     if (member->rez[i][j].size() >= max_length) //too long result
	       num_hit_limits++;
	  }
     }
   member->calc_penalty(valuator_type, pen_progsize, pen_rezgood, pen_rezbad);   

   cout<<in_str<<endl;
   for (size_t i = 0 ; i < ansambles.size() ; i++)     
     {
	cout<<"ans N="<<i<<endl;
	for (size_t j = 0 ; j < ansambles[i].size() ; j++)
	  cout<<ansambles[i][j]<<" ----> "<<member->rez[i][j]<<endl; 
     }
   
   if (num_hit_limits)
     {
	cout<<"!!!ATTENTION!!! num_hit_limits is not zero!!!!"<<endl;
	cout<<"num_hit_limits="<<num_hit_limits<<endl;
     }
   cout<<"part_LP="<<member->part_LP<<
     " part_Lgood="<<member->part_Lgood<<
     " part_Lbad="<<member->part_Lbad<<endl;   
   
   size_t ngood, lgood, nbad, lbad;
   cl_tamga_rez_analysis(member->rez, ngood, lgood, nbad,  lbad);
   
   cout<<"ngood="<<ngood<<" nbad="<<nbad<<endl;
}
//                                                                          
void init(int argc, char*argv[])
{
   srandom(time(NULL));
   sp_parmap p(argc, argv, defv);
   alphabet   = p.get_s("alphabet");
   ignore     = p.get_s("ignore");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   max_length         = p.get_i("max_length");
   valuator_type      = p.get_s("valuator_type");
   pen_progsize       = p.get_d("pen_progsize");
   pen_rezgood        = p.get_d("pen_rezgood");
   pen_rezbad         = p.get_d("pen_rezbad");
   in_str             = p.get_s("in");
       
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
   
   ansambles.resize(p.get_vs("ansambles").size());
   for (size_t i = 0 ; i < p.get_vs("ansambles").size() ; i++)     
     read_ansamble(p.get_vs("ansambles")[i], ansambles[i]);
   
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
