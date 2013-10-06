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
#include "cl_rmdlga2_tools.h"
#include "cl_rmdlga_tools.h"
#include "cl_strga.h"
//#include "cl_ga_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansamble=\n                 Given ansamble",
   "alphabet=SKI\n              Alphabet",
   "alphabet_yi=01\n            Alphabet yi",
   "ignore=\n                   Symbols to ignore in result",   
   "init_l=10\n                 Initial length of each of member",
   "init_l_yi=5\n               Initial length of yi",
   "psize=500\n                 The size of the population",
   "nchildren=auto\n            Number of children (if auto nchildre = psize * 2)",
   "max_mem=10000\n             Maximal memory consumation during a single computation (nterms)",
   "max_steps=5000\n            Maximal reduction steps during a single computation",
   "p_exchange_mutation=0.05\n  probability of exchange mutation",
   "p_trim_mutation=0.05\n      probability of trim mutation",
   "p_yitrim_mutation=0.1\n     probability of yi trim mutation",
   "pen_progsize=1\n            Penalty for size of the program (per symbol)",
   "pen_wrongrez=10\n           Penalty for each wrong symbol in the result",
   "pen_absentrez=auto\n        Penalty for each empty symbol in the result (auto=pen_wrongrez)",   
   "valuator_type=\n            Valuator type (s or a)",
   "crossover_type=1\n          0-alex, 1-sergey1",
   "is_precise_cmp=0\n          Make precise comparison of xi and Syi (not only prefix)",   
   NULL
};

vector<string> ansamble;
string alphabet, alphabet_yi, ignore;
size_t init_l, init_l_yi, psize, nchildren;
size_t max_mem, max_steps;
int counter = 0;   //global counter (we use it in cl_term for assess memory usage)
double p_exchange_mutation, p_trim_mutation, p_yitrim_mutation;
double pen_progsize, pen_wrongrez, pen_absentrez;
string ans_alphabet; //alphabet of ansambler
string valuator_type;
bool is_precise_cmp; //if true we make precise comparison between xi and Syi
                     //(if false we take only prefix of Syi)

int crossover_type;

vector<cl_rmdlga2_member*> population;
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
cl_rmdlga2_member* make_cl_rmdlga2_member(cl_term* cl,
					 const vector<cl_term*>& yi, 
					 const vector<string>& yi_str);
bool fun_for_sort_members(cl_rmdlga2_member*m1, cl_rmdlga2_member*m2);
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
   alphabet_yi= p.get_s("alphabet_yi");
   ignore     = p.get_s("ignore");
   init_l     = p.get_i("init_l");
   init_l_yi  = p.get_i("init_l_yi");
   psize      = p.get_i("psize");
   max_mem    = p.get_i("max_mem");
   max_steps  = p.get_i("max_steps");
   crossover_type     = p.get_i("crossover_type");
   p_exchange_mutation = p.get_d("p_exchange_mutation");
   p_trim_mutation    = p.get_d("p_trim_mutation");
   p_yitrim_mutation  = p.get_d("p_yitrim_mutation");
   
   pen_progsize  = p.get_d("pen_progsize");
   pen_wrongrez  = p.get_d("pen_wrongrez");
   
   if (p.get_s("pen_absentrez") == "auto")
     pen_absentrez = pen_wrongrez;
   else
     pen_absentrez = p.get_d("pen_absentrez");

//   cout<<pen_progsize<<" "<<pen_wrongrez<<" "<<pen_absentrez<<endl;
   
   is_precise_cmp = p.get_b("is_precise_cmp");
   
   
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
   
   cl_brute_generator bg_yi;
   bg_yi.init(init_l_yi, alphabet_yi);
   while(population.size() < psize) 
     {
	cl_term* term = new cl_term(bg.create_random(), &counter);
	vector<cl_term*> yi(ansamble.size()); 
	vector<string> yi_str(ansamble.size());
	for (size_t i = 0 ; i < yi.size() ; i++)
	  {
	     yi[i]     = new cl_term(bg_yi.create_random(), &counter);
	     yi_str[i] = yi[i]->conv2str();
	  }
	if (uniqchecker.try_to_add(term->conv2str(), yi_str))
	  {
	     population.push_back(make_cl_rmdlga2_member(term, yi, yi_str));
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
   vector<cl_term*> yi1(ansamble.size());
   vector<cl_term*> yi2(ansamble.size());
   for (size_t i = 0 ; i < ansamble.size() ; i++)
     {
	yi1[i] = new cl_term(*(population[id1]->yi[i]));
	yi2[i] = new cl_term(*(population[id2]->yi[i]));
     }
   
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
     } //end sex between terms
   else //make sex between yi[ran]
     {	
	if (crossover_type == 0)
	  yi1[ran]->crossover_a(yi2[ran]);     
	else if (crossover_type == 1)
	  yi1[ran]->crossover_s1(yi2[ran]);
	else
	  {
	     cerr<<"Error | make_sex | unknown crossover type"<<endl;
	     exit(EXIT_FAILURE);
	  }
        
	yi1[ran]->exchange_mutation(p_exchange_mutation, alphabet_yi);
	yi2[ran]->exchange_mutation(p_exchange_mutation, alphabet_yi);
	
	yi1[ran]->trim_mutation(p_trim_mutation);
	yi2[ran]->trim_mutation(p_trim_mutation);
     }
   
   vector<string> yi1_str(yi1.size());
   vector<string> yi2_str(yi2.size());
   
   for (size_t i = 0 ; i < yi1.size() ; i++)
     {
	yi1_str[i] = yi1[i]->conv2str();
	yi2_str[i] = yi2[i]->conv2str();
     }
   
   if (uniqchecker.try_to_add(t1->conv2str(), yi1_str))
     population.push_back(make_cl_rmdlga2_member(t1, yi1, yi1_str));
   else
     {
	delete t1;
	for (size_t i = 0 ; i < yi1.size() ; i++)
	  delete yi1[i];
     }
   if (uniqchecker.try_to_add(t2->conv2str(), yi2_str))
     population.push_back(make_cl_rmdlga2_member(t2, yi2, yi2_str));
   else
     {
	delete t2;
	for (size_t i = 0 ; i < yi2.size() ; i++)
	  delete yi2[i];
     }
}
//                                                                          
void extinction()
{   
   sort(population.begin(), population.end(), fun_for_sort_members);
   
   for (size_t i = psize ; i < population.size() ; i++)
     {
	uniqchecker.delete_term(population[i]->term_str, population[i]->yi_str);
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
     cout<<population.at(0)->get_termyi(i)<<" ----> "<<population.at(0)->rez[i]<<endl; 
   cout<<"part_L1="<<population.at(0)->part_L1<<
     " part_L2="<<population.at(0)->part_L2<<
     " part_R="<<population.at(0)->part_R<<endl;
}
//                                                                           
cl_rmdlga2_member* make_cl_rmdlga2_member(cl_term* cl, 
					  const vector<cl_term*>& yi,
					  const vector<string>& yi_str)
{
   cl_rmdlga2_member* member = new cl_rmdlga2_member;
   
   member->term = cl;
   member->term_str = cl->conv2str();
   member->yi       = yi;
   member->yi_str   = yi_str;
   
   member->rez.resize(ansamble.size());
   
   bool is_hit_limits = false;
   for (size_t i = 0 ; i < ansamble.size() && !is_hit_limits ; i++)
     {
	cl_term cl_calc(member->get_termyi(i), &counter);
	
	size_t req_length = ansamble[i].size();
	if (is_precise_cmp) // in this case we should allow longer results
	  req_length = (size_t)(ansamble[i].size() * 1.5 + 2);
	cl_resultator_length resultator(req_length, ignore);
	
	int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	
      
	if (rez == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (rez < 0) //we hit some of limits 
	  is_hit_limits = true;
	
	member->rez[i] = resultator.get_rez();
	
	if (is_precise_cmp && member->rez[i].size() >= req_length) //resultat is too long
	  is_hit_limits = true;	
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
bool fun_for_sort_members(cl_rmdlga2_member* m1, cl_rmdlga2_member*m2)
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
