
#ifndef _COMMON_H
#define _COMMON_H

// some frequently used STLs
#include <algorithm>
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <string>
//using std::string;
//using std::to_string;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <unordered_set>
//using std::unordered_set;
#include <unordered_map>
using std::unordered_map;
using std::make_pair;
#include <deque>
using std::deque;
// #include <pair>
using std::pair;
#include <map>
using std::map;
#include <cassert>
#include <cmath>

#include <iostream>
using std::cerr;
using std::endl;

// for zipping output streams

// #include "MPIFormat.h"
// #include "MemoryFormat.h"
std::string to_string(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

using PTID = pair<unsigned, unsigned>;


class Uncopyable{
protected:
   // allow construction
   Uncopyable() {} // and destruction of
   ~Uncopyable() {} // derived objects...
private:
   Uncopyable(const Uncopyable&);
   // ...but prevent copying
   Uncopyable& operator=(const Uncopyable&);
};

// global variables
// #include "GlobalConfig.h"
// extern map<string, unsigned> global_switch_;
// extern map<string, string> global_switch_string_;

extern bool global_debug_log_enable_;
extern bool global_stat_resetted_;

using RegressionStats = map<string, uint64_t>;
using RegressionStatsVector = vector<pair<string, uint64_t>>;

#endif
