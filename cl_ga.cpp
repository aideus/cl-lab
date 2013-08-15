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
   "req=\n                      Required rezult",
   "alphabet=SKI\n              Alphabet",
   "ignore=I\n                  Symbols to ignore in result",
   "init_l=10\n                 Initial length of each of member",
   "psize=500\n                 The size of the population",
   "nchildren=1000\n            Number of children",
   "pen_progsize=1\n            Penalty for size of the program (per symbol)",
   "pen_wrongrez=10\n           Penalty for each wrong symbol in the result",
   "pen_absentrez=10\n          Penalty for each empty symbol in the result",
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "p_exchange_mutation=0.05\n  probability of exchange mutation",
   "p_trim_mutation=0.05\n      probability of trim mutation",
   "crossover_type=1\n          0-alex, 1-sergey1",
   "postfix=none\n              postfix to every program",
   NULL
};

string req_rez, alphabet,ignore;
size_t init_l, psize, nchildren;
cl_ga_valuator* valuator;
size_t max_mem, max_steps;
int counter;   //global counter (we use it in cl_term for assess memory usage)
double p_exchange_mutation, p_trim_mutation;
int crossover_type;
string postfix;

vector<cl_ga_member*> population;
cl_ga_uniqchecker     uniqchecker;

int iteration = 0; //iteration counter

void init(int argc, char*argv[]);
void create_population();
void eva_step();
void make_sex();
void extinction();
void print_best_result();

//make new cl_ga_member from cl_term
//cl_ga_member will delete term by itself after
cl_ga_member* make_cl_ga_member(cl_term* cl);
bool fun_for_sort_members(cl_ga_member*m1, cl_ga_member*m2);

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
   req_rez    = p.get_s("req");
   alphabet   = p.get_s("alphabet");
   ignore     = p.get_s("ignore");
   init_l     = p.get_i("init_l");
   psize      = p.get_i("psize");
   nchildren  = p.get_i("nchildren");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   crossover_type     = p.get_i("crossover_type");
   p_exchange_mutation = p.get_d("p_exchange_mutation");
   p_trim_mutation    = p.get_d("p_trim_mutation");
   
   valuator = new cl_ga_valuator(p.get_i("pen_progsize"), 
				 p.get_i("pen_wrongrez"), 
				 p.get_i("pen_absentrez"));
   postfix = p.get_s("postfix");
   if (postfix == "none")
     postfix.clear();
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
	     population.push_back(make_cl_ga_member(term));
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
     population.push_back(make_cl_ga_member(t1));
   else
     delete t1;
   
   if (uniqchecker.try_to_add(t2->conv2str()))
     population.push_back(make_cl_ga_member(t2));
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
   
   cl_term cl_calc(*(population.at(0)->term));
   if (!postfix.empty())
     cl_calc.add_postfix(postfix);

   cl_resultator_length resultator(int(req_rez.size() * 1.5) + 1 , ignore);
   cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
   string long_rez = resultator.get_rez();
   
   cout<<population.at(0)->term_str<<endl;
   cout<<population.at(0)->rez<<" ("<<long_rez.substr(min(req_rez.size(),long_rez.size()), string::npos)<<")"<<endl;
   
/*   if (iteration == 100)
     {
	for (size_t i = 0 ; i < population.size(); i++)
	  
	  {
	     cout<<i<<" "<<population[i]->term_str<<" "<<population[i]->penalty<<endl;
	     cout<<population[i]->rez<<endl<<endl;
	  }	
	exit(0);
     }*/
}
//                                                                           
cl_ga_member* make_cl_ga_member(cl_term* cl)
{
   cl_ga_member* member = new cl_ga_member;
   
   member->term = cl;
   member->term_str = cl->conv2str();
   
   cl_term cl_calc(*cl);
   if (!postfix.empty())
     cl_calc.add_postfix(postfix);
   cl_resultator_length resultator(req_rez.size(), ignore);

   int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
   
   member->rez = resultator.get_rez();
      
   if (rez == -1) //it's imposible
     {
	cerr<<"error | make_cl_ga_member | internal error"<<endl;
	exit(EXIT_FAILURE);
     }
   
   if (rez >= 0) //ok
     member->penalty = valuator->evaluate(member->term_str.size(), member->rez, req_rez);
   else
     member->penalty = 1e+100; //some big number if we hit limits (bad style... I know)
      
   member->generation = iteration;
   return member;
}
//                                                                          
bool fun_for_sort_members(cl_ga_member* m1, cl_ga_member*m2)
{
   return m1->penalty < m2->penalty;
}
