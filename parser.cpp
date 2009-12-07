
/** Parser based on the libxml++ example */

#include <libxml++/libxml++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

#include <basics.h>
#include "NaiveTrie.h"
#include "Trie.h"

using namespace std;

class tnode {
  public:
    static int act_nr;

    int nr;
    int id;
    tnode * parent;

    tnode(int _id, tnode * _parent) :id(_id),parent(_parent) {
        nr = ++act_nr; 
    }

    bool less(const tnode & n) const {
      if(id<n.id) return true;
      if(id>n.id) return false;
      if(parent==n.parent) {
        if(nr<n.nr) return true;
        else return false;
      }
      if(parent==NULL) return true;
      if(n.parent==NULL) return false;
      return (*parent).less(*n.parent);
    }

    bool operator<(const tnode & n) const {
      if(parent==n.parent) {
        if(nr<n.nr) return true;
        else return false;
      }
      if(parent==NULL) return true;
      if(n.parent==NULL) return false;
      if((*parent).less(*(n.parent))) return true;
      return false;
    }
};

bool compare(const tnode * n1, const tnode * n2) {
  return (*n1)<(*n2);
}

int tnode::act_nr = 0;
vector<tnode *> nodes;
map<string, int> ids;
int max_id = 1;

string tagstr(int id) {
  for(map<string,int>::iterator iter = ids.begin(); iter != ids.end(); ++iter) {
    if(id==iter->second) return iter->first;
  }
  return "";
}

void saveDictionary(string fname) {
  ofstream out(fname.c_str());
  assert(out.good());
  out << ids.size() << endl;
  for(map<string,int>::iterator iter = ids.begin(); iter != ids.end(); ++iter) 
    out << iter->first << " " << iter->second << endl;
  out.close();
}

void saveMapping(string fname) {
  ofstream out(fname.c_str(),ios::binary);
  assert(out.good());
  uint len = nodes.size();
  uint * mapping = new uint[len];
  for(uint i=0;i<nodes.size();i++)
    mapping[i] = nodes[i]->nr;
  out.write((char*)&len,sizeof(uint));
  out.write((char*)mapping,len*sizeof(uint));
  out.close();
  delete [] mapping;
}

tnode* process_node(const xmlpp::Node* node, tnode * parent) {
  const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
  const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node);
  const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*>(node);
  string nodename = node->get_name();

  //Treat the various node types differently: 
  if(nodeText) { // text
    return NULL;
  }
  else if(nodeComment) { // comment
    return NULL;
  }
  else if(nodeContent) { // content
    return NULL;
  }

  if(ids[nodename]==0)
    ids[nodename] = max_id++;

  tnode * act_node = new tnode(ids[nodename],parent);
  nodes.push_back(act_node);
  
  if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node)) { //A normal Element node:
    //Print attributes:
    const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
    for(xmlpp::Element::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
      const xmlpp::Attribute* attribute = *iter;
      string aname = string("@")+attribute->get_name();
      if(ids[aname]==0)
        ids[aname] = max_id++;
      tnode * tmp_node = new tnode(ids[aname],act_node);
      nodes.push_back(tmp_node);
    }
  }
  
  if(!nodeContent) {
    //Recurse through child nodes:
    xmlpp::Node::NodeList list = node->get_children();
    xmlpp::Node::NodeList::iterator iter = list.begin();
    for(; iter != list.end(); ++iter) {
      process_node(*iter, act_node);
    }
  }

  return act_node;
}

vector<uint> parse(string s) {
  vector<uint> res;
  if(s.find("//")!=0) {
    cout << "  ** Incorrect query" << endl;
    return res;
  }
  s = s.substr(2);
  size_t pos = s.find("/");
  while(pos!=string::npos) {
    uint id = ids[s.substr(0,pos)];
    if(id==0) {
      cout << "  ** Node " << s.substr(0,pos) << " is not part of the document" << endl;
      return res;
    }
    res.push_back(id);
    s = s.substr(pos+1);
    pos = s.find("/");
  }
  uint id = ids[s];
  if(id==0) {
    cout << "  ** Node " << s.substr(0,pos) << " is not part of the document" << endl;
    return res;
  }
  res.push_back(id);
  vector<uint> ret;
  ret.insert(ret.end(),res.rbegin(),res.rend());
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

#define REP 100

void answerQueries(NaiveTrie & trie) {
  while(!cin.eof()) {
    string s;
    cout << "> ";
    cin >> s;
    cout << endl;
    if(s.length()==0) break;
    if(s=="exit" || s=="quit") break;
    if(s=="size") {
      cout << "  Index size: " << trie.getSize()*sizeof(uint)/1024 << "Kb" << endl;
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
    vector<uint> qry = parse(s);
    uint len=qry.size();
    vector<uint> res;
    if(len>0) {
      start_clock();
      for(uint k=0;k<REP;k++) {
        res = trie.getValues(qry);
      }
      res = trie.getValues(qry);
      double time = 1000.*stop_clock()/(REP+1);
      //cout << "  Results for " << s << endl;
      cout << "  (results: " << res.size() << " | time: " << time << "ms | size: " << sizeof(uint)*trie.getSize()/1024 << "Kb)" << endl;
    }
    cout << endl;
  }
}

int main(int argc, char* argv[])
{
  string filepath, fileout;
  if(argc == 3) {
    filepath = argv[1]; 
    fileout = argv[2];
  } else {
    cout << "usage: " << argv[0] << " <input file> <output base name>" << endl;
    return 0;
  }
  
  NaiveTrie trie;

  try {
    {
      xmlpp::DomParser *parser;
      parser = new xmlpp::DomParser;
      //parser.set_validate();
      //parser.set_substitute_entities(); 
      cout << "Parsing file." << endl;
      parser->parse_file(filepath);
      if(*parser) {
        const xmlpp::Node* pNode = parser->get_document()->get_root_node(); 
        cout << "Generating the tree." << endl;
        process_node(pNode, NULL);
      }
      delete parser;
    }

    //cout << "Saving Dictionary" << endl;
    //saveDictionary(fileout+".dict");

    //cout << "Saving Mapping" << endl;
    //saveMapping(fileout+".map");

    cout << "Building NaiveTrie." << endl;
    for(uint i=0;i<nodes.size();i++) {
      vector<uint> path;
      uint id = nodes[i]->nr;
      tnode * tmp_node = nodes[i];
      while(tmp_node!=NULL) {
        path.push_back(tmp_node->id);
        tmp_node = tmp_node->parent;
      }
      trie.insertPath(path,id);
    }
    {
      extern uint nr_nodes;
      cout << "Number of nodes in the trie: " << nr_nodes << endl;
    }

    for(uint i=0;i<nodes.size();i++)
      delete nodes[i];


  } catch(const std::exception& ex) {
    cout.flush();
    cout << "Exception caught: " << ex.what() << std::endl;
  }

  cout << "Building Trie." << endl;
  ofstream out((fileout+".index").c_str(),ios::binary);
  Trie t(&trie);
  t.save(out);
  out.close();

  return 0;
}

