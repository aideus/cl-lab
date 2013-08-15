//cl_rmdlga.cpp
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
#include "cl_rmdlga_tools.h"
#include "cl_strga.h"
//#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansamble=\n                 Given ansamble",
   "alphabet=SKI\n              Alphabet",
   "ignore=\n                   Symbols to ignore in result",   
   "init_l=10\n                 Initial length of each of member",
   "init_yi_l=5\n               Initial length of yi",
   "psize=500\n                 The size of the population",
   "nchildren=auto\n            Number of children (if auto nchildre = psize * 2)",
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "p_echange_mutation=0.05\n   probability of echange mutation",
   "p_trim_mutation=0.05\n      probability of trim mutation",
   "p_yitrim_mutation=0.1\n     probability of yi trim mutation",
   "pen_progsize=1\n            Penalty for size of the program (per symbol)",
   "pen_wrongrez=10\n           Penalty for each wrong symbol in the result",
   "pen_absentrez=10\n          Penalty for each empty symbol in the result",   
   "valuator_type=\n            Valuator type (s or a)",
   "crossover_type=1\n          0-alex, 1-sergey1",
   NULL
};

vector<string> ansamble;
string alphabet,ignore;
size_t init_l, init_yi_l, psize, nchildren;
size_t max_mem, max_steps;
int counter;   //global counter (we use it in cl_term for assess memory usage)
double p_echange_mutation, p_trim_mutation, p_yitrim_mutation;
double pen_progsize, pen_wrongrez, pen_absentrez;
string ans_alphabet; //alphabet of ansambler
string valuator_type;

int crossover_type;

vector<cl_rmdlga_member*> population;
cl_rmdlga_uniqchecker     uniqchecker;

int iteration = 0; //iteration counter

void init(int argc, char*argv[]);
void create_population();
void eva_step();
void make_sex();
void extinction();
void print_best_result();

//make new cl_ga_member from cl_term
//cl_ga_member will delete term by itself after
cl_rmdlga_member* make_cl_rmdlga_member(cl_term* cl, const vector<string>& yi);
bool fun_for_sort_members(cl_rmdlga_member*m1, cl_rmdlga_member*m2);
void read_ansamble(string file, vector<string>& req);
void find_ans_alphabet();

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
   init_yi_l  = p.get_i("init_yi_l");
   psize      = p.get_i("psize");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   crossover_type     = p.get_i("crossover_type");
   p_echange_mutation = p.get_d("p_echange_mutation");
   p_trim_mutation    = p.get_d("p_trim_mutation");
   p_yitrim_mutation  = p.get_d("p_yitrim_mutation");
   
   pen_progsize  = p.get_d("pen_progsize");
   pen_wrongrez  = p.get_d("pen_wrongrez");
   pen_absentrez = p.get_d("pen_absentrez");
   
   valuator_type = p.get_s("valuator_type");
   
   if (p.get_s("nchildren") == "auto")
     nchildren = psize * 2;
   else
     nchildren  = p.get_i("nchildren");
            
   read_ansamble(p.get_s("ansamble"), ansamble);
   find_ans_alphabet();
}
//                                                                          
void create_population()
{   
   cl_brute_generator bg;
   bg.init(init_l, alphabet);
   while(population.size() < psize) 
     {
	cl_term* term = new cl_term(bg.create_random(), &counter);
	vector<string> yi(ansamble.size()); 
	for (size_t i = 0 ; i < yi.size() ; i++)
	  {
	     yi[i] = cl_str_make_random(ans_alphabet, init_yi_l);
	  }
	if (uniqchecker.try_to_add(term->conv2str(), yi))
	  {
	     population.push_back(make_cl_rmdlga_member(term, yi));
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
   
   int ran = random() % (ansamble.size() + 1);
   
   
   
   cl_term* t1 = new cl_term(*(population[id1]->term));
   cl_term* t2 = new cl_term(*(population[id2]->term));
   vector<string> yi1 = population[id1]->yi;
   vector<string> yi2 = population[id2]->yi;
   
   if (ran == (int)ansamble.size()) //make sex between terms   
     {	
	if (crossover_type == 0)
	  t1->crossover_a(t2);     
	else if (crossover_type == 1)
	  t1->crossover_s1(t2);
	else
	  {
	     cerr<<"Error | make_sex | unknown crossover type"<<endl;
	     exit(EXIT_FAILURE);
	  }
        
	if (p_echange_mutation > 0)
	  {
	     t1->echange_mutation(p_echange_mutation, alphabet);
	     t2->echange_mutation(p_echange_mutation, alphabet);
	  }
	
	if (p_trim_mutation > 0)
	  {
	     t1->trim_mutation(p_trim_mutation);
	     t2->trim_mutation(p_trim_mutation);
	  }
     } //end sex between terms
   else //make sex between yi[ran]
     {
      
	bool is_ok = false;
	while (!is_ok)
	  {
	     is_ok = true;
	     cl_str_crossover(yi1[ran], yi2[ran]);
	     cl_str_echange_mutation(yi1[ran], p_echange_mutation, ans_alphabet);
	     cl_str_echange_mutation(yi2[ran], p_echange_mutation, ans_alphabet);
	     
	     cl_str_trim_mutation(yi1[ran], p_yitrim_mutation);
	     cl_str_trim_mutation(yi2[ran], p_yitrim_mutation);
	     if (yi1[ran].size() < ans_alphabet.size() || yi2[ran].size() < ans_alphabet.size())
	       {
		  is_ok = false;
		  yi1 = population[id1]->yi;
		  yi2 = population[id2]->yi;
	       }
	  }
     }
   
   if (uniqchecker.try_to_add(t1->conv2str(), yi1))
     population.push_back(make_cl_rmdlga_member(t1, yi1));
   else
     delete t1;
   
   if (uniqchecker.try_to_add(t2->conv2str(), yi2))
     population.push_back(make_cl_rmdlga_member(t2, yi2));
   else
     delete t2;
}
//                                                                          
void extinction()
{   
   sort(population.begin(), population.end(), fun_for_sort_members);
   
   for (size_t i = psize ; i < population.size() ; i++)
     {
	uniqchecker.delete_term(population[i]->term_str, population[i]->yi);
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
      
   for (size_t i = 0 ; i < ansamble.size() ; i++)     
     cout<<population.at(0)->term_str<<" "<<population.at(0)->yi[i]<<" ----> "<<population.at(0)->rez[i]<<endl; 
   cout<<"part_L1="<<population.at(0)->part_L1<<
     " part_L2="<<population.at(0)->part_L2<<
     " part_R="<<population.at(0)->part_R<<endl;
}
//                                                                           
cl_rmdlga_member* make_cl_rmdlga_member(cl_term* cl, const vector<string>& yi)
{
   cl_rmdlga_member* member = new cl_rmdlga_member;
   
   member->term = cl;
   member->term_str = cl->conv2str();
   member->yi       = yi;
   member->rez.resize(ansamble.size());
   
   bool is_hit_limits = false;
   for (size_t i = 0 ; i < ansamble.size() && !is_hit_limits ; i++)
     {
	cl_term cl_calc(*cl);
	cl_calc.add_postfix(member->yi[i]);
	
	cl_resultator_length resultator(ansamble[i].size(), ignore);

	int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	
      
	if (rez == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (rez < 0) //we hit some of limits 
	  is_hit_limits = true;
	
	member->rez[i] = resultator.get_rez();
     }
   if (!is_hit_limits) //ok
     {
	member->calc_penalty(valuator_type, ansamble, pen_progsize, pen_wrongrez, pen_absentrez);
     }
   else //we heat some limits (memory or number of steps)
     {
	member->penalty = 1e+100; //some big number if we hit limits (bad style... I know)
     }
   member->generation = iteration;
   return member;
}
//                                                                          
bool fun_for_sort_members(cl_rmdlga_member* m1, cl_rmdlga_member*m2)
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
void find_ans_alphabet()
{
   set<char> al;
   for (size_t i = 0 ; i < ansamble.size() ; i++ )
     for (size_t j = 0 ; j < ansamble[i].size() ; j++)
       al.insert(ansamble[i][j]);
   ans_alphabet.clear();
   for (set<char>::iterator it = al.begin(); it != al.end() ; it++)
     ans_alphabet.push_back(*it);
}