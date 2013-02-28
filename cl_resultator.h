//cl_resultator.h

/*
   Copyright (C) 2013  AIDEUS
   authors: Sergey Rodionov (astroseger@gmail.com)
            Alexey Potapov  (potapov@aideus.com)
   aideus.com

   This file is part of cl-lab.

   cl-lab is released under the GNU General Public License (GNU GPL).
   Please read the included file COPYING for more information.
*/


//class which assess results on the fly

#ifndef _CL_RESULTATOR_H__
#define _CL_RESULTATOR_H__
#include <string>
#include <vector>
using namespace std;

class cl_resultator
{
 public:   
   virtual ~cl_resultator(){};
   
   //put next character
   //return 0  - we should continue because result is unknown
   //return -1 - we shoule stop --> result is negative anyway
   //return 1  - we should stop --> result is positive
   virtual int put_next(char c) = 0; 
};
//                                                          

//class useful for brut force when we wait exact result
class cl_resultator_exact:public cl_resultator
{
 public:
   cl_resultator_exact(string req_rez_, string ignore_)
     :req_rez(req_rez_), ignore(ignore_){};
   int put_next(char c);
   
   string get_rez(){return rez;};
 private: 
   string req_rez; //required result
   string ignore;  //list of characters to ignore (when we want to ignore SK for example)
   string rez;     //current result
};
//                                                          
//class useful for GA when we only need lenght of result
class cl_resultator_length:public cl_resultator
{
 public:
   cl_resultator_length(size_t req_length_, string ignore_)
     :req_length(req_length_),ignore(ignore_){};
   int put_next(char c);
   
   string get_rez(){return rez;};
      
 private:
   size_t req_length; //required result length
   string ignore;     //list of characters to ignore (when we want to ignore SK for example)
   string rez;        //current result   
};

//class useful for brut force when we need multiply different results
class cl_resultator_multexact:public cl_resultator
{
 public:
   cl_resultator_multexact(const vector<string>& req_rez_, string ignore_)
     :req_rez(req_rez_), ignore(ignore_), status(0){};
   int put_next(char c);   

   string get_rez(){return rez;};   
   const vector<string>& get_founds() {return founds;};
   //get string for which resultat is unknown
   //so it means their prefix is equal to rez, but rez.size() is less than their length
   void get_not_finished(vector<string>& non_finished);
 private:
   int check_last_char(string rr); //0 -- we should continue
                                   //-1 -- we've finished --- fail
                                   //0  -- we've finished --- good!
 private: 
   vector<string> req_rez; //required result. if req_rez[i] == 0 --> we've finished with it...
   string ignore;          //list of characters to ignore (when we want to ignore SK for example)
   int status;             //current status
   vector<string> founds;   //found strings
   string rez;             //current result
};


#endif
