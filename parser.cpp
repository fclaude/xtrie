
/** 
 * Builds the XTrie 
 * @author Francisco Claude
 */

#include <libxml++/libxml++.h>
#include <libxml++/parsers/textreader.h>
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
using namespace xmlpp;

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

uint fake_id = 1;
map<string, int> ids;
int max_id = 1;
NaiveTrie trie;

uint getId(const string & tag) {
  uint ret = ids[tag];
  if(ret==0) {
    ids[tag] = max_id++;
    return max_id-1;
  } else {
    return ret;
  }
}

void saveDictionary(string fname) {
  ofstream out(fname.c_str());
  assert(out.good());
  out << ids.size() << endl;
  for(map<string,int>::iterator iter = ids.begin(); iter != ids.end(); ++iter) 
    out << iter->first << " " << iter->second << endl;
  out.close();
}

class MySaxParser : public SaxParser {
  protected:
    vector<uint> path;

  public:
    MySaxParser() {}
    virtual ~MySaxParser() {}

  protected:
    virtual void on_start_document() {}
    virtual void on_end_document() {}
    virtual void on_start_element(const Glib::ustring & name, 
        const AttributeList & properties) {
      path.push_back(getId(name));
      vector<uint> rpath;
      rpath.insert(rpath.end(),path.rbegin(),path.rend());
      trie.insertPath(rpath,fake_id++);
      rpath.insert(rpath.begin(),0);
      for(AttributeList::const_iterator iter=properties.begin(); iter!=properties.end(); ++iter) {
        rpath[0] = getId(iter->name);
        trie.insertPath(rpath,fake_id++);
      }
    }
    virtual void on_end_element(const Glib::ustring & name) {
      assert(getId(name)==path[path.size()-1]);
      path.pop_back();
    }
    virtual void on_characters(const Glib::ustring & characters) {}
    virtual void on_comment(const Glib::ustring & text) {}
    virtual void on_warning(const Glib::ustring & text) {}
    virtual void on_error(const Glib::ustring & text) {}
    virtual void on_fatal_error(const Glib::ustring & text) {
      cerr << "Fatal error parsing XML file" << endl;
      exit(-1);
    }
};

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
  

  try {
    
    cout << "Parsing XML file." << endl;
    start_clock();
    ifstream xmlFile(filepath.c_str());
    char buffer[1024*1024*8];
    MySaxParser parser;

    do {
      xmlFile.read(buffer,63);
      Glib::ustring input(buffer, xmlFile.gcount());
      parser.parse_chunk(input);
    } while(xmlFile);
  
    cout << "Finished parsing XML file: " << 1000.*stop_clock() << "ms." << endl;

    cout << "Building Trie." << endl;
    start_clock();
    Trie t(&trie);
    cout << "Finished building Trie: " << 1000.*stop_clock() << "ms." << endl;

    cout << "Saving files." << endl;
    start_clock();
    ofstream out((fileout+".index").c_str(),ios::binary);
    t.save(out);
    out.close();
    saveDictionary(fileout+".dict");
    cout << "Finished saving files: " << 1000.*stop_clock() << "ms." << endl;


  } catch(const std::exception& ex) {
    cout.flush();
    cout << "Exception caught: " << ex.what() << std::endl;
  }

  return 0;
}

