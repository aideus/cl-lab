//cl_base.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/

#include "cl_base.h"
#include "cl_termstorage.h"
#include <algorithm>
#include <locale>
#include <sstream>
#include <stack>

cl_term::cl_term(char term_, int* counter_):term(term_)
{
   install_counter_null_ts(counter_);
}
//                                                                           
/*cl_term::cl_term(const cl_term& cl)
{
   term    = cl.term;
   install_counter(cl.counter);
   typedef pair<cl_term*, const cl_term*> pair_type;
   stack<pair_type> chain_copy_stack; //to,from
   chain_copy_stack.push(pair_type(this, &cl));
   while(!chain_copy_stack.empty())
     {
	cl_term* to         = chain_copy_stack.top().first;
	const cl_term* from = chain_copy_stack.top().second;
	chain_copy_stack.pop();
	for (list<cl_term*>::const_iterator it = from->chain.begin(); it != from->chain.end() ; it++)
	  {
	     cl_term* t = new cl_term((*it)->term, (*it)->counter);
	     chain_copy_stack.push(pair_type(t, *it));
	     to->chain.push_back(t);
	  }
     }
}*/
//                                                                         
cl_term::cl_term(cl_term& cl, bool is_delayed)
{
   install_counter_null_ts(cl.counter);
   if (is_delayed && cl.is_nontrivial())
     {
	//if it was already copied and chain is empty ---> just add one origin
	if (cl.term == CL_TS_TERM && cl.chain.empty()) 
	  {	     
	     cl.ts->add_new_origin(this); //add this as origin to ts 
	  }
	else //normal delayed copy
	  {
	     ts    = new cl_termstorage(cl);	
	     ts->add_new_origin(this);
	  }
     }
   else
     {
	init_from_term(cl);
     }
}
//                                                                        
cl_term::cl_term(const cl_term& cl)
{
   install_counter_null_ts(cl.counter);
   init_from_term(cl);
}
//                                                                        
void cl_term::install_counter_null_ts(int* counter_)
{
//   if (counter_ == NULL)
//     counter = &cl_term_global_counter;
//   else
   counter = counter_;
   (*counter)++;
   ts      = NULL;
}
//                                                                      
void cl_term::init_from_term(const cl_term& cl)
{
   if (cl.term == CL_TS_TERM)
     {
	cerr<<"error | cl_term::cl_term | TS-term in non delayed copy"<<endl;
	cerr<<cl.conv2str()<<endl;
	exit(EXIT_FAILURE);
     }
   term    = cl.term;
   for (list<cl_term*>::const_iterator it = cl.chain.begin(); it != cl.chain.end() ; it++)
     {
	chain.push_back((new cl_term(*(*it))));
     }
}
//                                                                        
cl_term::cl_term(string str, int* counter_)
{
   term = 0;
   install_counter_null_ts(counter_);   
   str = "(" + str + ")";
   //c++ is shit
   str.erase(std::remove_if(str.begin(), str.end(), (int(*)(int))isspace), str.end());
   size_t pos = 0;
   rec_create_term(str, pos);
}
//                                                                        
cl_term::~cl_term()
{
//   cout<<"delete "<<term<<" "<<chain.size()<<endl;
   (*counter)--;
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
     delete *it;
   if (term == CL_TS_TERM)
     {
	if (ts == NULL || ts->norigins <= 0) // something wrong
	  {
	     cerr<<"Error | cl_term::~cl_term | ts is NULL but test is CL_TS_TERM"<<endl;
	     exit(EXIT_FAILURE);
	  }
	ts->norigins--;
	if (ts->norigins == 0) //nobody refer to it anymore
	  delete ts;
	ts = NULL;
     }
   if (ts != NULL)
     {
	cerr<<"Error | cl_term::~cl_term | ts != NULL"<<endl;
	exit(EXIT_FAILURE);
     }
}
//                                                                        
bool cl_term::rec_conv2str(string& str, size_t max_len) const
{
   if (max_len > 0 && str.size() > max_len)
     return false;
   if (term == CL_TS_TERM)
     {
	if (ts == NULL)
	  {
	     cerr<<"Error | cl_term::rec_print | ts is NULL"<<endl;
	     exit(EXIT_FAILURE);
	  }
	str += '(';
	if (!ts->t->rec_conv2str(str, max_len))
	  return false;
	str += ')';	
     }
   else
     str += term;
   for (list<cl_term*>::const_iterator it = chain.begin(); it != chain.end() ; it++)
     {
	if ((*it)->chain.size() > 0 || (*it)->term == CL_TS_TERM)
	  str +='(';
	if ( ! (*it)->rec_conv2str(str, max_len) )
	  return false;
	if ((*it)->chain.size() > 0 || (*it)->term == CL_TS_TERM)
	  str += ')';
     }
   return true;
}
//                                                                        
/*void cl_term::non_rec_print(ostream& out)
{
   out<<term;
   typedef pair_type pair<list<cl_term*>::const_iterator, cl_term*>; 
   stack<pair_type> rps; //return point stack
   rps.push(pair_type(this,chain.begin()));
   while(!rps.empty())
     {
	list<cl_term*>::const_iterator it = rps.top().first;
	cl_term*                       t  = rps.top().second;
	bool is_stop = false;
	while(it != t.end() && !is_stop)
	  {
	     if ((*it)->chain.size() == 0)
	       out<<(*it)->term;
	     else
	       {
		  is_stop = true;
		  out<<'(';
		  rps.push_back(*it, it->chain.begin()))
		  (*it)->rec_print(out);
		  out<<')';
	       }
	     it++;
	  }
	rps.pop();
     }
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
     {
	if ((*it)->chain.size() == 0)
	  out<<(*it)->term;
	else
	  {
	     out<<'(';
	     (*it)->rec_print(out);
	     out<<')';
	  }
     }
}*/
//                                                                        
string cl_term::conv2str(size_t max_len) const
{
   string str;
   rec_conv2str(str, max_len);
   return str;
}
//                                                                        
int cl_term::reduce_step(int& steps_left, int max_mem)
{
//   rec_print(cout);
//   cout<<endl;
//   cout<<endl;
   if (term == 'S' && chain.size() >= 3)
     {
	apply_S();
	return 1;       
     }
   else if (term == 'K' && chain.size() >= 2)
     {
	apply_K();
	return 1;
     }
   else if (term == 'I' && chain.size() >= 1)
     {
	apply_I();
	return 1;
     }
   else if (term == 'B' && chain.size() >= 3)
     {
	apply_B();
	return 1;
     }
   else if (term == 'C' && chain.size() >= 3)
     {
	apply_C();
	return 1;
     }
   else if (term == 'W' && chain.size() >= 2)
     {
	apply_W();
	return 1;
     }
   else if (term == 'Y' && chain.size() >= 1)
     {
	apply_Y();
	return 1;
     }
   else if (term == 'M' && chain.size() >= 1)
     {
	apply_M();
	return 1;
     }
   else if (term == 'T' && chain.size() >= 2)
     {
	apply_T();
	return 1;
     }
   else if (term == 'J' && chain.size() >= 4)
     {
	apply_J();
	return 1;
     }
   else if (term == 'b' && chain.size() >= 3)
     {
	apply_b();
	return 1;
     }
   else if (term == 'V' && chain.size() >= 3)
     {
	apply_V();
	return 1;
     }   
   else if (term == 'F' && chain.size() >= 4)
     {
	int rez = apply_F(steps_left, max_mem);
	if (rez < 0) //limit was hit
	  return rez;
	return 1;
     }   
   return 0;
}
//                                                                             
int cl_term::reduce_all(int max_steps, int max_mem, cl_resultator* resultator)
{
   int steps_left = max_steps;
   return reduce_all_(steps_left, max_mem, resultator);
}
//                                                                             
int cl_term::one_level_reduce(int& steps_left, int max_mem)
{
   while (1)
     {	
	//	cout<<"process: "<<term<<endl;
	if (max_mem <= *counter) 
	  return -3; //hit memory limit
	int reduce_step_rez = reduce_step(steps_left, max_mem);
	
	if (reduce_step_rez < 0) //some limits was hit
	  return reduce_step_rez;
	
	if (reduce_step_rez == 1) //successefull reduce_step
	  {
	     steps_left--;
	     if (steps_left <= 0)
	       return -2; //hit steps limit
	  }
	else if (term == CL_TS_TERM) //reduce_step_rez == 0 but could be CL_TS_TERM
	  {
	     if (!ts->is_reduced)
	       {
		  int rez = ts->t->one_level_reduce(steps_left, max_mem); 
		  if (rez < 0) //limits was hit
		    return rez; 
		  ts->is_reduced = true;
	       }
	     if (ts->t->term == CL_TS_TERM) //it's impossible if it was reduced!
	       {
		  cerr<<"error | cl_term::one_level_reduce | impossible"<<endl;
		  exit(EXIT_FAILURE);
	       }
	     if (ts->norigins == 1) //only one origin ... so we just replace
	       {
		  //we make it first
		  ts->norigins--;
		  
		  //like replace_this_with_a but without delete a
		  term = ts->t->term; 
		  chain.splice(chain.begin(), ts->t->chain);
		  
		  delete ts;
		  //it cannot be CL_TS_TERM so NULL
		  ts = NULL;
	       }
	     else //more then one origin --- we really copy it
	       {
		  term = ts->t->term;		  
		  list<cl_term*> chain_to;
		  
		  for (list<cl_term*>::iterator it = ts->t->chain.begin();
		      it != ts->t->chain.end();
		      it++)
		    {
		       chain_to.push_back((new cl_term(*(*it), true))); //create and allow deleyed copy
		    }
		  chain.splice(chain.begin(), chain_to);
		    
		  ts->norigins--;
		  
		  //and finally because it cannot be CL_TS_TERM
		  ts = NULL; 
	       }	     
	  }
	else  //reduce_step_rez == 0 and it is not CL_TS_TERM
	  return 0; //ok reduction of level is complited
     }
}
//                                                                             
int cl_term::reduce_all_(int& steps_left, int max_mem, cl_resultator* resultator)
{
   int rez = one_level_reduce(steps_left, max_mem);
   if (rez < 0) //limits was hit
     return rez;
   
   //so head can be added to resultator
   if (resultator != NULL)
     {
	int rez = resultator->put_next(term);
	if (rez != 0) 
	  return rez; //-1 --- we should stop --- resultat is negative
	              //1  --- we should stop --- we obtain required resultat
     }
   for (list<cl_term*>::const_iterator it = chain.begin(); it != chain.end() ; it++)
     {
	int rez = (*it)->reduce_all_(steps_left, max_mem, resultator);
	if (rez != 0)     
	  return rez;
     }
   return 0; //so we finish but result wasn't obtained (or we want full reduce with resultator = NULL)
}
//                                                                             
void cl_term::apply_S()
{
   if (term != 'S' && chain.size() < 3)
     {
	cerr<<"Error | cl_term::apply_S | bad term application"<<endl;
     }
   //S a b c  ac (bc)   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   chain.erase(chain.begin(), ++it); //remove a, b, c from the chain
   
   cl_term* c2 = new cl_term(*c, true); //copy c (allow delayed copy)
   
   b->chain.push_back(c2); //add second c to b
   
   
   chain.insert(chain.begin(), b); //add (bc2) to the front of the chain
   chain.insert(chain.begin(), c); //add c to the front of the chain
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                          
void cl_term::apply_K()
{
   if (term != 'K' && chain.size() < 2)
     {
	cerr<<"Error | cl_term::apply_K | bad term application"<<endl;
     }
   //K a b --> a
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it; //a
   cl_term *b = *(++it); //b
   delete b; //we don't need b
   chain.erase(chain.begin(), ++it); //remove a and b from the chain
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                           
void cl_term::apply_I()
{
   if (term != 'I' && chain.size() < 1)
     {
	cerr<<"Error | cl_term::apply_I | bad term application"<<endl;
     }
   //Ia --> a
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it; //a
   chain.erase(it); //remove a from the chain

   //replace current term with a
   replace_this_with_a(a);
}
//                                                                             
void cl_term::apply_B()
{
   if (term != 'B' && chain.size() < 3)
     {
	cerr<<"Error | cl_term::apply_B | bad term application"<<endl;
     }
   //B a b c  a (b c)
   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   chain.erase(chain.begin(), ++it); //remove a, b, c from the chain
      
   b->chain.push_back(c); //add c to b
      
   chain.insert(chain.begin(), b); //add (bc) to the front of the chain
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                             
void cl_term::apply_C()
{
   if (term != 'C' && chain.size() < 3)
     {
	cerr<<"Error | cl_term::apply_C | bad term application"<<endl;
     }
   //Cabc acb
   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   chain.erase(chain.begin(), ++it); //remove a, b, c from the chain
      
      
   chain.insert(chain.begin(), b); //add b to the front of the chain
   chain.insert(chain.begin(), c); //add c to the front of the chain
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                          
void cl_term::apply_W()
{
   if (term != 'W' && chain.size() < 2)
     {
	cerr<<"Error | cl_term::apply_W | bad term application"<<endl;
     }
   //W a b --> abb
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   chain.erase(chain.begin(), ++it); //remove a and b from the chain
   
   cl_term* b2 = new cl_term(*b, true); //copy b, allow delayed copy
   chain.insert(chain.begin(), b);  //insert b in front of the chain
   chain.insert(chain.begin(), b2); //insert b2 in front of the chain
   
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                           
void cl_term::apply_Y()
{
   if (term != 'Y' && chain.size() < 1)
     {
	cerr<<"Error | cl_term::apply_Y | bad term application"<<endl;
     }
   //Ya --> a(Ya)
   list<cl_term*>::iterator it = chain.begin(); //iterator to a
   cl_term *a = *it;     //a
   chain.erase(it);      //remove a from the chain
   cl_term* a2 = new cl_term(*a, true); //copy a, allow delayed copy
   
   cl_term* Y  = new cl_term('Y',counter);
   Y->chain.push_back(a2);   //Ya2
   a->chain.push_back(Y);    //we've created a(Ya2)
   
   //replace current term with a
   replace_this_with_a(a);   
}
//                                                                          
void cl_term::apply_M()
{
   if (term != 'M' && chain.size() < 1)
     {
	cerr<<"Error | cl_term::apply_M | bad term application"<<endl;
     }
   //M a --> aa
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;       //a
   
   cl_term* a2 = new cl_term(*a, true); //copy a, allow delayed copy
      
   //replace current term with a2
   replace_this_with_a(a2);
}
//                                                                          
void cl_term::apply_T()
{
   if (term != 'T' && chain.size() < 2)
     {
	cerr<<"Error | cl_term::apply_T | bad term application"<<endl;
     }
   //T a b --> b a
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   chain.erase(chain.begin(), ++it); //remove a and b from the chain
   
   chain.insert(chain.begin(), a);  //insert a in front of the chain
      
   //replace current term with b
   replace_this_with_a(b);
}
//                                                                             
void cl_term::apply_J()
{
   if (term != 'J' && chain.size() < 4)
     {
	cerr<<"Error | cl_term::apply_J | bad term application"<<endl;
     }
   //J abcd --> ab(adc)  
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   cl_term *d = *(++it); //d
   chain.erase(chain.begin(), ++it); //remove a, b, c, d from the chain
   
   cl_term* a2 = new cl_term(*a, true); //copy a, allow delayed copy
   
   a2->chain.push_back(d);    //add d to a2
   a2->chain.push_back(c);    //add c to a2
   
   
   chain.insert(chain.begin(), a2); //add a2dc to the front of the chain
   chain.insert(chain.begin(), b);   //add c to the front of the chain
   
   //replace current term with a
   replace_this_with_a(a);
}
//                                                                             
void cl_term::apply_b()
{
   if (term != 'b' && chain.size() < 3)
     {
	cerr<<"Error | cl_term::apply_b | bad term application"<<endl;
     }
   //B' abc  --> b(ac)
   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   chain.erase(chain.begin(), ++it); //remove a, b, c from the chain
      
   a->chain.push_back(c); //add c to a
      
   chain.insert(chain.begin(), a); //add (ac) to the front of the chain
   
   //replace current term with b
   replace_this_with_a(b);
}
//                                                                             
void cl_term::apply_V()
{
   if (term != 'V' && chain.size() < 3)
     {
	cerr<<"Error | cl_term::apply_V | bad term application"<<endl;
     }
   //Vabc cab
   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   chain.erase(chain.begin(), ++it); //remove a, b, c from the chain
      
      
   chain.insert(chain.begin(), b); //add b to the front of the chain
   chain.insert(chain.begin(), a); //add a to the front of the chain
   
   //replace current term with c
   replace_this_with_a(c);
}
//                                                                             
int cl_term::apply_F(int& steps_left, int max_mem)
{
   if (term != 'F' && chain.size() < 4)
     {
	cerr<<"Error | cl_term::apply_F | bad term application"<<endl;
     }
   //Fabcd  if (c == d) "a" else "b"
   
   list<cl_term*>::iterator it = chain.begin();
   cl_term *a = *it;     //a
   cl_term *b = *(++it); //b
   cl_term *c = *(++it); //c
   cl_term *d = *(++it); //d
   
   //now we ask to fully reduce c and d
   int rez = c->reduce_all_(steps_left, max_mem);
   if (rez < 0) //limits was hit
     return rez;
   
   rez = d->reduce_all_(steps_left, max_mem);
   if (rez < 0) //limits was hit
     return rez;

   chain.erase(chain.begin(), ++it); //remove a, b, c, d from the chain
   
   string str_c = c->conv2str();
   string str_d = d->conv2str();
   
   delete c;
   delete d;
   
   if (str_c == str_d)
     {   
	//replace current term with a
	replace_this_with_a(a);
	delete b;
     }
   else
     {
	//replace current term with b
	replace_this_with_a(b);
	delete a;	
     }
   return 0;
}
//                                                                           
void cl_term::replace_this_with_a(cl_term* a)
{
   term = a->term;
   chain.splice(chain.begin(), a->chain);
   if (term == CL_TS_TERM)
     {
	ts = a->ts;
	a->ts = NULL;
	a->term++; //just make it not CL_TS_TERM to delete without problem
     }
   delete a;
}
//                                                                           
void cl_term::rec_create_term(string s, size_t& pos)
{
   if (s[pos] != '(' || term != 0 || chain.size() != 0)
     {
	cout<<"Error | rec_create_term | internal error 1"<<endl;
	exit(EXIT_FAILURE);
     }
   pos++; //eat leading (
   
   if (s[pos] == '(')
     rec_create_term(s, pos);
   
   //current symbol is not '(' 
   while (s[pos] != ')' && s.size() != pos)
     {
	if (s[pos] == '(') //create new cl_term
	  {
	     cl_term* sub_term = new cl_term(0, counter);
	     sub_term->rec_create_term(s, pos);
	     if (sub_term->term == 0) //this is error
	       {
		  cout<<"Error | rec_create_term | internal error 2"<<endl;
		  exit(EXIT_FAILURE);
	       }
	     chain.push_back(sub_term);
	  }
	else
	  {
	     char c = s[pos++];
	     if (term == 0) //this is head
	       term = c;
	     else
	       {
		  chain.push_back(new cl_term(c, counter));
	       }
	  }
     }   
   if (s[pos] != ')')
     {
	cerr<<"Error |  rec_create_term | missing ')'"<<endl;
	exit(EXIT_FAILURE);
     }
   pos++; //eat tailing ')'
}
//                                                                           
int cl_term::count_nonterm_nodes()
{
  int count = chain.size();
  if (count == 0)
    return 0;
  for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
  {
    if ((*it)->chain.size() != 0)
    {
      count += (*it)->count_nonterm_nodes();
    }
  }
  return count;
}
//                                                                        
int cl_term::get_nonterm_node(cl_term** term, int n)
{
  int count = 0;
  if (n == 0) {
    *term = this;
    return 0;
  }
  *term = NULL;
  if (chain.size() == 0) {
    return 0;
  }
  for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++) {
    count++;
    if (n <= count) {
      *term = (*it);
      return 0;
    }
    if ((*it)->chain.size() != 0) {
      count += (*it)->get_nonterm_node(term, n-count);
      if (*term != NULL) {
        return 0;
      }
    }
  }
  return count;
}
//                                                                        
void cl_term::crossover_a(cl_term* a)
{
   if (a == this)
     {
	cerr<<"Error | cl_term::crossover | cannot make crossover with selfmem"<<endl;
	exit(EXIT_FAILURE);
     }
   cl_term * term1, * term2;
   int cnt1 = count_nonterm_nodes();
   int cnt2 = a->count_nonterm_nodes();
   //if cnt1 > 1 && cnt2 > 1
   
//   term1->rec_print(cout); cout<<endl;
//   term2->rec_print(cout); cout<<endl;
//   cout<<endl;
   get_nonterm_node(&term1,    random() %(cnt1 + 1));
   a->get_nonterm_node(&term2, random() %(cnt2 + 1) );
   
   term1->chain.swap(term2->chain);
}
//                                                                        
int cl_term::count_subterms()
{
   nsubterms = chain.size();
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
     {
	nsubterms += (*it)->count_subterms();
     }
   return nsubterms;
}
//                                                                        
cl_term* cl_term::get_subterm(int num)
{
   if (num < 0)
     {
	cerr<<"Error | cl_term::get_sub_term | num < 0"<<endl;
	exit(EXIT_FAILURE);
     }
   if (num == 0)
     return this;
   num--;
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
       {
	  if (num <= (*it)->nsubterms)
	    return (*it)->get_subterm(num);
	  num -= (*it)->nsubterms + 1;
       }
   cerr<<"Error | cl_term::get_sub_term | internal error"<<endl;
   exit(EXIT_FAILURE);
   return NULL;
}
//                                                                        
void cl_term::crossover_s1(cl_term* a)
{
   if (a == this)
     {
	cerr<<"Error | cl_term::crossover_s1 | cannot make crossover with selfmem"<<endl;
	exit(EXIT_FAILURE);
     }
   int cnt1 =    count_subterms();
   int cnt2 = a->count_subterms();

   cl_term *term1 =    get_subterm(random() %(cnt1 + 1));
   cl_term *term2 = a->get_subterm(random() %(cnt2 + 1) );
   
   term1->chain.swap(term2->chain);
   swap(term1->term, term2->term);
}
//                                                                        
void cl_term::echange_mutation(double p, string alphabet)
{
   if ( random()/(double)RAND_MAX < p )
     {
	term = alphabet[random() % alphabet.size()];
     }
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
     (*it)->echange_mutation(p, alphabet);
}
//                                                                       
void cl_term::trim_mutation(double p)
{
   for (list<cl_term*>::iterator it = chain.begin(); it != chain.end() ; it++)
     {
	if (random()/(double)RAND_MAX < p)
	  {
	     delete *it;
	     chain.erase(it);
	     return;
	  }
	(*it)->trim_mutation(p);
     }
}
//                                                                       
void cl_term::add_postfix(string postfix)
{
   for(size_t i = 0 ; i < postfix.size() ;i++)
     {
	if (postfix[i] == ')' || postfix[i] == '(')
	  {
	     cerr<<"Error | cl_term::add_postfix | postfix shoudn't contain \'(\' or \')\'"<<endl;
	     exit(EXIT_FAILURE);
	  }
	chain.push_back(new cl_term(postfix[i], counter));
     }
}
//                                                                       
bool cl_term::is_nontrivial() 
{
   return chain.size() > 0 || term == CL_TS_TERM;
}
