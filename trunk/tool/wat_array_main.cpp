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

#include <iostream>
#include <fstream>
#include <vector>
#include "../src/wat_array.hpp"
#include "cmdline.h"

using namespace std;

int ReadArrayFromFile(const std::string& file_name, vector<uint64_t>& array){
  array.clear();
  ifstream ifs(file_name.c_str());
  if (!ifs){
    cerr << "Unable to open [" << file_name << "]" << endl;
    return -1;
  }

  for (uint64_t val; ifs >> val; ){
    array.push_back(val);
  }
  return 0;
}

int BuildIndex(const string& input_file_name,
	       const string& index_name){
  vector<uint64_t> array;
  if (ReadArrayFromFile(input_file_name, array) == -1){
    return -1;
  }

  wat_array::WatArray wa;
  wa.Init(array);
  
  ofstream ofs(index_name.c_str());
  if (!ofs){
    cerr << "Unable to open [" << index_name << "]" << endl;
    return -1;
  }
  wa.Save(ofs);

  if (!ofs){
    cerr << "Save failed [" << index_name << "]" << endl;
    return -1;
  }

  return 0;
}


int main(int argc, char* argv[]){
  cmdline::parser p;
  p.add<string>("input",     'i', "input data",          true);
  p.add<string>("wat_index", 'w', "watarray index data", true);
  p.add<string>("format",    'f', "format type",         false);
  p.add        ("help",      'h', "print help");
  p.set_program_name("wat_array_cmdtool");
  bool parse_error = p.parse(argc, argv);
  if (parse_error || p.exist("help")){
    p.usage();
    return -1;
  }

  if (BuildIndex(p.get<string>("input"), p.get<string>("wat_index")) == -1){
    return -1;
  }  
  
  return 0;
}
