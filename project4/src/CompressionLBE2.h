//CompressionLBE2.h
// implements the CPACK compression algorithm

#ifndef CompressionLBE2_H
#define CompressionLBE2_H

#include "common.h"
#include "CompressionBlock.h"
#include "CompressionDict.h"
#include "DictLBE2.h"
#include <string.h>

class CompressionLBE2 : public CompressionDict, public CompressionBlock{
   DictLBE2* dict_ = nullptr;
   unsigned symbol_count_ = 0;
   vector<unsigned> decomp_latency_line_;

   unsigned getPointerSize(unsigned dictsize) const;
   unsigned compress(const uint8_t* address, unsigned len,
        DictLBE2& dict, unsigned& symbol_count) const;
   // unsigned compress(const uint8_t* address, unsigned len);
   
   // added for derived classes
   virtual void onDictFull(){};
   // virtual void onDictFullWhileCompressing(){};
   // void onDictFull() const{}; // definitely do nothing on the const version
   inline void onDictFullWhileCompressing() const{}; // definitely do nothing on the const version
   // inline void addDict32(uint32_t word, set<uint32_t>& dict32) const{dict32.emplace(word);};
   // void addDict32(uint32_t word, set<uint32_t>& dict32) const{dict32.emplace(word)};

   static constexpr unsigned xml_u8_size_ = 4;
   static constexpr unsigned xml_u16_size_ = 3;
   static constexpr unsigned xml_u32_size_ = 2;
   static constexpr unsigned xml_m32_size_ = 2;
   static constexpr unsigned xml_z32_size_ = 4;
   static constexpr unsigned xml_m64_size_ = 4;
   static constexpr unsigned xml_z64_size_ = 5;
   static constexpr unsigned xml_m128_size_ = 5;
   static constexpr unsigned xml_z128_size_ = 5;
   static constexpr unsigned xml_m256_size_ = 5;
   static constexpr unsigned xml_z256_size_ = 5;



public:
   // unsigned compressBlock(const uint8_t* address, unsigned len) const override;
   unsigned incrementalCompress(const uint8_t* address, unsigned len, uint64_t) override;
   unsigned tryIncrementalCompress(const uint8_t* address, unsigned len, uint64_t) override;

   void resetDict() override{
      dict_->reset();
      decomp_latency_line_.clear();
      symbol_count_ = 0;
   };
   void reset() override{resetDict();}

   CompressionLBE2(unsigned dictsize = 512)
   : dict_{new DictLBE2(dictsize)}
   {}

   CompressionLBE2(DictLBE2* dict)
   : dict_{dict}
   {}

   unsigned estimateCompressibility(const uint8_t* data, const uint64_t address) override;
   unsigned compressBlock(const uint8_t* address, unsigned len) const;
   unsigned compressBlock(const uint8_t* address, unsigned len);
   bool isDictionaryFull() const{return dict_->isFull();};

   unsigned calculateDecompressionLatency(unsigned pos) const override;
   DictLBE2* setDictionary(DictLBE2* dict){
      auto prev = dict_;
      dict_ = dict;
      return prev;
   }

   // profiling decompression time
private:
   static map<unsigned, unsigned> profiling_decompression_latency_;

   static void logDecompressionLatency(unsigned latency){
      profiling_decompression_latency_[latency] += 1;
   }

public:
   static void profilingDecompressionLatencyResetStats(){
      profiling_decompression_latency_.clear();
   }

   static void profilingDecompressionLatencyPrint(){
      // sort
      // const auto& v = sortMap(profiling_decompression_latency_);
      cerr << "SourceCodeCompressibilityProfiler" << endl;
      for (const auto& e : profiling_decompression_latency_){
         cerr << e.first << ": " << e.second << endl;
      }
   }

   static void profilingDecompressionLatencyRegression(RegressionStats& stats){
      // merge stats first
      static map<unsigned, unsigned> merged;
      for (const auto& e : profiling_decompression_latency_){
         unsigned val = e.first / 64;
         if (val > 8) val = 8;

         merged[val] += e.second;
      }

      for (const auto& e : merged){
         string n = string("torc_latency_") + to_string(e.first);
         stats[n] = e.second;
      }
   }


};

#endif
