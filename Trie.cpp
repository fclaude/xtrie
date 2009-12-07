
#include <basics.h>

#include "Trie.h"

using namespace std;

size_t Trie::pos = 0;

Trie::Trie(NaiveTrie * nt) {
  subTreeSize = (uint)-1;
  values = new Array(nt->values);
  vector<uint> values;
  vector<NaiveTrie *> pointers;
  for(map<uint,NaiveTrie*>::iterator iter=(nt->ptrs).begin(); iter!=(nt->ptrs).end(); ++iter) {
    values.push_back(iter->first);
    pointers.push_back(iter->second);
  }
  this->values = new Array(values);
  ptrs = new Trie*[values.size()];
  for(size_t i=0; i<values.size(); i++)
    ptrs[i] = new Trie(pointers[i]);
}

Trie::~Trie() {
  delete labels;
  for(size_t i=0;i<values->getLength();i++)
    delete ptrs[i];
  delete [] ptrs;
  delete values;
}

uint * Trie::getValues(uint * path, uint len, uint * rlen) const {
  Trie * node = (Trie*)this; // We know its going to respect the const
  for(uint i=0;i<len;i++) {
    size_t branch = getBranch(path[i]);
    if(path[i]==labels->getField(branch)) {
      node = node->ptrs[branch];
    } else {
      *rlen = 0;
      return NULL;
    }
  }
  Trie::pos = 0;
  *rlen = node->getSubTreeSize();
  uint * ret = new uint[*rlen];
  node->fillValues(ret);
  return ret;
}

size_t Trie::getBranch(uint v) const {
  size_t ini = 0;
  size_t fin = labels->getLength()-1;
  while(ini<fin) {
    size_t med = (ini+fin)/2;
    if(labels->getField(med)<v) {
      ini = med+1;
    } else {
      fin = med;
    }
  }
  return labels->getField(fin);
}

size_t Trie::getSubTreeSize() const {
  size_t ret = values->getLength();
  for(size_t i=0;i<labels->getLength();i++)
    ret += ptrs[i]->getSubTreeSize();
  return ret;
}

size_t Trie::getSize() const {
  size_t ret = labels->getSize() + values->getSize();
  ret += sizeof(Trie*)*labels->getLength() + sizeof(Trie);
  return ret;
}

void Trie::fillValues(uint * buff) const {
  for(size_t i=0;i<values->getLength();i++)
    buff[Trie::pos++] = values->getField(i);
  for(size_t i=0;i<labels->getLength();i++)
    ptrs[i]->fillValues(buff);
}

void Trie::save(ofstream & out) const {
  assert(out.good());
  labels->save(out);
  values->save(out);
  out.write((char*)subTreeSize,sizeof(size_t));
  for(size_t i=0;i<labels->getLength();i++)
    ptrs[i]->save(out);
}

Trie::Trie(ifstream & in) {
  assert(in.good());
  labels = new Array(in);
  values = new Array(in);
  in.read((char*)&subTreeSize,sizeof(size_t));
  ptrs = new Trie*[labels->getLength()];
  for(size_t i=0;i<labels->getLength();i++)
    ptrs[i] = new Trie(in);
}

