//CompressionDict.h
// implements the CPACK compression algorithm

#ifndef CompressionDict_H
#define CompressionDict_H

#include "common.h"
#include "CompressionBlock.h"

class CompressionDict{
protected:
   static constexpr unsigned xml_cacheline_size_ = 64;
   
public:
   virtual unsigned incrementalCompress(const uint8_t* data, unsigned len, uint64_t address) = 0;
   virtual unsigned incrementalCompress(const uint8_t* data, unsigned len, uint64_t address, uint8_t* output) {
      assert(0); // unimplemented
   }
   virtual unsigned tryIncrementalCompress(const uint8_t* data, unsigned len, uint64_t) = 0;
   virtual unsigned estimateCompressibility(const uint8_t* data, const uint64_t address){
      assert(0);
      return 0;
   }
   virtual void resetDict() = 0;

   virtual unsigned calculateDecompressionLatency(unsigned pos) const {
      return 64/16 * (pos+1); // assuming 16B/cycle decompression latency
      // return 64/8 * (pos+1); // assuming 8B/cycle decompression latency
   };
   virtual void removeCacheline(uint64_t a){
      assert(0);
   };
   virtual void removeCacheline(const uint8_t* a) {
      assert(0);
   };
   virtual void removeCacheline(uint64_t a, const uint8_t* data) {
      assert(0);
   };

   virtual uint flush(){return 0;};
};

#endif
