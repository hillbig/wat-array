#include <iostream>
#include "doc_search.hpp"

using namespace std;

void usage(){
  cout << "indexer filelist" << endl;
}


int main(int argc, char* argv[]){
  if (argc != 2){
    usage();
    return -1;
  }

  wat_array::DocSearch doc_search;
  doc_search.Build(argv[1]);

  for (std::string query;;){
    cout << ">";
    getline(cin, query);
    vector<uint8_t> query_u8(query.begin(), query.end());
    doc_search.Search(query_u8);
  }
  

  return 0;
}
