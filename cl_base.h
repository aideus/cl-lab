//cl_base.h

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#ifndef __SEGER__CL_BASE_H__
#define __SEGER__CL_BASE_H__
#include <list>
#include <string>
#include <cstdlib>
#include <iostream>
#include "cl_resultator.h"

using namespace std;

class cl_termstorage;

class cl_term 
{
 public:      
   cl_term(char term_, int* counter); //create empty 
   
   //copy all tree and make delaying copy (is_delayed must be true)
   cl_term(cl_term&, bool is_delayed);  

   //make simple copy from constant term
   cl_term(const cl_term&);  

   cl_term(string s, int* counter);   
   ~cl_term();
   
   //convert to string (recursively, so result is attached to str) 
   //max_len --- maximal lenght of the string (0 - no limits, but could be dangerous)
   //return false if hit max_len
   bool rec_conv2str(string &str, size_t max_len = 0) const;      
   string conv2str(size_t max_len = 0) const;
   
   
   //try to make one step of reduction
   bool reduce();   
   
   //make full reduction
   //0 --- we finish but resultat haven't obtained or we wanted full reduction with 
   //      resultator = NULL
   //1 --- resultator told that we finish, so we stoped
   //-1 --- resultator told that we fail, so we stoped
   //-2 --- we hit step limit
   //-3 --- we hit memory limit
   int reduce_all (int max_steps, int max_mem, cl_resultator* resultator = NULL); 
   
   //can return only < 0 if hits limit or 0
   int one_level_reduce(int& steps_left, int max_mem);
   int reduce_all_(int& steps_left, int max_mem, cl_resultator* resultator = NULL); 
   
   
   int count_nonterm_nodes();
   int get_nonterm_node(cl_term** term, int n);
   void crossover_a(cl_term* a);
   
   //count number of sub_terms (except this term)
   int count_subterms();
   
   //get sub term
   //0 -- return this term
   //num can be from 0 to nsubterms including
   //(should be call strictly after count_subterms)
   cl_term* get_subterm(int num);
   
   //make term exchange crossover
   void crossover_s1(cl_term*);
   
   void echange_mutation(double p, string alphabet);
   
   //make only one trim at once!!!
   void trim_mutation(double p);
   
   //add simple postfix
   void add_postfix(string postfix);
   
   //is this term is difficult enough to make delayed copy?

   bool is_nontrivial(); 
 private:
   void install_counter_null_ts(int* counter);
   void init_from_term(const cl_term&);   //only copy term and chain!
      
   void apply_S(); //Sabc   ac(bc)
   void apply_K(); //Kab    a
   void apply_I(); //Ia     a
   void apply_B(); //Babc   a(bc)
   void apply_C(); //Cabc   acb
   void apply_W(); //Wab    abb
   void apply_Y(); //Yx     x(Yx)
   void apply_M(); //Mx     xx
   void apply_T(); //Txy    yx
   void apply_J(); //Jxyzv  xy(xvz)
   void apply_b(); //B'xyz  y(xz)
   void apply_V(); //Vxyz   zxy
   
   
   void replace_this_with_a(cl_term* a);
   //recursively create term
   void rec_create_term(string s, size_t& pos); 
   
   friend class cl_termstorage;
 private:
   char term;
   list<cl_term*> chain; // to which this term is applied
   
   //number of subterms. Internal TEMPORAL variable which we use during crosover
   int nsubterms; 
   
   int * counter; //internal variable for reduce_all (memory control)
   
   cl_termstorage* ts;
};
#endif
