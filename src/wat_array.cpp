/* 
 *  Copyright (c) 2010 Daisuke Okanohara
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#include "wat_array.hpp"

using namespace std;

namespace wat_array {

WatArray::WatArray() : alphabet_num_(0), alphabet_bit_num_(0), length_(0){
}
  
WatArray::~WatArray() {
}

void WatArray::Clear(){
  vector<BitArray>().swap(bit_arrays_);
  occs_.Clear();
  alphabet_num_ = 0;
  alphabet_bit_num_ = 0;
  length_ = 0;
}

void WatArray::Init(const vector<uint64_t>& array){
  Clear();
  alphabet_num_     = GetAlphabetNum(array);
  alphabet_bit_num_ = Log2(alphabet_num_-1);
  length_           = static_cast<uint64_t>(array.size());
  SetArray(array);
  SetOccs(array);
}

uint64_t WatArray::Lookup(uint64_t pos) const{
  if (pos >= length_) return NOTFOUND;
  uint64_t st = 0;
  uint64_t en = length_;
  uint64_t c = 0;
  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    const BitArray& ba = bit_arrays_[i];
    uint64_t boundary  = st + ba.Rank(0, en) - ba.Rank(0, st);
    uint64_t bit       = ba.Lookup(st + pos);
    c <<= 1;
    if (bit){
      pos = ba.Rank(1, st + pos) - ba.Rank(1, st);
      st = boundary;
      c |= 1LLU;
    } else {
      pos = ba.Rank(0, st+ pos) - ba.Rank(0, st);
      en = boundary;
    }
    
  }
  return c;	 
}

uint64_t WatArray::Rank(uint64_t c, uint64_t pos) const{
  if (c >= alphabet_num_)  return NOTFOUND;
  if (pos > length_) return NOTFOUND;
  uint64_t st = 0;
  uint64_t en = length_;

  for (size_t i = 0; i < bit_arrays_.size() && en > st; ++i){
    const BitArray& ba = bit_arrays_[i];
    uint64_t st_zero   = ba.Rank(0, st);
    uint64_t en_zero   = ba.Rank(0, en);
    uint64_t boundary  = st + en_zero - st_zero;
    uint64_t bit       = GetMSB(c, i, bit_arrays_.size());
    if (!bit){
      pos = st + ba.Rank(0, pos) - st_zero;
      en = boundary;
    } else {
      pos = boundary + ba.Rank(1, pos) - (st - st_zero);
      st = boundary;
    }
  }
  return pos - st;
}

uint64_t WatArray::Select(uint64_t c, uint64_t rank) const{
  if (c >= alphabet_num_) {
    return NOTFOUND;
  }
  if (rank > Freq(c)){
    return NOTFOUND;
  }

  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    uint64_t lower_c = c & ~((1LLU << (i+1)) - 1);
    uint64_t st  = occs_.Select(1, lower_c  + 1) - lower_c;
    const BitArray& ba = bit_arrays_[alphabet_bit_num_ - i - 1];
    uint64_t bit = GetLSB(c, i);
    uint64_t before_rank = ba.Rank(bit, st);
    rank = ba.Select(bit, before_rank + rank) - st + 1;
  }
  return rank - 1;
}


uint64_t WatArray::RankLessThan(uint64_t c, uint64_t pos) const{
  if (c > alphabet_num_) return NOTFOUND;
  if (pos > length_) return NOTFOUND;
  uint64_t st   = 0;
  uint64_t en   = length_;
  uint64_t rank = 0;
  for (size_t i = 0; i < bit_arrays_.size() && en > st; ++i){
    const BitArray& ba = bit_arrays_[i];
    uint64_t st_zero   = ba.Rank(0, st);
    uint64_t en_zero   = ba.Rank(0, en);
    uint64_t boundary  = st + en_zero - st_zero;
    uint64_t bit       = GetMSB(c, i, bit_arrays_.size());
    if (!bit){
      pos = st + ba.Rank(0, pos) - st_zero;
      en = boundary;
    } else {
      rank += ba.Rank(0, pos) - st_zero;
      pos = boundary + ba.Rank(1, pos) - (st - st_zero);
      st = boundary;
    }
  }
  return rank;
}

uint64_t WatArray::RankRange(uint64_t min_c, uint64_t max_c, uint64_t begin_pos, uint64_t end_pos) const{
  if (min_c >= alphabet_num_) return 0;
  if (max_c <= min_c) return 0;
  if (end_pos > length_ || begin_pos > end_pos) return 0;
  return 
    + RankLessThan(end_pos,   max_c)
    - RankLessThan(end_pos,   min_c)
    - RankLessThan(begin_pos, max_c)
    + RankLessThan(begin_pos, min_c);
}

void WatArray::RangeMaxQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const {
  RangeQuery(begin_pos, end_pos, 0, MAX_QUERY, pos, val);
} 

void WatArray::RangeMinQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const {
  RangeQuery(begin_pos, end_pos, 0, MIN_QUERY, pos, val);
}

void WatArray::RangeTopKQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t k, uint64_t& pos, uint64_t& val) const {
  RangeQuery(begin_pos, end_pos, k, KTH_QUERY, pos, val);
}


void WatArray::RangeQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t k, Strategy strategy, uint64_t& pos, uint64_t& val) const {
  if (end_pos > length_ || begin_pos >= end_pos) {
    pos = NOTFOUND;
    val = NOTFOUND;
    return;
  }
  
  val = 0;
  uint64_t st = 0;
  uint64_t en = length_;
  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    const BitArray& ba = bit_arrays_[i];
    uint64_t st_zero   = ba.Rank(0, st);
    uint64_t en_zero   = ba.Rank(0, en);
    uint64_t st_one    = st - st_zero;
    uint64_t beg_zero  = ba.Rank(0, begin_pos);
    uint64_t end_zero  = ba.Rank(0, end_pos);
    uint64_t beg_one   = begin_pos - beg_zero;
    uint64_t end_one   = end_pos - end_zero;
    uint64_t boundary  = st + en_zero - st_zero;
    
    if (ChooseLeftChild(end_zero - beg_zero, 
			end_one  - beg_one,
			k,
			strategy)){
      en        = boundary; 
      begin_pos = st + beg_zero - st_zero;
      end_pos   = st + end_zero - st_zero;
      val       = val << 1;
    } else {
      st        = boundary; 
      begin_pos = boundary + beg_one - st_one;
      end_pos   = boundary + end_one - st_one;
      val       = (val << 1) + 1;

      k -= end_zero - beg_zero;
    }
  }

  uint64_t rank = begin_pos - st;
  pos = Select(val, rank+1);
}

bool WatArray::ChooseLeftChild(uint64_t zero_num, uint64_t one_num, uint64_t k, Strategy strategy) const{
  if (strategy == MAX_QUERY){
    if (one_num > 0) return false;
    return true;
  } else if (strategy == MIN_QUERY){
    if (zero_num > 0) return true;
    return false;
  } else if (strategy == KTH_QUERY){
    if (zero_num > k) return true; 
    return false;
  } else {
    return true; // not support
  }
}

void WatArray::ListRange(uint64_t begin_pos, uint64_t end_pos, uint64_t num, vector<ListResult>& res) const {
  res.clear();
  if (end_pos > length_) return;

  ListRangeRec(0, length_, begin_pos, end_pos, num, 0, 0, res);
}

void WatArray::ListRangeRec(uint64_t st, uint64_t en, uint64_t begin_pos, uint64_t end_pos, uint64_t num, 
			    uint64_t depth, uint64_t c, vector<ListResult>& res) const {
  if (res.size() >= num) return;
  if (depth >= alphabet_bit_num_){
    ListResult lr;
    lr.c = c;
    lr.freq = end_pos - begin_pos;
    res.push_back(lr);
    return; 
  };

  const BitArray& ba = bit_arrays_[depth];
  
  uint64_t st_zero   = ba.Rank(0, st);
  uint64_t en_zero   = ba.Rank(0, en);
  uint64_t st_one    = st - st_zero;
  uint64_t beg_zero  = ba.Rank(0, begin_pos);
  uint64_t end_zero  = ba.Rank(0, end_pos);
  uint64_t beg_one   = begin_pos - beg_zero;
  uint64_t end_one   = end_pos - end_zero;
  uint64_t boundary  = st + en_zero - st_zero;
  if (end_zero - beg_zero > 0) { // zero
    ListRangeRec(st, boundary, st + beg_zero - st_zero, st + end_zero - st_zero, num, depth+1, c << 1, res);
  }

  if (end_one - beg_one > 0){ // one
    ListRangeRec(boundary, en, boundary + beg_one - st_one, boundary + end_one - st_one, num, depth+1, (c << 1) + 1, res);
  } 
}

uint64_t WatArray::Freq(uint64_t c) const {
  if (c >= alphabet_num_) return NOTFOUND;
  return occs_.Select(1, c+2) - occs_.Select(1, c+1) - 1;  
}

uint64_t WatArray::alphabet_num() const{
  return alphabet_num_;
}

uint64_t WatArray::length() const{
  return length_;
}

uint64_t WatArray::GetAlphabetNum(const std::vector<uint64_t>& array) const {
  uint64_t alphabet_num = 0;
  for (size_t i = 0; i < array.size(); ++i){
    if (array[i] >= alphabet_num){
      alphabet_num = array[i]+1;
    }
  }
  return alphabet_num;
}

uint64_t WatArray::Log2(uint64_t x) const{
  uint64_t bit_num = 0;
  while (x >> bit_num){
    ++bit_num;
  }
  return bit_num;
}

uint64_t WatArray::PrefixCode(uint64_t x, uint64_t len, uint64_t bit_num) const{
  return x >> (bit_num - len);
}

uint64_t WatArray::GetMSB(uint64_t x, uint64_t pos, uint64_t len) const {
  return (x >> (len - (pos + 1))) & 1LLU;
}

uint64_t WatArray::GetLSB(uint64_t x, uint64_t pos) const {
  return (x >> pos) & 1LLU;
}

void WatArray::SetArray(const vector<uint64_t>& array) {
  if (alphabet_num_ == 0) return;
  bit_arrays_.resize(alphabet_bit_num_, length_);

  vector<vector<uint64_t> > beg_poses;
  GetBegPoses(array, alphabet_bit_num_, beg_poses);
  
  for (size_t i = 0; i < array.size(); ++i){
    uint64_t c = array[i];
    for (uint64_t j = 0; j < alphabet_bit_num_; ++j){
      uint64_t prefix_code = PrefixCode(c, j, alphabet_bit_num_);
      uint64_t bit_pos     = beg_poses[j][prefix_code]++;
      bit_arrays_[j].SetBit(GetMSB(c, j, alphabet_bit_num_), bit_pos);
    }
  }
 
  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    bit_arrays_[i].Build();
  }
}

void WatArray::SetOccs(const vector<uint64_t>& array){
  vector<uint64_t> counts(alphabet_num_);
  for (size_t i = 0; i < array.size(); ++i){
    counts[array[i]]++;
  }

  occs_.Init(array.size() + alphabet_num_ + 1);
  uint64_t sum = 0;
  for (size_t i = 0; i < counts.size(); ++i){
    occs_.SetBit(1, sum);
    sum += counts[i] + 1;
  }
  occs_.SetBit(1, sum);
  occs_.Build();
}

void WatArray::GetBegPoses(const vector<uint64_t>& array, 
			   uint64_t alpha_bit_num, 
			   vector< vector<uint64_t> >& beg_poses) const{
  beg_poses.resize(alpha_bit_num);
  for (uint64_t i = 0; i < beg_poses.size(); ++i){
    beg_poses[i].resize(1 << i);
  }
  
  for (size_t i = 0; i < array.size(); ++i){
    uint64_t c = array[i];
    for (uint64_t j = 0; j < alpha_bit_num; ++j){
      beg_poses[j][PrefixCode(c, j, alpha_bit_num)]++;
    }
  }

  for (size_t i = 0; i < beg_poses.size(); ++i){
    uint64_t sum = 0;
    vector<uint64_t>& beg_poses_level = beg_poses[i];
    for (size_t j = 0; j < beg_poses_level.size(); ++j){
      uint64_t num = beg_poses_level[j];
      beg_poses_level[j] = sum;
      sum += num;
    }
  }
}

void WatArray::Save(ostream& os) const{
  os.write((const char*)(&alphabet_num_), sizeof(alphabet_num_));
  os.write((const char*)(&length_), sizeof(length_));
  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    bit_arrays_[i].Save(os);
  }
  occs_.Save(os);
}

void WatArray::Load(istream& is){
  Clear();
  is.read((char*)(&alphabet_num_), sizeof(alphabet_num_));
  alphabet_bit_num_ = Log2(alphabet_num_);
  is.read((char*)(&length_), sizeof(length_));

  bit_arrays_.resize(alphabet_bit_num_);
  for (size_t i = 0; i < bit_arrays_.size(); ++i){
    bit_arrays_[i].Load(is);
    bit_arrays_[i].Build();
  }
  occs_.Load(is);
  occs_.Build();
}

}
