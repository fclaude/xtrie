
#ifndef _NAIVE_TRIE
#define _NAIVE_TRIE

#include <map>
#include <vector>
#include <fstream>

#include <basics.h>

using namespace std;

class NaiveTrie {
  map<uint, NaiveTrie*> ptrs;
  vector<uint> values;

  public:
    NaiveTrie();
    ~NaiveTrie();

    void insertPath(const vector<uint> path, const uint value);
    vector<uint> getValues(vector<uint> path) const;
    void getSubTreeValues(vector<uint> & ret);
    void save(ofstream & out) const;
};

#endif

