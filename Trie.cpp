
#include <basics.h>

#include "Trie.h"

using namespace std;

Trie::Trie(NaiveTrie * nt) {}

Trie::~Trie() {
}

void Trie::insertPath(const vector<uint> path, const uint value) {
  Trie * node = this;
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL) 
      node->ptrs[path[i]] = new Trie();
    node = node->ptrs[path[i]];
  }
  node->values.push_back(value);
}

vector<uint> Trie::getValues(vector<uint> path) const {
  vector<uint> ret;
  Trie * node = (Trie*)this; // We know its going to respect the const
  for(uint i=0;i<path.size();i++) {
    if(node->ptrs[path[i]] == NULL)
      node->ptrs[path[i]] = new Trie();
    node = node->ptrs[path[i]];
  }
  node->getSubTreeValues(ret);  
  return ret;
}

void Trie::getSubTreeValues(vector<uint> & res) {
  res.insert(res.end(), values.begin(), values.end());
  for(map<uint,Trie*>::iterator iter=ptrs.begin(); iter!=ptrs.end(); ++iter) {
    iter->second->getSubTreeValues(res);
  }
}

void Trie::save(ofstream & out) const {
}

uint Trie::getSize() {
}

