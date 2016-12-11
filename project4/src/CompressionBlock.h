//CompressionBlock.h
// implements the CPACK compression algorithm

#ifndef CompressionBlock_H
#define CompressionBlock_H

#include "common.h"

class CompressionBlock{
   uint64_t address_;
public:
   virtual unsigned compressBlock(const uint8_t* data, unsigned len) const = 0;
   virtual unsigned compressBlock(const uint8_t* data, unsigned len){
      return static_cast<const CompressionBlock*>(this)->compressBlock(data, len);
   }

   virtual void reset(){};
};

#endif
