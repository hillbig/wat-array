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

#ifndef WATARRAY_WATARRAY_HPP_
#define WATARRAY_WATARRAY_HPP_

#include <vector>
#include <stdint.h>
#include <iostream>
#include "bit_array.hpp"

namespace wat_array {

/**
 WAvelet Tree Array
 
 For an array A[0...n], 0 <= A[i] < k,
 WatArray supports several queries efficiently using n log_2 k bits.
 
 All following operations are supported in O(log k) time (constant for n)
 - Rank(c, i)   : Return the number of c in T[0...i)
 - Select(c, i) : Return the position of (i+1)-th c in T
 - RankRange(min_c, max_c, sp, ep) : Return the number of min_c <= c < max_c in T[0...i)
 */

class WatArray{
public:
  /**
   * Constructor
   */
  WatArray();

  /**
   * Destructor 
   */
  ~WatArray();

  /**
   * Initialize an index from an array
   * @param An array to be initialized
   */
  void Init(const std::vector<uint64_t>& array);

  /**
   * Clear and release the resouces
   */
  void Clear();

  /**
   * Lookup A[pos]
   * @param pos the position
   * @return return A[pos] if found, or return NOT_FOUND if pos >= length 
   */
  uint64_t Lookup(uint64_t pos) const;

  /**
   * Compute the rank = the frequency of a character 'c' in the prefix of the array A[0...pos)
   * @param c Character to be examined
   * @param pos The position of the prefix (not inclusive)
   * @return The frequency of a character 'c' in the prefix of the array A[0...pos)
   *         or NOT_FOUND if c >= alphabet_num or pos > length
   */
  uint64_t Rank(uint64_t c, uint64_t pos) const;

  /**
   * Compute the select = the position of the (rank+1)-th occurence of 'c' in the array.
   * @param c Character to be examined
   * @param rank The rank of the character
   * @return The position of the (rank+1)-th occurence of 'c' in the array. 
   *         or NOT_FOUND if c >= alphabet_num or rank > Freq(c)
   */
  uint64_t Select(uint64_t c, uint64_t rank) const;

  /**
   * Compute the frequency of characters c' < c in the prefix of the array A[0...pos)
   * @param c The upper bound of the character
   * @param pos The position of the end of the prefix (not inclusive)
   * @return The frequency of characters c' < c in the prefix of the array A[0...pos)
             or NOTFOUND if c > alphabet_num or pos > length
   */
  uint64_t RankLessThan(uint64_t c, uint64_t pos) const;

  /**
   * Compute the frequency of characters min_c <= c' < max_c in the subarray A[begin_pos ... end_pos)
   * @param min_c The smallerest character to be examined
   * @param max_c The uppker bound of the character to be examined
   * @param begin_pos The beginning position of the array (inclusive)
   * @param end_pos The ending position of the array (not inclusive)
   * @return The frequency of characters min_c <= c < max_c in the subarray A[begin_pos .. end_pos)
             or NOTFOUND if max_c > alphabet_num or end_pos > length
   */
  uint64_t RankRange(uint64_t min_c, uint64_t max_c, uint64_t begin_pos, uint64_t end_pos) const;

  /**
   * Range Max Query
   * @param begin_pos The beginning position
   * @param end_pos The ending position
   * @param pos The position where the largest value appeared in the subarray A[begin_pos .. end_pos)
                If there are many items having the largest values, the smallest pos will be reporeted
   * @param val The largest value appeared in the subarray A[begin_pos ... end_pos)
   */
  void RangeMaxQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const; 

  /**
   * Range Min Query
   * @param begin_pos The beginning position
   * @param end_pos The ending position
   * @param pos The position where the smallest value appeared in the subarray A[begin_pos .. end_pos)
                If there are many items having the smalles values, the smallest pos will be reporeted
   * @param val The smallest value appeared in the subarray A[begin_pos ... end_pos)
   */
  void RangeMinQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const; 

  /**
   * Range Top-K Query, Return the K-th largest value in the subarray
   * @param begin_pos The beginning position
   * @param end_pos The ending position
   * @param k The order (should be smaller than end_pos - begin_pos).
   * @param pos The position where the k-th largest value appeared in the subarray A[begin_pos .. end_pos)
                If there are many items having the k-th largest values, the smallest pos will be reporeted
   * @param val The k-th largest value appeared in the subarray A[begin_pos ... end_pos)
   */
  void RangeTopKQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t k, uint64_t& pos, uint64_t& val) const; 

  struct ListResult{
    uint64_t c;
    uint64_t freq;
  };

  /**
   * List the distinct characters appeared in A[begin_pos ... end_pos)
   * @param begin_pos The beginning position of the array (inclusive)
   * @param end_pos The ending positin of the array (not inclusive)
   * @param num The maximum number of reporting results.
   * @param res The distinct chracters in the A[begin_pos ... end_pos) from smallest ones. 
   *            Each item consists of c:character and freq: frequency of c. 
   */
  void ListRange(uint64_t begin_pos, uint64_t end_pos, uint64_t num, std::vector<ListResult>& res) const;

  /**
   * Compute the frequency of the character c
   * @param c The character to be examined
   * param Return the frequency of c in the array.
   */
  uint64_t Freq(uint64_t c) const;

  /**
   * Return the number of alphabets in the array
   * @return The number of alphabet in the array
   */
  uint64_t alphabet_num() const;

  /**
   * Return the length of the array
   * @return The length of the array
   */
  uint64_t length() const;

  /**
   * Save the current status to a stream
   * @param os The output stream where the data is saved
   */
  void Save(std::ostream& os) const;

  /**
   * Load the current status from a stream
   * @param is The input stream where the status is saved
   */
  void Load(std::istream& is);

private:
  uint64_t GetAlphabetNum(const std::vector<uint64_t>& array) const;
  uint64_t Log2(uint64_t x) const;
  uint64_t PrefixCode(uint64_t x, uint64_t len, uint64_t total_len) const;
  uint64_t GetMSB(uint64_t x, uint64_t pos, uint64_t len) const;
  uint64_t GetLSB(uint64_t x, uint64_t pos) const;
  void SetArray(const std::vector<uint64_t>& array);
  void SetOccs(const std::vector<uint64_t>& array);
  void GetBegPoses(const std::vector<uint64_t>& array, 
		   uint64_t alpha_bit_num,
		   std::vector<std::vector<uint64_t> >& beg_poses) const;

  enum Strategy {
    MAX_QUERY = 0,
    MIN_QUERY = 1,
    KTH_QUERY = 2
  };

  void RangeQuery(uint64_t begin_pos, uint64_t end_pos, uint64_t k,
		  Strategy strategy, uint64_t& pos, uint64_t& val) const;


  bool ChooseLeftChild(uint64_t zero_num, uint64_t one_num, uint64_t k, Strategy strategy) const;

  void ListRangeRec(uint64_t st, uint64_t en,
		    uint64_t begin_pos, uint64_t end_pos, uint64_t num, 
		    uint64_t depth, uint64_t c, std::vector<ListResult>& res) const;


  std::vector<BitArray> bit_arrays_;
  BitArray occs_;

  uint64_t alphabet_num_;
  uint64_t alphabet_bit_num_;
  uint64_t length_;
};

}


#endif // WASEQ_WASEQ_HPP_
