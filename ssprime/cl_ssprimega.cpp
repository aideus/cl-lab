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
#include "cl_ssprimega_tools.h"
#include "cl_brute.h"
#include <algorithm>
#include <fstream>

const char* defv[] = {
   "ansamble=\n                 Given ansamble",
   "alphabet=SKI\n              Alphabet",
   "ignore=\n                   Symbols to ignore in result",
   "max_yi_l=\n                 Maximal length of rez_Sp[i]",
   "init_l=10\n                 Initial length of each of member",
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
size_t init_l, max_yi_l, psize, nchildren;
size_t max_mem, max_steps;
int counter;   //global counter (we use it in cl_term for assess memory usage)
double p_echange_mutation, p_trim_mutation, p_yitrim_mutation;
double pen_progsize, pen_wrongrez, pen_absentrez;
string ans_alphabet; //alphabet of ansambler
string valuator_type;

int crossover_type;

vector<cl_ssprimega_member*> population;
cl_ssprimega_uniqchecker     uniqchecker;

int iteration = 0; //iteration counter

void init(int argc, char*argv[]);
void create_population();
void eva_step();
void make_sex();
void make_sex_between_terms(cl_term* t1, cl_term* t2);
void extinction();
void print_best_result();

//make new cl_ga_member from cl_term
//cl_ga_member will delete term by itself after
cl_ssprimega_member* make_cl_ssprimega_member(cl_term* Sp, cl_term* S);
bool fun_for_sort_members(cl_ssprimega_member*m1, cl_ssprimega_member*m2);
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
   max_yi_l   = p.get_i("max_yi_l");
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
	cl_term* Sp = new cl_term(bg.create_random(), &counter);
	cl_term* S = new cl_term(bg.create_random(), &counter);
	if (uniqchecker.try_to_add(Sp->conv2str(), S->conv2str()))
	  {
	     population.push_back(make_cl_ssprimega_member(Sp, S));
	  }
	else
	  {
	     delete Sp;
	     delete S;
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
   
   
      
   cl_term* Sp1 = new cl_term(*(population[id1]->Sp));
   cl_term* Sp2 = new cl_term(*(population[id2]->Sp));
   cl_term* S1  = new cl_term(*(population[id1]->S));
   cl_term* S2  = new cl_term(*(population[id2]->S));
   
   if (random() % 2) //make sex between terms   
	make_sex_between_terms(Sp1, Sp2);
   else
     make_sex_between_terms(S1, S2);
        
   if (uniqchecker.try_to_add(Sp1->conv2str(), S1->conv2str()))
     population.push_back(make_cl_ssprimega_member(Sp1, S1));
   else
     {
	delete Sp1;
	delete S1;
     }
   if (uniqchecker.try_to_add(Sp2->conv2str(), S2->conv2str()))
     population.push_back(make_cl_ssprimega_member(Sp2, S2));
   else
     {
	delete Sp2;
	delete S2;
     }
   
}
//                                                                          
void make_sex_between_terms(cl_term* t1, cl_term* t2)
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
}
//                                                                          
void extinction()
{   
   sort(population.begin(), population.end(), fun_for_sort_members);
   
   for (size_t i = psize ; i < population.size() ; i++)
     {
	uniqchecker.delete_term(population[i]->Sp_str, population[i]->S_str);
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
   cout<<"Sp / S="<<population.at(0)->Sp_str<<" / "<<population.at(0)->S_str<<endl;
   for (size_t i = 0 ; i < ansamble.size() ; i++)     
     cout<<ansamble[i]<<" ---> "<<population.at(0)->rez_Sp[i]<<" ----> "<<population.at(0)->rez_S[i]<<endl; 
   cout<<"part_L1="<<population.at(0)->part_L1<<
     " part_L2="<<population.at(0)->part_L2<<
     " part_R="<<population.at(0)->part_R<<endl;
}
//                                                                           
cl_ssprimega_member* make_cl_ssprimega_member(cl_term* Sp, cl_term* S)
{
   cl_ssprimega_member* member = new cl_ssprimega_member;
   
   member->Sp     = Sp;
   member->Sp_str = Sp->conv2str();

   member->S     = S;
   member->S_str = S->conv2str();

   member->rez_Sp.resize(ansamble.size());
   member->rez_S.resize(ansamble.size());

   //calculate rez_Sp[i] = Sp . ans[i]  (maximal length max_yi_l)
   bool is_hit_limits = false;
   for (size_t i = 0 ; i < ansamble.size() && !is_hit_limits ; i++)
     {
	cl_term cl_calc(*Sp);
	cl_calc.add_postfix(ansamble[i]);
	
	cl_resultator_length resultator(max_yi_l, ignore);

	int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	      
	if (rez == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (rez < 0) //we hit some of limits 
	  is_hit_limits = true;	
	member->rez_Sp[i] = resultator.get_rez();
     }

   //calculate rez_S[i] = S . rez_Sp[i]  (maximal length ansamble[i].size())
   for (size_t i = 0 ; i < ansamble.size() && !is_hit_limits ; i++)
     {
	cl_term cl_calc(*S);
	cl_calc.add_postfix(member->rez_Sp[i]);
	
	cl_resultator_length resultator(ansamble[i].size(), ignore);

	int rez = cl_calc.reduce_all(max_steps, counter + max_mem, &resultator);
	      
	if (rez == -1) //it's imposible
	  {
	     cerr<<"error | make_cl_ga_member | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	
	if (rez < 0) //we hit some of limits 
	  is_hit_limits = true;	
	member->rez_S[i] = resultator.get_rez();
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
bool fun_for_sort_members(cl_ssprimega_member* m1, cl_ssprimega_member*m2)
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
