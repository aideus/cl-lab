//cl_strga.h
/*
 *    Copyright (C) 2013  AIDEUS
 *    authors: Sergey Rodionov (astroseger@gmail.com)
 *             Alexey Potapov  (potapov@aideus.com)
 *    aideus.com
 * 
 *    This file is part of cl-lab.
 * 
 *    cl-lab is released under the GNU General Public License (GNU GPL).
 *    Please read the included file COPYING for more information.
 * */

#include <string>
using namespace std;


#ifndef __SEGER__CL_STRGA_H__
#define __SEGER__CL_STRGA_H__

void cl_str_crossover(string& s1, string& s2);
void cl_str_exchange_mutation(string& s, double p, string alphabet);
void cl_str_trim_mutation(string&s, double p);

//create random substring with non zero length
void cl_str_random_substr(string s, size_t& p, size_t& len);

string cl_str_make_random(string alphabet, size_t size);

#endif
