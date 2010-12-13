#include <fstream>
#include <algorithm>
#include <iostream>
#include "doc_search.hpp"
#include "sais.hxx"

using namespace std;

namespace wat_array{

DocSearch::DocSearch(){
}

DocSearch::~DocSearch(){
}

void DocSearch::ReadFileNames(const char* fn){
  ifstream ifs(fn);
  if (!ifs){
    throw string("cannot open:") + fn; 
  }
  
  for (string file_name; getline(ifs, file_name); ){
    file_names.push_back(file_name);
  }
}

void DocSearch::ReadFile(const string& file_name){
  FILE* fp = fopen(file_name.c_str(), "rb");
  if (fp == NULL){
    throw string("cannot open:") + file_name;
  }

  for (int c; (c = fgetc(fp)) != EOF; ){
    text.push_back(static_cast<uint8_t>(c));
  }
  
  fclose(fp);
}

void DocSearch::ReadFiles(const char* fn){
  ReadFileNames(fn);
  for (size_t i = 0; i < file_names.size(); ++i){
    file_offsets.push_back(static_cast<uint64_t>(text.size()));
    ReadFile(file_names[i]);
    text.push_back(0);
  }
}

unsigned int DocSearch::GetDocID(const int pos) const {
  vector<int>::const_iterator it = lower_bound(file_offsets.begin(), file_offsets.end(), (uint64_t)pos);
  return it - file_offsets.begin();
}

void DocSearch::BuildDocIndex(){
  uint64_t doc_num = file_offsets.size();
  vector<int> prev_doc_pos(doc_num, 0);
  vector<uint64_t> prev_array(SA.size());
  for (uint64_t i = 0; i < SA.size(); ++i){
    int doc_id = GetDocID(SA[i]);
    prev_array[i] = prev_doc_pos[doc_id];
    prev_doc_pos[doc_id] = i;
  }
  
  wa.Init(prev_array);
}

int DocSearch::Compare(const uint32_t ind, const vector<uint8_t>& query, uint32_t& match) const {
  while (match < query.size() && match + ind < text.size()){
    if (text[ind+match] != query[match]){
      return (int)text[ind+match] - query[match];
    }
    ++match;
  }
  if (match + ind == text.size()){
    return -1;
  } else { // match == query.size()
    return 0;
  }
}


void DocSearch::Bsearch(const vector<uint8_t>& query, 
			  uint32_t& beg, uint32_t& half, uint32_t& size, 
			  uint32_t& match, uint32_t& lmatch, uint32_t& rmatch, 
			  const int state) const {
  half = size/2;
  for (; size > 0; size = half, half /= 2){
    match = min(lmatch, rmatch);
    int r = Compare(SA[beg + half], query, match);
    if (r < 0 || (r == 0 && state==2)){
      beg += half + 1;
      half -= (1 - (size & 1));
      lmatch = match;
    } else if (r > 0 || state > 0){
      rmatch = match;
    } else {
      // T[SA[beg+half]...]'s prefix equals to query
      break; 
    }
  }
}


void DocSearch::Search(const vector<uint8_t>& query) const {
  
  // Binary Search of the SA position containing a query as a prefix
  uint32_t beg    = 0;
  uint32_t size   = static_cast<uint32_t>(SA.size());
  uint32_t half   = size/2;
  uint32_t match  = 0;
  uint32_t lmatch = 0;
  uint32_t rmatch = 0;
  Bsearch(query, beg, half, size, match, lmatch, rmatch, 0);

  if (size == 0) return; // No matching found

  // Lower Bound
  uint32_t lbeg    = beg;
  uint32_t lsize   = half;
  uint32_t lhalf   = half / 2;
  uint32_t llmatch = lmatch;
  uint32_t lrmatch = match;
  uint32_t lmatch2 = 0;
  Bsearch(query, lbeg, lhalf, lsize, lmatch2, llmatch, lrmatch, 1);

  // Upper Bound
  uint32_t rbeg    = beg + half + 1;
  uint32_t rsize   = size - half - 1;
  uint32_t rhalf   = rsize / 2;
  uint32_t rlmatch = match;
  uint32_t rrmatch = rmatch;
  uint32_t rmatch2 = 0;
  Bsearch(query, rbeg, rhalf, rsize, rmatch2, rlmatch, rrmatch, 2);

  // SA[lbeg...rbeg) are matching positions;  
  vector<uint32_t> hit_docs(rbeg - lbeg);
  for (uint32_t i = lbeg; i < rbeg; ++i){
    hit_docs[i-lbeg] = GetDocID(SA[i]);
  }
  sort(hit_docs.begin(), hit_docs.end());
  hit_docs.erase(unique(hit_docs.begin(), hit_docs.end()), hit_docs.end());



  uint32_t estimate_num = wa.RankLessThan(lbeg, rbeg) - lbeg + 1;
  cout << "Hit Positions:" << rbeg - lbeg << endl;
  cout << "     Hit Docs:" << hit_docs.size() << endl; 
  cout << " RankLessThan:" << estimate_num << endl;
}


void DocSearch::Build(const char* fn){
  ReadFiles(fn);

  cout << "Read " << file_names.size() << " files." << endl
       << "Total " << text.size() << " length." << endl;
  
  SA.resize(text.size()+1);
  int n = static_cast<int>(text.size());
  saisxx(text.begin(), SA.begin(), n, 0x100);

  BuildDocIndex();
}

}
