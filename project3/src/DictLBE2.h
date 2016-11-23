//DictLBE2.h
// implements the CPACK compression algorithm

#ifndef DictLBE2_H
#define DictLBE2_H

#include "common.h"
#include "BigInt.h"

class DictLBE2{
   set<uint32_t> dict32_[2];
   set<uint64_t> dict64_[2];
   set<uint128_t> dict128_[2];
   set<uint256_t> dict256_[2];
   unsigned dictsize_;
   unsigned copyid_ = 0;

public:
   DictLBE2(unsigned dictsize = 512)
   : dictsize_{dictsize / 4}
   {
      // dictsize is in byte
      // internal dictsize is in the number of 32b words, or 4B
      assert(dictsize % 4  == 0);
      assert(dictsize != 0);
   }

   unsigned getPointerSize(unsigned dictsize) const{
      unsigned ret = 0;
      while(dictsize != 0 && dictsize != 1){
         dictsize = dictsize >> 1;
         ret += 1;
      }
      return ret;
   }

   template<class T>
   bool count(const T& word){
      bool found = estimate(word);
      // if (found)
         // setUsed(word);
      return found;
   }

   bool estimate(const uint32_t& word) const{
      return dict32_[copyid_].count(word);
   }
   bool estimate(const uint64_t& word) const{
      return dict64_[copyid_].count(word);
   }
   bool estimate(const uint128_t& word) const{
      return dict128_[copyid_].count(word);
   }
   bool estimate(const uint256_t& word) const{
      return dict256_[copyid_].count(word);
   }

   void emplace(const uint32_t& word){
      dict32_[copyid_].insert(word);
   }
   void emplace(const uint64_t& word){
      dict64_[copyid_].insert(word);
   }
   void emplace(const uint128_t& word){
      dict128_[copyid_].insert(word);
   }
   void emplace(const uint256_t& word){
      dict256_[copyid_].insert(word);
   }

   virtual bool isFull() const{
      return (dict32_[copyid_].size() >= dictsize_);
   }

   virtual unsigned pointerSize32() const{
      return getPointerSize(dict32_[copyid_].size());
   }
   virtual unsigned pointerSize64() const{
      return getPointerSize(dict64_[copyid_].size());
   }
   virtual unsigned pointerSize128() const{
      return getPointerSize(dict128_[copyid_].size());
   }
   virtual unsigned pointerSize256() const{
      return getPointerSize(dict256_[copyid_].size());
   }

   void reset(){
      dict32_[0].clear();
      dict64_[0].clear();
      dict128_[0].clear();
      dict256_[0].clear();
      dict32_[1].clear();
      dict64_[1].clear();
      dict128_[1].clear();
      dict256_[1].clear();

      // resetProfiling();
   }
   void saveState(){
      dict32_[copyid_ ^ 1] = dict32_[copyid_];
      dict64_[copyid_ ^ 1] = dict64_[copyid_];
      dict128_[copyid_ ^ 1] = dict128_[copyid_];
      dict256_[copyid_ ^ 1] = dict256_[copyid_];
   }
   void restoreState(){
      copyid_ ^= 1;
      assert(copyid_ == 0 || copyid_ == 1);
   }
};

#endif
