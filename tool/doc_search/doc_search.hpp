#ifndef WAT_ARRAY_DOC_SEARCH_HPP_
#define WAT_ARRAY_DOC_SEARCH_HPP_

#include <vector>
#include <string>
#include <stdint.h>
#include <wat_array/wat_array.hpp>

namespace wat_array {

class DocSearch {
public:
  DocSearch();
  ~DocSearch();

  void Build(const char* fn);
  void Search(const std::vector<uint8_t>& query) const;

private:
  void BuildDocIndex();
  void ReadFile(const std::string& file_name);
  void ReadFiles(const char* fn);
  void ReadFileNames(const char* fn);
  unsigned int GetDocID(const int pos) const;


  int Compare(const uint32_t ind, const std::vector<uint8_t>& query, uint32_t& offset) const;
  void Bsearch(const std::vector<uint8_t>& query, 
	       uint32_t& beg, uint32_t& half, uint32_t& size, 
	       uint32_t& match, uint32_t& lmatch, uint32_t& rmatch, const int state) const ;


  
  std::vector<std::string> file_names;
  std::vector<int>     SA;
  std::vector<uint8_t> text;
  std::vector<int>     file_offsets;

  WatArray wa;
};


}

#endif // WAT_ARRAY_DOC_SEARCH_HPP_
