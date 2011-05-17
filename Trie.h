
/** XTrie definition
 * @author Francisco Claude <fclaude@cs.uwaterloo.ca>
 */

#ifndef _TRIE
#define _TRIE

#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#include <basics.h>
#include <Array.h>

#include "NaiveTrie.h"

using namespace std;


class Trie {
  Array * labels;
  Trie ** ptrs;
  size_t subTreeSize;
  Array * values;

  static size_t pos;

  size_t getBranch(uint v) const;
  size_t getSubTreeSize() const;
  void fillValues(uint * buff) const;

  public:
    Trie(ifstream & in);
    Trie(NaiveTrie * node);
    ~Trie();

    uint * getValues(uint * path, uint len, uint * rlen) const;
    void save(ofstream & out) const;
    size_t getSize() const;
    size_t getSizePointers() const;
    size_t getSizeIds() const;
    size_t getSizeLabels() const;
};

#endif

