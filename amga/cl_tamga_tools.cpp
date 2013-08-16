//cl_amga_tools.cpp

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/



#include "cl_tamga_tools.h"
#include <cmath>
#include <map>
#include <set>
#include <algorithm>

void cl_tamga_member::calc_penalty(string valuator_type, double pen_progsize, double pen_rezgood, double pen_rezbad)
{
   if (pen_rezgood >= pen_rezbad)
     {
	cout<<"Error | cl_tamga_member::calc_penalty | pen_rezgood should be smaller than pen_rezbad"<<endl;
	exit(EXIT_FAILURE);
     }
   if (valuator_type == "a")
     {
	part_LP = term_str.size() * pen_progsize;
	size_t ngood, lgood, nbad, lbad;
	cl_tamga_rez_analysis(rez, ngood, lgood, nbad, lbad);
	part_Lgood = lgood * pen_rezgood;
	part_Lbad  = nbad  * pen_rezbad;
     }
   else if (valuator_type == "s")
     {
	part_LP = term_str.size() * pen_progsize;
	
	size_t ngood, lgood, nbad, lbad;	
	cl_tamga_rez_analysis(rez, ngood, lgood, nbad, lbad);
	//ngood + nbad = number of strings in all ansambles
	part_Lgood = lgood * pen_rezgood / double(ngood + nbad);
	part_Lbad  = nbad  * pen_rezbad  / double(ngood + nbad);  
     }
   else
     {
	cerr<<"Error | cl_tamga_member::calc_penalty | unknown valuator type="<<valuator_type<<endl;
	exit(EXIT_FAILURE);
     }
   penalty = part_LP + part_Lgood + part_Lbad;
}

//find iterator to the entrance with maximal value
map<string,size_t>::iterator cl_tamga_ra_find_maxval(map<string,size_t>& m);
void cl_tamga_ra_count_el(map<string,size_t>::iterator& it, size_t& n, size_t& l);
void cl_tamga_ra_count_all_el(map<string,size_t>& m, size_t& n, size_t& l);


//we calculate number of "good" results, and their total length
//and number of "bad" results, and their total length
void cl_tamga_rez_analysis(const vector< vector<string> >& rez, 
			   size_t& ngood, size_t& lgood,
			   size_t& nbad,  size_t& lbad )
{
   vector< map<string,size_t> > gmap(rez.size()); 
   for (size_t i = 0 ; i < rez.size() ; i++)
     for (size_t j = 0 ; j < rez[i].size() ; j++)
       {
	  if (gmap[i].find(rez[i][j]) == gmap[i].end()) //doesn't exist
	    gmap[i][rez[i][j]] = 1;
	  else
	    gmap[i][rez[i][j]]++;
       }
   ngood = 0;
   lgood = 0;
   nbad  = 0;
   lbad  = 0;
   
   vector< string > goods; //good strings
   
   //for first ansamble we assigne most frequently string as good.
   map<string,size_t>::iterator it = cl_tamga_ra_find_maxval(gmap[0]);
   goods.push_back(it->first);
   cl_tamga_ra_count_el(it, ngood, lgood);
   gmap[0].erase(it);
   cl_tamga_ra_count_all_el(gmap[0], nbad, lbad);
//   cout<<"l0 \""<<goods[0]<<"\" first="<<ngood<<" "<<lgood<<" "<<nbad<<" "<<lbad<<endl;
   
   //all other good string should be unique 
   for (size_t i = 1 ; i < rez.size();  i++)
     {
	bool is_good = false;	
	while((!is_good) && (gmap[i].size() != 0))
	  {
	     it = cl_tamga_ra_find_maxval(gmap[i]);
//	     cout<<"i="<<i<<" find "<<it->first<<" "<<it->second<<endl;
	     if (find(goods.begin(), goods.end(), it->first) != goods.end()) //already exists
	       {
		  cl_tamga_ra_count_el(it, nbad, lbad);
		  gmap[i].erase(it);
	       }
	     else
	       {
		  cl_tamga_ra_count_el(it, ngood, lgood);
		  goods.push_back(it->first);
		  gmap[i].erase(it);
		  cl_tamga_ra_count_all_el(gmap[i], nbad, lbad);
		  is_good = true;
	       }
	  }
     }
}


map<string,size_t>::iterator cl_tamga_ra_find_maxval(map<string,size_t>& m)
{
   map<string,size_t>::iterator it_rez = m.begin();
   for (map<string,size_t>::iterator it = m.begin() ; it != m.end(); it++)
     {
	if (it->second > it_rez->second)
	  it_rez = it;
     }
   return it_rez;
}
//                                                                         
void cl_tamga_ra_count_el(map<string,size_t>::iterator& it, size_t& n, size_t& l)
{
   n += it->second; //number of this strings
   l += it->second * (it->first).size(); //total length of this strings
}
//                                                                        
void cl_tamga_ra_count_all_el(map<string,size_t>& m, size_t& n, size_t& l)
{
   for (map<string,size_t>::iterator it = m.begin() ; it != m.end(); it++)
     cl_tamga_ra_count_el(it, n, l);
}

//                                                                       
