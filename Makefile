
CPP=g++
LXMLFLAGS=-I/usr/include/libxml++-2.6 -I/usr/lib/libxml++-2.6/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lglib-2.0

LCDSFLAGS=-I./libcds/includes/ 
LCDSLIB=./libcds/lib/libcds.a 

CPPFLAGS= -g3 -Wall 
CPPFLAGS2= -g3 -Wall 
#CPPFLAGS= -O3 -Wall -m32
#CPPFLAGS2= -O3 -Wall 

all:
	@echo " [MSG] Building libcds"
	@make --no-print-directory -C libcds
	@echo " [C++] Compiling gen_xml.cpp"
	@gcc -w -c gen_xml.c
	@echo " [C++] Compiling NaiveTrie.cpp"
	@$(CPP) $(CPPFLAGS2) $(LCDSFLAGS) -c NaiveTrie.cpp
	@echo " [C++] Compiling Trie.cpp"
	@$(CPP) $(CPPFLAGS2) $(LCDSFLAGS) -c Trie.cpp
	@echo " [C++] Compiling parser.cpp"
	@$(CPP) $(CPPFLAGS2) $(LXMLFLAGS) $(LCDSFLAGS) -c parser.cpp
	@echo " [C++] Compiling engine.cpp"
	@$(CPP) $(CPPFLAGS2) $(LXMLFLAGS) $(LCDSFLAGS) -c engine.cpp
	@echo " [C++] Compiling gen_xml.c"
	@gcc -w -c gen_xml.c 
	@echo " [C++] Building parser"
	@$(CPP) $(CPPFLAGS2) $(LXMLFLAGS) $(LCDSFLAGS) -o parser parser.o NaiveTrie.o Trie.o $(LCDSLIB) 
	@echo " [C++] Building engine"
	@$(CPP) $(CPPFLAGS2) $(LXMLFLAGS) $(LCDSFLAGS) -o engine engine.o NaiveTrie.o Trie.o $(LCDSLIB) 
	@echo " [C++] Building gen_xml"
	@gcc -o gen_xml gen_xml.o

clean:
	@make --no-print-directory -C libcds clean
	@echo " [CLN] Cleaning binaries"
	@rm -f parser gen_xml
	@rm -f *.o
