/** Simple engine for testing the xtrie implementation
 * @author Francisco Claude
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

#include <basics.h>

#include "Trie.h"

map<string,uint> ids;
map<uint,string> rids;
uint maxTag = 0;
uint maxNode = 0;
Trie * index;
extern uint sizeIds;

uint * parse(string s, uint * len) {
  vector<uint> res;
  if(s.find("//")!=0) {
    cout << "  ** Incorrect query" << endl;
    *len = 0;
    return NULL;
  }
  s = s.substr(2);
  size_t pos = s.find("/");
  while(pos!=string::npos) {
    uint id = ids[s.substr(0,pos)];
    if(id==0) {
      cout << "  ** Node " << s.substr(0,pos) << " is not part of the document" << endl;
      *len = 0;
      return NULL;
    }
    res.push_back(id);
    s = s.substr(pos+1);
    pos = s.find("/");
  }
  uint id = ids[s];
  if(id==0) {
    cout << "  ** Node " << s.substr(0,pos) << " is not part of the document" << endl;
    *len = 0;
    return NULL;
  }
  res.push_back(id);
  *len = res.size();
  uint * ret = new uint[*len];
  for(uint i=0;i<*len;i++) {
    ret[i] = res[i];
  }
  return ret;
}

/* Time meassuring */
double ticks= (double)sysconf(_SC_CLK_TCK);
struct tms t1,t2;

void start_clock() {
  times (&t1);
}

double stop_clock() {
  times (&t2);
  return (t2.tms_utime-t1.tms_utime)/ticks;
}
/* end Time meassuring */

#define REP 1000

void answerQueries() {
  while(!cin.eof()) {
    string s;
    cout << "> ";
    cin >> s;
    cout << endl;
    if(s.length()==0) break;
    if(s=="exit" || s=="quit") break;
    if(s=="size") {
      cout << "  Index size     : " << index->getSize()/1024.0 << "Kb" << endl;
      cout << "  Ids size       : " << index->getSizeIds()/1024.0 << "Kb" << endl;
      cout << "  Labels size    : " << index->getSizeLabels()/1024.0 << "Kb" << endl;
      cout << "  Pointers size  : " << index->getSizePointers()/1024.0 << "Kb" << endl;
      cout << "  Nr of nodes    : " << sizeIds << endl;
      cout << endl;
      continue;
    }
    if(s=="help") {
      cout << "  Valid commands: " << endl;
      cout << "  - help: shows this message" << endl;
      cout << "  - //a/.../b: searches for path a/.../b" << endl;
      cout << "  - size: shows the index size" << endl;
      cout << "  - quit: finishes the program" << endl;
      cout << "  - exit: finishes the program" << endl;
      cout << endl;
      continue;
    }
    uint len, lres;
    uint * qry = parse(s,&len);
    uint * res = NULL;
    if(len>0) {
      start_clock();
      for(uint k=0;k<REP;k++) {
        res = index->getValues(qry,len,&lres);
        if(res!=NULL)
          delete [] res;
      }
      res = index->getValues(qry,len,&lres);
      double time = 1000.*stop_clock()/(REP+1);
      //cout << "  Results for " << s << endl;
      cout << "  (results: " << lres << " | time: " << time << "ms | index size: " << index->getSize()/1024.0 << "Kb)" << endl;
      delete [] qry;
    }
    cout << endl;
    if(res!=NULL)
      delete [] res;
  }
}

int main(int argc, char ** argv) {
  if(argc!=2) {
    cout << "usage: " << argv[0] << " <base name>" << endl;
    return 0;
  }
  string basename = argv[1];

  ifstream trie_in((basename+".index").c_str(),ios::binary);
  index = new Trie(trie_in);
  trie_in.close();

  ifstream input((basename+".dict").c_str());
  assert(input.good());
  input >> maxTag;
  for(uint i=0;i<maxTag;i++) {
    string tag;
    uint id;
    input >> tag >> id;
    ids[tag] = id;
    rids[id] = tag;
  }
  input.close();

  answerQueries();
  delete index;
}

