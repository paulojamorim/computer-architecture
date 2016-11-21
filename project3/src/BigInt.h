//BigInt.h
// implements the CPACK compression algorithm

#ifndef BigInt_H
#define BigInt_H

#include "common.h"

struct uint128_t{
   uint64_t num[2];
   bool operator< (const uint128_t& rhs) const{
      for (unsigned i = 0; i <= 1; ++i){
         if (num[i] < rhs.num[i])
            return true;
         else if (num[i] > rhs.num[i])
            return false;
      }

      // totally equal
      return false;
   }

   bool isZero() const{
      return (num[0] == 0 && num[1] == 0);
   }

   bool operator==(uint64_t rhs) const{
      if (num[1] != 0)
         return false;
      return (num[0] == rhs);
   }
};

struct uint256_t{
   uint64_t num[4];
   bool operator< (const uint256_t& rhs) const{
      for (unsigned i = 0; i <= 3; ++i){
         if (num[i] < rhs.num[i])
            return true;
         else if (num[i] > rhs.num[i])
            return false;
      }

      // totally equal
      return false;
   }

   bool isZero() const{
      return (num[0] == 0 && num[1] == 0 && num[2] == 0 && num[3] == 0);
   }

   bool operator==(uint64_t rhs) const{
      if (num[3] != 0 || num[2] != 0 || num[1] != 0)
         return false;
      return (num[0] == rhs);
   }
};

#endif
