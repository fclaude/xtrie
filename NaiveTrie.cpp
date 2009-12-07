
#include <basics.h>

#include "NaiveTrie.h"

int nr_nodes = 0;

using namespace std;

NaiveTrie::NaiveTrie() {
  //cout << "Nr of nodes" << ++nr_nodes << endl;
  ++nr_nodes;
}

NaiveTrie::~NaiveTrie() {
  for(map<uint,NaiveTrie*>::iterator iter=ptrs.begin(); iter!=ptrs.end(); ++iter) 
    delete iter->second;
}

void NaiveTrie::insertPath(const vector<uint> path, const uint value) {
  NaiveTrie * node = this;
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL) 
      node->ptrs[path[i]] = new NaiveTrie();
    node = node->ptrs[path[i]];
  }
  node->values.push_back(value);
}

vector<uint> NaiveTrie::getValues(vector<uint> path) const {
  vector<uint> ret;
  NaiveTrie * node = (NaiveTrie*)this; // We know its going to respect the const
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL)
      node->ptrs[path[i]] = new NaiveTrie();
    node = node->ptrs[path[i]];
  }
  node->getSubTreeValues(ret);  
  return ret;
}

void NaiveTrie::getSubTreeValues(vector<uint> & res) {
  res.insert(res.end(), values.begin(), values.end());
  for(map<uint,NaiveTrie*>::iterator iter=ptrs.begin(); iter!=ptrs.end(); ++iter) {
    iter->second->getSubTreeValues(res);
  }
}

void NaiveTrie::save(ofstream & out) const {
  
}

size_t NaiveTrie::getSize() {
  size_t integers = 0;
  map<uint,NaiveTrie*>::iterator iter=ptrs.begin();
  for(; iter!=ptrs.end(); ++iter) {
    integers += iter->second->getSize();
    integers += 4;
  }
  integers += values.size();
  return integers;
}

