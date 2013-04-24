//cl_termstorage.cpp

#include "cl_termstorage.h"

cl_termstorage::cl_termstorage(cl_term& cl)
{
   is_reduced = false;
   
   //transfer cl to this->t
   this->t = new cl_term(cl.term, cl.counter);
   if (cl.term == CL_TS_TERM)
     {
	this->t->ts = cl.ts;
     }
   swap(this->t->chain, cl.chain);
   norigins = 1; //cl
   cl.ts = this;
   cl.term = CL_TS_TERM;
}
//                                                             
void cl_termstorage::add_new_origin(cl_term* new_cl)
{
   new_cl->term = CL_TS_TERM;
   new_cl->ts   = this;
   norigins++;   
}
//                                                             
cl_termstorage::~cl_termstorage()
{
   if (norigins != 0)
     {
	cerr<<"error | ~cl_termstorage::cl_termstorage | non empty origins"<<endl;
	exit(EXIT_FAILURE);
     }  
   delete t;
}
                                                              
