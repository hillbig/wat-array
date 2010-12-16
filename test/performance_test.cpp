#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include "../src/wat_array.hpp"

using namespace std;

struct RandomQuery{
  RandomQuery(int n){
    for (;;){
      beg = rand() % n;
      end = rand() % (n+1);
      if (beg != end) break;
    }
    if (beg > end) swap(beg, end);
  }
  uint64_t beg;
  uint64_t end;
};

double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

void TestTime(uint64_t length, uint64_t alphabet_num){
  vector<uint64_t> array(length);
  vector<uint64_t> freqs(alphabet_num); 
  for (uint64_t i = 0; i < length; ++i){
    array[i] = rand() % alphabet_num;
    freqs[array[i]]++;
  }
  
  vector<uint64_t> non_zeros;
  for (size_t i = 0; i < freqs.size(); ++i){
    if (freqs[i] > 0) non_zeros.push_back(i);
  }
  uint64_t non_zero_num = non_zeros.size();

  wat_array::WatArray ws;
  double begin_time = 0.0;

  begin_time = gettimeofday_sec();
  ws.Init(array);
  double init_time = gettimeofday_sec() - begin_time;
  
  const int N = 1000;
  uint64_t dummy = 0;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    dummy += ws.Lookup(rand() % (length+1));
  }
  double lookup_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    dummy += ws.Rank(rand() % alphabet_num, rand() % (length+1));
  }
  double rank_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    uint64_t ind = rand() % non_zero_num;
    uint64_t c = non_zeros[ind]; 
    dummy += ws.Select(c, (rand() % freqs[c]) + 1);
  }
  double select_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    uint64_t pos = 0;
    uint64_t val = 0;
    ws.MaxRange(rq.beg, rq.end, pos, val);
    dummy += pos;
  }
  double max_range_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    uint64_t pos = 0;
    uint64_t val = 0;
    ws.QuantileRange(rq.beg, rq.end, (rq.end-rq.beg)/2, pos, val);
    dummy += pos;
  }
  double quantile_range_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    RandomQuery arq(alphabet_num);
    dummy += ws.FreqRange(arq.beg, arq.end, rq.beg, rq.end);
  }
  double freq_range_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    RandomQuery arq(alphabet_num);
    vector<wat_array::ListResult> lrs;
    ws.ListModeRange(arq.beg, arq.end, rq.beg, rq.end, 1, lrs);
    dummy += lrs[0].freq;
  }
  double list_mode_range_one_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    RandomQuery arq(alphabet_num);
    vector<wat_array::ListResult> lrs;
    ws.ListMaxRange(arq.beg, arq.end, rq.beg, rq.end, 1, lrs);
    dummy += lrs[0].freq;
  }
  double list_max_range_one_time = gettimeofday_sec() - begin_time;


  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    RandomQuery arq(alphabet_num);
    vector<wat_array::ListResult> lrs;
    ws.ListModeRange(arq.beg, arq.end, rq.beg, rq.end, 10, lrs);
    dummy += lrs[0].freq;
  }
  double list_mode_range_ten_time = gettimeofday_sec() - begin_time;

  begin_time = gettimeofday_sec();
  for (int i = 0; i < N; ++i){
    RandomQuery rq(length);
    RandomQuery arq(alphabet_num);
    vector<wat_array::ListResult> lrs;
    ws.ListMaxRange(arq.beg, arq.end, rq.beg, rq.end, 10, lrs);
    dummy += lrs[0].freq;
  }
  double list_max_range_ten_time = gettimeofday_sec() - begin_time;


  cerr << length << "\t"
       << alphabet_num << "\t"
       << init_time << "\t"
       << lookup_time << "\t"
       << rank_time << "\t"
       << select_time << "\t"
       << max_range_time << "\t"
       << quantile_range_time << "\t" 
       << freq_range_time << "\t"
       << list_max_range_one_time << "\t"
       << list_max_range_ten_time << "\t"
       << list_mode_range_one_time << "\t"
       << list_mode_range_ten_time << endl;

}

int main(int argc, char* argv[]){
  for (uint64_t length = 100000; length <= 100000000; length *= 10){
    for (uint64_t alpha_num = 10; alpha_num <= length; alpha_num *= 10){
      TestTime(length, alpha_num);
    }
  }
      
  return 0;
}
