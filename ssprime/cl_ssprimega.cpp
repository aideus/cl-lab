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
   "max_SS_n=\n                 Maximal quantity of similar S or S'",
   "ext_type=e\n                Extinction type: e - elitist, p - proportional",
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
size_t max_SS_n;
string ext_type; // extinction type

int crossover_type;

const double INF_PENALTY = 1e+100; //infinity penalty

vector<cl_ssprimega_member*> population;
cl_ssprimega_uniqchecker     uniqchecker;
multiset<string>    Sp_nchecker, S_nchecker;

int iteration = 0; //iteration counter

void init(int argc, char*argv[]);
void create_population();
void eva_step();
void make_sex();
void make_sex_between_terms(cl_term* t1, cl_term* t2);
void extinction();
void extinction_proportional();
void delete_member(cl_ssprimega_member*);
void print_best_result();

//make new cl_ga_member from cl_term
//cl_ga_member will delete term by itself after
cl_ssprimega_member* make_cl_ssprimega_member(cl_term* Sp, cl_term* S);
void try_to_add_or_delete(cl_term* Sp, cl_term* S);
bool fun_for_sort_members(cl_ssprimega_member*m1, cl_ssprimega_member*m2);
void read_ansamble(string file, vector<string>& req);
void find_ans_alphabet();
void multiset_delete_one(multiset<string>& S, string str );
int max_count(const multiset<string>& S);

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
   
   max_SS_n      = p.get_i("max_SS_n");
   
   ext_type      = p.get_s("ext_type");
   
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
	try_to_add_or_delete(Sp, S);
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
   if (ext_type == "e") //elitist extinction
     {
	extinction();
     }
   else if (ext_type == "p") //proportional extinction
     {
	extinction_proportional();
     }
   else
     {
	cerr<<"Error | eva_step | unknown extinction type"<<endl;
	exit(EXIT_FAILURE);
     }
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
       
   try_to_add_or_delete(Sp1, S1); 
   try_to_add_or_delete(Sp2, S2); 
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
	delete_member(population[i]);
     }
   population.resize(psize);
}
//                                                                                      
void extinction_proportional()
{   
   sort(population.begin(), population.end(), fun_for_sort_members);
   
   //make pre extinction (remove all with INF_PENALTY)
   size_t n_del = 0;
   for (size_t i = population.size() - 1 ; 
	i >= psize && population[i]->penalty == INF_PENALTY ; //remove not more than psize
	i--) 
     {
	delete_member(population[i]);
	n_del++;
     }
   population.resize(population.size() - n_del);
   
   
   if (population.size() == psize) //ok
     return;

   double min_prob = 0.02;
   double max_prob = 1; 
   double min_pen = population.front()->penalty; //probability of extinction min_prob
   double max_pen = population.back()->penalty;   //probability of extinction max_prob
   
   if (min_pen == max_pen)  //in this case we make simple extinction
     {
	extinction();
	return;
     }
//   cout<<"min/max="<<min_pen<<" "<<max_pen<<endl;
   
   double black_label = max_pen * 2;
   
   size_t ext = 0;
   
   while (ext < (population.size() - psize))  //we should kill (population.size() - psize)
     {
	size_t idx = random() % population.size();
	if (population[idx]->penalty != black_label) //if it haven't been already killed
	  {
	     double ep = min_prob + (max_prob - min_prob) *
	       (population[idx]->penalty - min_pen) / (max_pen - min_pen);
//	     cout<<ep<<" "<<population[idx]->penalty<<" "<<min_pen<<"/"<<max_pen<<endl;
	     double r = random() / double(RAND_MAX);
	     if (ep > r) 
	       {
//		  if (population[idx]->penalty == min_pen)
//		    cout<<"Best extinction "<<ep<<" "<<r<<endl;
//		  cout<<"extinction"<<endl;
		  population[idx]->penalty = black_label;
		  ext++;
	       }
	  }
     }

   extinction();
   for (size_t i = 0 ; i < population.size() ; i++)
     if (population[i]->penalty == black_label)
       {
	  cerr<<"Error | extinction_proportional | internal"<<endl;
	  exit(EXIT_FAILURE);
       }
}
//                                                                           
void delete_member(cl_ssprimega_member* m)
{
   uniqchecker.delete_term(m->Sp_str, m->S_str);
   multiset_delete_one(Sp_nchecker, m->Sp_str);
   multiset_delete_one(S_nchecker,  m->S_str);
   delete m;
}
//                                                                           
void print_best_result()
{
   int nc = 0;
   for (size_t i = 0 ; i < population.size() ; i++)
     if (population[i]->generation == iteration)
       nc++;
   cout<<"survived children = "<<nc<<endl;

   cout<<"count: "<<Sp_nchecker.size()<<" "<<S_nchecker.size()<<" --> "<<max_count(Sp_nchecker)<<" "<<max_count(S_nchecker)<<endl;
   cout<<"Best/Worse="<<population.at(0)->penalty<<"/"<<population.back()->penalty<<endl;
   cout<<"Sp / S="<<population.at(0)->Sp_str<<" / "<<population.at(0)->S_str<<endl;
   for (size_t i = 0 ; i < ansamble.size() ; i++)     
     cout<<ansamble[i]<<" ---> "<<population.at(0)->rez_Sp[i]<<" ----> "<<population.at(0)->rez_S[i]<<endl; 
   cout<<"part_L1="<<population.at(0)->part_L1<<
     " part_L2="<<population.at(0)->part_L2<<
     " part_R="<<population.at(0)->part_R<<endl;
}
//                                                                           
void try_to_add_or_delete(cl_term* Sp, cl_term* S)
{
   string Sp_str = Sp->conv2str();
   string S_str  = S->conv2str();
   Sp_nchecker.insert(Sp_str);
   S_nchecker.insert(S_str);
   if (Sp_nchecker.count(Sp_str) >= max_SS_n || S_nchecker.count(S_str) >= max_SS_n)
     {
	multiset_delete_one(Sp_nchecker, Sp_str);
	multiset_delete_one(S_nchecker,  S_str);
	delete Sp;
	delete S;
	return;
     }
   if (uniqchecker.try_to_add(Sp_str, S_str))
     {
	population.push_back(make_cl_ssprimega_member(Sp, S));
     }
   else
     {
	multiset_delete_one(Sp_nchecker, Sp_str);
	multiset_delete_one(S_nchecker,  S_str);
	delete Sp;
	delete S;
	return;
     }
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
	member->penalty = INF_PENALTY; //some big number if we hit limits (bad style... I know)
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
//                                                                                        
void multiset_delete_one(multiset<string>& S, string str )
{
   S.erase(S.find(str));
}
//                                                                                        
int max_count(const multiset<string>& S)
{
   size_t m = 0;
   multiset<string>::const_iterator it = S.begin();
   while (it != S.end())
     {
	size_t urange_size = 0;
	string s = *it;
	while (it != S.end() && s == *it)
	  {
	     urange_size++;
	     it++;
	  }
	m = max(urange_size, m);
     }
   return m;
}
