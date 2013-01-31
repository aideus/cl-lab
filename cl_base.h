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

class cl_term 
{
 public:      
   cl_term(char term_, int* counter); //create empty 
   cl_term(const cl_term&);           //copy all tree
   //recursively create tree from cl expression
   //if counter == NULL --> we use global counter 
   //and it is NOT THREAD-SAFE
   cl_term(string s, int* counter);   
   ~cl_term();
   
   void rec_print(ostream &out);   
   string conv2str();
   
   
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
 private:
   void install_counter(int* counter);
      
   void apply_S(); //Sabc ab(bc)
   void apply_K(); //Kab  a
   void apply_I(); //Ia   a
   void apply_B(); //Babc a(bc)
   void apply_C(); //Cabc acb
   void apply_W(); //Wxy  xyy
   void apply_Y(); //Yx   x(Yx)
  
   void replace_this_with_a(cl_term* a);
   //recursively create term
   void rec_create_term(string s, size_t& pos); 
   
//   void non_rec_create(const cl_term&);
 private:
   char term;
   list<cl_term*> chain; // to which this term is applied
   
   //number of subterms. Internal TEMPORAL variable which we use during crosover
   int nsubterms; 
   
   int * counter; //internal variable for reduce_all (memory control)
};
#endif
