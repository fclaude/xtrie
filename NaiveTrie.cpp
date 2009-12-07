
#include <basics.h>

#include "NaiveTrie.h"

static int nr_nodes = 0;

using namespace std;

NaiveTrie::NaiveTrie() {
  cout << "Nr of nodes" << ++nr_nodes << endl;
}

NaiveTrie::~NaiveTrie() {}

void NaiveTrie::insertPath(const vector<uint> path, const uint value) {
  NaiveTrie * node = &this;
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL)
      node->ptrs[path[i]] = new NaiveTrie();
    node = node->ptrs[path[i]];
  }
  node->values.push_back(value);
}

vector<uint> NaiveTrie::getValues(vector<uint> path) const {
  vector<uint> ret;
  NaiveTrie * node = &this;
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL)
      node->ptrs[path[i]] = new NaiveTrie();
    node = node->ptrs[path[i]];
  }
  node->getSubTreeValues(ret);  
  return ret;
}

void NaiveTrie::getSubTreeValues(vector<uint> & res) const {
  res.insert(res.end(), values.begin(), values.end());
  for(map<uint,NaiveTrie*>::iterator iter=ptrs.begin(); iter!=ptrs.end(); ++iter) {
    (*iter)->getSubTreeValues(ret);
  }
}

void NaiveTrie::save(ofstream & out) const {
  
}


