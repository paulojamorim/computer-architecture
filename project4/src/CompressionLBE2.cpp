//CompressionLBE2.cpp


#include "CompressionLBE2.h"


static bool onBoundary(unsigned bytes, unsigned i){
   if (bytes == 256)
      return (i % 8 == 0);
   else if (bytes == 128)
      return (i % 4 == 0);
   else if (bytes == 64)
      return (i % 2 == 0);
   else if (bytes == 32)
      return (i % 1 == 0);
   else
      assert(0);
}

unsigned CompressionLBE2::compress(const uint8_t* address, unsigned len,
      DictLBE2& dict, unsigned& symbol_count) const
{
   uint32_t * words = (uint32_t*)address;
   uint64_t * words64 = (uint64_t*)address;
   uint128_t * words128 = (uint128_t*)address;
   uint256_t * words256 = (uint256_t*)address;

   // CPACK compresses by word, each 32b
   unsigned compression_size_b = 0;
   // cout << "Starting compression..." << endl; // debug
   // for (unsigned i = 0; i < len/sizeof(uint32_t); i++){
   unsigned i = 0;
   while (i < len/sizeof(uint32_t)){
      if (dict.isFull())
         onDictFullWhileCompressing();
      bool dict_not_full = !dict.isFull();

      if (onBoundary(256, i)){
         uint256_t word = words256[i/8];

         if (word.isZero()){
            compression_size_b += xml_z256_size_;
            i += sizeof(uint256_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else if (dict.count(word)){
            compression_size_b += xml_m256_size_ + dict.pointerSize256();
            i += sizeof(uint256_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else{
            if (dict_not_full){
               dict.emplace(word);
            }
         }
      }

      if (onBoundary(128, i)){
         uint128_t word = words128[i/4];
         if (word.isZero()){
            compression_size_b += xml_z128_size_;
            i += sizeof(uint128_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else if (dict.count(word)){
            compression_size_b += xml_m128_size_ + dict.pointerSize128();
            i += sizeof(uint128_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else{
            if (dict_not_full){
               dict.emplace(word);
            }
         }
      }

      if (onBoundary(64, i)){
         uint64_t word = words64[i/2];
         if (word == 0){
            compression_size_b += xml_z64_size_;
            i += sizeof(uint64_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else if (dict.count(word)){
            compression_size_b += xml_m64_size_ + dict.pointerSize64();
            i += sizeof(uint64_t) / sizeof(uint32_t);
            symbol_count += 1;
            continue;
         }
         else{
            if (dict_not_full){
               dict.emplace(word);
            }
         }
      }

      // 32b
      uint32_t word = words[i];
      if (word == 0){
         compression_size_b += xml_z32_size_;
         i += 1;
         symbol_count += 1;
         continue;
      }
      else{
         auto upper16 = word >> 16;
         auto upper24 = word >> 8;

         if (dict.count(word)){
            compression_size_b += xml_m32_size_;
            compression_size_b += dict.pointerSize32();
            i += 1;
            symbol_count += 1;
            continue;
         }
         else{
            if (upper24 == 0)
               compression_size_b += xml_u8_size_ + 8;
            else if (upper16 == 0)
               compression_size_b += xml_u16_size_ + 16;
            else
               compression_size_b += xml_u32_size_ + 32;

            if (dict_not_full){
               dict.emplace(word);
            }
            i += 1;
            symbol_count += 1;
            continue;
         }
      }
      assert(0);
   }

   // cout << "       total: " << dec << compression_size_b << endl; // debug
   if (compression_size_b > 512)
      compression_size_b = 512;
   return compression_size_b;
}

// unsigned CompressionLBE2::compressBlock(const uint8_t* address, unsigned len) const{
//     unordered_set<uint32_t> dictionary;
//     dictionary.clear();
//     return compress(address, len, dictionary);
// }


unsigned CompressionLBE2::incrementalCompress(const uint8_t* address, unsigned len, uint64_t){
   // auto last_symbol_count = symbol_count_;
   auto ret = compress(address, len, *dict_, symbol_count_);
   decomp_latency_line_.push_back(symbol_count_);

   return ret;
}

unsigned CompressionLBE2::tryIncrementalCompress(const uint8_t* address, unsigned len, uint64_t){
   dict_->saveState();
   auto ret = compress(address, len, *dict_, symbol_count_);
   dict_->restoreState();
   return ret;
}

unsigned CompressionLBE2::compressBlock(const uint8_t* address, unsigned len) const{
   unsigned unused;
   auto ret = compress(address, len, *dict_, unused);
   dict_->reset();
   return ret;
}

unsigned CompressionLBE2::compressBlock(const uint8_t* address, unsigned len){
   unsigned ret = incrementalCompress(address, len, 0);
   if (isDictionaryFull())
      onDictFull();
   return ret;
}


unsigned CompressionLBE2::estimateCompressibility(const uint8_t* data, const uint64_t address){
   // uint32_t word32 = *((uint32_t*)data);
   // uint64_t word64 = *((uint64_t*)data);
   // uint128_t word128 = *((uint128_t*)data);
   // uint256_t word256 = *((uint256_t*)data);

   // if (dict_->estimate(word256))
   //    return (256 - 256) * 2;
   // if (dict_->estimate(word128))
   //    return (256 - 128) * 2;
   // if (dict_->estimate(word64))
   //    return (256 - 64) * 2;
   // if (dict_->estimate(word32))
   //    return (256 - 32) * 2;

   // return 512;

   return tryIncrementalCompress(data, 64, address);
}

unsigned CompressionLBE2::calculateDecompressionLatency(unsigned pos) const{
   unsigned latency2;
#if 1
   pos = decomp_latency_line_.size() < pos ? decomp_latency_line_.size() : pos;
   unsigned latency = decomp_latency_line_[pos];
   latency2 = latency / 2; if (latency % 2) latency2 += 1;

#else
   latency2 = 4 * pos;
#endif

   logDecompressionLatency(latency2);
   return latency2;

   // return 8;
}

map<unsigned, unsigned> CompressionLBE2::profiling_decompression_latency_;