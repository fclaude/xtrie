
#ifndef _TRIE
#define _TRIE

#include <map>
#include <vector>
#include <fstream>

#include <basics.h>

using namespace std;

class Trie {
  Array * labels;
  Trie * ptrs;
  uint subTreeSize;
  Array * values;

  static int pos;

  public:
    Trie(ifstream & in);
    Trie(NaiveTrie * node);
    ~Trie();

    Array * getValues(uint * path, uint len) const;
    void getSubTreeValues();
    void save(ofstream & out) const;
    uint getSize() const;
};

#endif

