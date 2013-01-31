//cl_brute.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_brute.h"

void cl_brute_all_brackets(int i1, int i2, vector< vector< pair<int,int> > >& allb)
{
   //1. no brackets at all
   allb.push_back( vector< pair<int,int> >() );
   
//   cout<<i1<<" "<<i2<<endl;
   for (int pos1 = i1; pos1 < i2 - 1 ; pos1++)
     for (int pos2 = pos1 + 2; pos2 <= i2 ; pos2++)
       {
	  pair<int,int> main_brackets(pos1,pos2);	  
	  vector< vector< pair<int,int> > > inside_main, after_main;
	  cl_brute_all_brackets(pos1 + 1, pos2, inside_main);
	  cl_brute_all_brackets(pos2, i2, after_main);
	  
	  for (size_t ii = 0 ; ii < inside_main.size() ; ii++)
	    for (size_t ia = 0 ; ia < after_main.size() ; ia++)
	      {
		 vector< pair<int,int> > b;
		 //add main brackets
		 b.push_back(main_brackets);
		 //add brackets inside main
		 b.insert(b.end(), inside_main[ii].begin(), inside_main[ii].end());
		 //add brackets after main
		 b.insert(b.end(), after_main[ia].begin(), after_main[ia].end());
		 allb.push_back(b);
	      }
       }
}
//                                                                                    
void cl_brute_apply_brackets(string s, const vector< pair<int,int> >& brackets,  string& rez)
{
   vector< string > ss(s.size());
   for (size_t i =  0 ; i < s.size() ; i++)
     ss[i] = s[i];
   for (size_t i = 0 ; i < brackets.size() ; i++)
     {
	size_t pos1 = brackets[i].first;
	size_t pos2 = brackets[i].second;
	if (pos1 < 0 || pos1 >= ss.size() ||
	    pos2 < 0 || pos2 >= ss.size())
	  {
	     cerr<<"Error | cl_brute_apply_brackets | internal error"<<endl;
	     exit(EXIT_FAILURE);
	  }
	ss[pos1].push_back('(');
	ss[pos2].push_back(')');
     }
   rez.clear();
   for (size_t i = 0 ; i < ss.size() ; i++)
     rez.append(ss[i]);
}
//                                                                                       
//                                                                                       
void cl_brute_generator::init(int l_, string alphabet_)
{
   l        = l_;
   alphabet = alphabet_;
   cl_brute_all_brackets(0, l - 1, allb);
   cs     = vector<size_t>(l, 0);
   cb     = 0;
}
//                                                                                       
bool cl_brute_generator::get_next(string& s)
{
   s = "";
   if (cb == allb.size()) //finish
     return false;
   generate_string(s);
   if (!get_next_cs())
     {
	//put zeros to cs again
	fill(cs.begin(), cs.end(), 0);
	cb++; //and take new bracket set
     }
   return true;
}
//                                                                                       
string cl_brute_generator::create_random()
{
   cb = random() % allb.size();
   for (size_t i = 0 ; i < cs.size() ; i++)
     cs[i] = random() % alphabet.size();
   string s;
   generate_string(s);
   return s;
}
//                                                                                       
bool cl_brute_generator::get_next_cs()
{
   size_t ci = cs.size() - 1; //current index in cs
  
   while(cs[0] != alphabet.size())
     {
	cs[ci]++;
	if (cs[ci] != alphabet.size())
	  {
	     return true;
	  }
	if (ci != 0)
	  {
	     cs[ci] = 0;
	     ci--;
	  }	
     }
   return false;
}
//                                                                                       
void cl_brute_generator::generate_string(string& s)
{
   string bfs;
   bfs.resize(cs.size()); //bracket free string 
   for (size_t i = 0 ; i < cs.size() ; i++)
     bfs[i] = alphabet.at(cs[i]);
   cl_brute_apply_brackets(bfs, allb.at(cb), s);
}
//                                                                                       
