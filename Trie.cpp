
/** XTrie implementation
 * @author Francisco Claude <fclaude@cs.uwaterloo.ca>
 */

#include <basics.h>

#include "Trie.h"

using namespace std;

size_t Trie::pos = 0;

Trie::Trie(NaiveTrie * nt) {
  subTreeSize = (size_t)-1;
  this->values = new Array(nt->values);
  vector<uint> labs;
  for(map<uint,NaiveTrie*>::iterator iter=(nt->ptrs).begin(); iter!=(nt->ptrs).end(); ++iter)
    labs.push_back(iter->first);
  this->labels = new Array(labs);
  ptrs = new Trie*[labs.size()];
  for(size_t i=0; i<labs.size(); i++)
    ptrs[i] = new Trie(nt->ptrs[labs[i]]);
  subTreeSize = getSubTreeSize();
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
  for(uint i=len-1;i+1!=0;i--) {
    if(node->labels->getLength()>0) {
      size_t branch = node->getBranch(path[i]);
      if(path[i]==node->labels->getField(branch)) {
        node = node->ptrs[branch];
      } else {
        *rlen = 0;
        return NULL;
      }
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
  return fin;
}

size_t Trie::getSubTreeSize() const {
  if(subTreeSize!=(size_t)-1) return subTreeSize;
  size_t ret = values->getLength();
  for(size_t i=0;i<labels->getLength();i++)
    ret += ptrs[i]->getSubTreeSize();
  return ret;
}

size_t Trie::getSize() const {
  size_t ret = labels->getSize() + values->getSize();
  ret += sizeof(Trie*)*labels->getLength() + sizeof(Trie);
  for(size_t i=0; i<labels->getLength(); i++)
    ret += ptrs[i]->getSize();
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
  out.write((char*)&subTreeSize,sizeof(size_t));
  //cout << "Entering Array.save()" << endl; cout.flush();
  for(size_t i=0;i<labels->getLength();i++)
    ptrs[i]->save(out);
  //cout << "Leaving Array.save()" << endl; cout.flush();
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

