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

#include <gtest/gtest.h>
#include <sstream>
#include "../src/wat_array.hpp"

using namespace std;

TEST(wat_array, trivial){
  wat_array::WatArray ws;
  ASSERT_EQ(0, ws.alphabet_num());
  ASSERT_EQ(0, ws.length());
  ASSERT_EQ(wat_array::NOTFOUND, ws.Rank(0, 0));
  ASSERT_EQ(wat_array::NOTFOUND, ws.Select(0, 0));
  ASSERT_EQ(wat_array::NOTFOUND, ws.Lookup(0));

  ostringstream oss;
  ws.Save(oss);
  ASSERT_EQ(false, !oss);
  
  istringstream iss(oss.str());
  wat_array::WatArray ws_load;
  ws_load.Load(iss);
  ASSERT_EQ(false, !iss);
  
  ASSERT_EQ(0, ws_load.alphabet_num());
  ASSERT_EQ(0, ws_load.length());
  ASSERT_EQ(wat_array::NOTFOUND, ws_load.Rank(0, 0));
  ASSERT_EQ(wat_array::NOTFOUND, ws_load.Select(0, 0));
  ASSERT_EQ(wat_array::NOTFOUND, ws_load.Lookup(0));
}

TEST(wat_array, small){
  wat_array::WatArray ws;
  vector<uint64_t> array;
  const uint64_t alphabet_num = 4;
  for (uint64_t i = 0; i < alphabet_num; ++i){
    array.push_back(i);
  }

  ws.Init(array);

  ASSERT_EQ(alphabet_num, ws.alphabet_num());
  ASSERT_EQ(4, ws.length());
  for (uint64_t i = 0; i < alphabet_num; ++i){
    ASSERT_EQ(1, ws.Freq(i)) << " " << i << " " << alphabet_num << endl;
  }
  vector<uint64_t> counts(alphabet_num);
  for (uint64_t i = 0; i < array.size(); ++i){
    uint64_t c = array[i];
    ASSERT_EQ(c, ws.Lookup(i));
    uint64_t sum = 0;
    for (uint64_t j = 0; j < alphabet_num; ++j){
      ASSERT_EQ(counts[j], ws.Rank(j, i));
      ASSERT_EQ(sum, ws.RankLessThan(j, i)) << j << " " << i << endl;
      sum += counts[j];
    }
    counts[c]++;
    ASSERT_EQ(i, ws.Select(c, counts[c])) << " Select(c:" << c << " ind:" << counts[c] << ")" << endl ;
  }
}

TEST(wat_array, random){
  wat_array::WatArray ws;
  vector<uint64_t> array;

  uint64_t alphabet_num = 100;
  uint64_t n = 10000;
  vector<uint64_t> freq(alphabet_num);
  for (uint64_t i = 0; i < n; ++i){
    uint64_t c = rand() % alphabet_num;
    array.push_back(c);
    freq[c]++;
  }
  ws.Init(array);

  ASSERT_EQ(alphabet_num, ws.alphabet_num());
  ASSERT_EQ(n, ws.length());
  for (uint64_t i = 0; i < alphabet_num; ++i){
    ASSERT_EQ(freq[i], ws.Freq(i));
  }

  vector<uint64_t> counts(alphabet_num);
  for (uint64_t i = 0; i < array.size(); ++i){
    uint64_t c = array[i];

    ASSERT_EQ(c, ws.Lookup(i));
    uint64_t sum = 0;
    for (uint64_t j = 0; j < alphabet_num; ++j){
      if ((rand() % 100) == 0){
	ASSERT_EQ(counts[j], ws.Rank(j, i)) << " Rank(" << j << "," << i << ")";
	ASSERT_EQ(sum, ws.RankLessThan(j, i)) << "RankLessThan(" << j << "," << ")";
      }
      sum += counts[j];
    }
    counts[c]++;
    ASSERT_EQ(i, ws.Select(c, counts[c])) << " c:" << c << " counts[c]:" << counts[c] << endl;
  }
}

