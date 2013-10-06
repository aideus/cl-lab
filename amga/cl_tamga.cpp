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
#include "cl_tamga_tools.h"
#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansambles=\n                Given ansambles (ans1.txt, ans2.txt)",
   "max_length=\n               Maximal length of result",
   "alphabet=SKI\n              Alphabet",
   "ignore=\n                   Symbols to ignore in result",
   "init_l=10\n                 Initial length of each of member",
   "psize=500\n                 The size of the population",
   "nchildren=auto\n            Number of children (if auto nchildre = psize * 2)",
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "pen_progsize=1\n            Penalty for program length",
   "pen_rezgood=0.1\n           Penalty for good results (for minimise length)",
   "pen_rezbad=10\n             Penalty for bad results (should be > pen_rezgood)",
   "p_exchange_mutation=0.05\n  probability of exchange mutation",
   "p_trim_mutation=0.05\n      probability of trim mutation",
   "crossover_type=1\n          0-alex, 1-sergey1",
   "postfix=none\n              postfix to every program",
   "valuator_type=a\n           Valuator type (a,s)",
   NULL
};

vector< vector<string> > ansambles; //ansambles
string alphabet,ignore;
size_t max_length;
size_t init_l, psize, nchildren;
size_t max_mem, max_steps;
int counter = 0;   //global counter (we use it in cl_term for assess memory usage)
double p_exchange_mutation, p_trim_mutation;
int crossover_type;
string postfix;
vector<cl_tamga_member*> population;
cl_ga_uniqchecker     uniqchecker;
string valuator_type;
double pen_progsize, pen_rezgood, pen_rezbad;

int iteration = 0; //iteration counter

void init(int argc, char*argv[]);
void create_population();
void eva_step();
void make_sex();
void extinction();
void print_best_result();

//make new cl_ga_member from cl_term
//cl_ga_member will delete term by itself after
cl_tamga_member* make_cl_tamga_member(cl_term* cl);
bool fun_for_sort_members(cl_tamga_member*m1, cl_tamga_member*m2);
void read_ansamble(string file, vector<string>& req);

int main(int argc, char*argv[])
{
   init(argc, argv);
   
   create_population();
   
   while(1)
     {
	eva_step();
     }
}
//                                                                          
void init(int argc, char*argv[])
{
   srandom(time(NULL));
   sp_parmap p(argc, argv, defv);
   alphabet   = p.get_s("alphabet");
   ignore     = p.get_s("ignore");
   init_l     = p.get_i("init_l");
   psize      = p.get_i("psize");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   crossover_type     = p.get_i("crossover_type");
   p_exchange_mutation = p.get_d("p_exchange_mutation");
   p_trim_mutation    = p.get_d("p_trim_mutation");
   max_length         = p.get_i("max_length");
   valuator_type      = p.get_s("valuator_type");
   pen_progsize       = p.get_d("pen_progsize");
   pen_rezgood        = p.get_d("pen_rezgood");
   pen_rezbad         = p.get_d("pen_rezbad");
   
   if (p.get_s("nchildren") == "auto")
     nchildren = psize * 2;
   else
     nchildren  = p.get_i("nchildren");
    
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
   
   ansambles.resize(p.get_vs("ansambles").size());
   for (size_t i = 0 ; i < p.get_vs("ansambles").size() ; i++)     
     read_ansamble(p.get_vs("ansambles")[i], ansambles[i]);
}
//                                                                          
void create_population()
{
   cl_brute_generator bg;
   bg.init(init_l, alphabet);
   while(population.size() < psize) 
     {
	cl_term* term = new cl_term(bg.create_random(), &counter);
	if (uniqchecker.try_to_add(term->conv2str()))
	  {
	     population.push_back(make_cl_tamga_member(term));
	  }
	else
	  {
	     delete term;
	  }
     }
   print_best_result();
}
//                                                                          
void eva_step()
{
   cout<<"iteration="<<(iteration++)<<endl;
   while (population.size() < psize + nchildren)
     {
	make_sex();
     }
   extinction();
   print_best_result();
}
//                                                                          
void make_sex()
{
   if (population.size() < psize)
     {
	cerr<<"Error | make_sex | internal error"<<endl;
	exit(EXIT_FAILURE);
     }
   size_t id1;
   size_t id2;
   do
     {
	id1 = random() % psize;
	id2 = random() % psize;	
     }
   while (id1 == id2);
   
   cl_term* t1 = new cl_term(*(population[id1]->term));
   cl_term* t2 = new cl_term(*(population[id2]->term));
   
   if (crossover_type == 0)
     t1->crossover_a(t2);     
   else if (crossover_type == 1)
     t1->crossover_s1(t2);
   else
     {
	cerr<<"Error | make_sex | unknown crossover type"<<endl;
	exit(EXIT_FAILURE);
     }
   
   if (p_exchange_mutation > 0)
     {
	t1->exchange_mutation(p_exchange_mutation, alphabet);
	t2->exchange_mutation(p_exchange_mutation, alphabet);
     }
   
   if (p_trim_mutation > 0)
     {
	t1->trim_mutation(p_trim_mutation);
	t2->trim_mutation(p_trim_mutation);
     }
   
   if (uniqchecker.try_to_add(t1->conv2str()))
     population.push_back(make_cl_tamga_member(t1));
   else
     delete t1;
   
   if (uniqchecker.try_to_add(t2->conv2str()))
     population.push_back(make_cl_tamga_member(t2));
   else
     delete t2;
}
//                                                                          
void extinction()
{   
   sort(population.begin(), population.end(), fun_for_sort_members);
   
   for (size_t i = psize ; i < population.size() ; i++)
     {
	uniqchecker.delete_string(population[i]->term_str);
	delete population[i];
     }
   population.resize(psize);
   int nc = 0;
   for (size_t i = 0 ; i < population.size() ; i++)
     if (population[i]->generation == iteration)
       nc++;
   cout<<"survived children = "<<nc<<endl;
}
//                                                                           
void print_best_result()
{
   cout<<"Best/Worse="<<population.at(0)->penalty<<"/"<<population.back()->penalty<<endl;
   
   
   cout<<population.at(0)->term_str<<endl;
   for (size_t i = 0 ; i < ansambles.size() ; i++)     
     {
	cout<<"ans N="<<i<<endl;
	for (size_t j = 0 ; j < ansambles[i].size() ; j++)
	  cout<<ansambles[i][j]<<" ----> "<<population.at(0)->rez[i][j]<<endl; 
     }
   cout<<"part_LP="<<population.at(0)->part_LP<<
     " part_Lgood="<<population.at(0)->part_Lgood<<
     " part_Lbad="<<population.at(0)->part_Lbad<<endl;
}
//                                                                           
cl_tamga_member* make_cl_tamga_member(cl_term* cl)
{
   cl_tamga_member* member = new cl_tamga_member;
   
   member->term = cl;
   member->term_str = cl->conv2str();
   member->rez.resize(ansambles.size());
   for (size_t i = 0 ; i < ansambles.size() ; i++)
     member->rez[i].resize(ansambles[i].size());
   
   bool is_hit_limits = false;
   for (size_t i = 0 ; i < ansambles.size() && !is_hit_limits ; i++)
     for (size_t j = 0 ; j < ansambles[i].size() && !is_hit_limits ; j++)
     {
	cl_term cl_calc(*cl);
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
	  is_hit_limits = true;
	
	member->rez[i][j] = resultator.get_rez();
	if (member->rez[i][j].size() >= max_length) //too long result
	  is_hit_limits = true;
     }
   if (!is_hit_limits) //ok
     {
	member->calc_penalty(valuator_type, pen_progsize, pen_rezgood, pen_rezbad);
     }
   else //we heat some limits (memory or number of steps)
     {
	member->penalty = 1e+100; //some big number if we hit limits (bad style... I know)
     }
   member->generation = iteration;
   return member;
}
//                                                                          
bool fun_for_sort_members(cl_tamga_member* m1, cl_tamga_member*m2)
{
   return m1->penalty < m2->penalty;
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
