#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/tokenizer.hpp>
#include "AVLTree.h"
#include <windows.h>
#include <commdlg.h>  
#include <locale>
#include "OperationQueue.h"
#include "BindsContainer.h"
#include "CvarContainer.h"
#include "Helpers.h"

using namespace std;
//function declarations
char* OpenFile(const char* title);
void parseScript(const std::string filename);
bool findvstr(std::string s);
bool is_valid_cvar(std::string line);
bool has_string(std::string s);


bool defaultdone = false;
bool showunused = true;
bool showcomments = true;
bool compressedspacing = true;
AVLTree<std::string> defaultval; //  seta cg_fov "100"
AVLTree<std::string> defaultcvar;//  cg_fov
AVLTree<std::string> boundvstr;  //  scriptname
vector<string> nonstandard;      //deprecated cvar line
vector<string> nonstandardcvar;  //deprecated cvar name
vector<string> aliases;          //alias definitions
vector<string> validvstr;        //script line
vector<string> removable;
vector<string> exec;
vector<tuple<string,string>> binds;  //bind x "something"
vector<tuple<string,string>> cvars;
vector<BindsContainer*> setbinds;
vector<CvarContainer*> setcvars;
std::string cvar;
std::string param;
std::string fullstring;
std::string vstrtoken;
OPENFILENAME ofn;          

//open windows file browser
char* OpenFile(const char* title) {
    if ( ofn.lpstrFile       ) { delete [] ofn.lpstrFile; }
    if ( ofn.lpstrInitialDir ) { delete [] ofn.lpstrInitialDir; }
    memset((void*)&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.Flags       |= OFN_NOVALIDATE;          // prevent disabling of front slashes
    ofn.Flags       |= OFN_HIDEREADONLY;        // hide readonly flag
    ofn.Flags       |= OFN_EXPLORER;            // use 'newer' explorer windows
    ofn.Flags       |= OFN_ENABLESIZING;        // allow window to be resized
    ofn.Flags       |= OFN_NOCHANGEDIR;         // prevent dialog for messing up the cwd
    ofn.nMaxFile     = 4096-1;
    ofn.lpstrFile    = new char[4096];
    ofn.lpstrFile[0] = 0;
    ofn.hwndOwner    = GetForegroundWindow();
    ofn.lpstrTitle   = title;
    int err = GetOpenFileName(&ofn);
    if ( err == 0 ) {
        err = CommDlgExtendedError();           // extended Error check
        if ( err == 0 ) return 0;                 // user hit 'cancel'
        fprintf(stderr, "CommDlgExtendedError() code=%d", err);
        return 0;
    }
	char *buf;
	size_t sz;
	sz = _snprintf(NULL, 0, "%s", ofn.lpstrFile);
	buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
	if(buf == NULL) return 0;
	_snprintf(buf, sz+1, "%s", ofn.lpstrFile);
	return buf;
}
//.script file parser
void parseScript(const string filename){
	ifstream input(filename);
	string next;
	if(input.fail()){
		std::cout << filename << ": file not found" << endl;
	}else{
		OperationQueue operQueue(filename);
		int linenumber = 0;
		while(!operQueue.isEmpty()){
			++linenumber;
			string opName;
			string parameter0;
			string parameter1;
			Operation op = operQueue.pop();
			opName = op.getName();
			if(!opName.empty()){
				if(!boost::starts_with(opName,"//")){
					boost::to_upper(opName);
					if(opName.compare("BINDSECTION") == 0){
						if(op.parameterCount() == 2){
							parameter0 = op.getParameter(0);
							parameter1 = op.getParameter(1);
							parameter0.erase(remove( parameter0.begin(), parameter0.end(), '\'' ), parameter0.end() );
							parameter1.erase(remove( parameter1.begin(), parameter1.end(), '\'' ), parameter1.end() );
						}else{
							cout << "Invalid parameters in: " << op.getName() << " on line " << linenumber << endl;
							system("PAUSE");
							exit(1);
							return;
						}
						setbinds.push_back(new BindsContainer(parameter0, parameter1));
					}
					if(opName.compare("ADDBIND") == 0){
						if(op.parameterCount() == 2){
							parameter0 = op.getParameter(0);
							parameter1 = op.getParameter(1);
							parameter0.erase(remove( parameter0.begin(), parameter0.end(), '\'' ), parameter0.end() );
							parameter1.erase(remove( parameter1.begin(), parameter1.end(), '\'' ), parameter1.end() );
						}else{
							cout << "Invalid parameters in: " << op.getName() << " on line " << linenumber << endl;
							system("PAUSE");
							exit(1);
							return;
						}
						for(std::vector<BindsContainer*>::size_type i = 0; i != setbinds.size(); i++) {
							if(ci_find_substr( setbinds[i]->getVarName(),parameter0) != -1){
								setbinds[i]->addFind(parameter1);
							}
						}
					}
					if(opName.compare("CVARSECTION") == 0){
						if(op.parameterCount() == 2){
							parameter0 = op.getParameter(0);
							parameter1 = op.getParameter(1);
							parameter0.erase(remove( parameter0.begin(), parameter0.end(), '\'' ), parameter0.end() );
							parameter1.erase(remove( parameter1.begin(), parameter1.end(), '\'' ), parameter1.end() );
						}else{
							cout << "Invalid parameters in: " << op.getName() << " on line " << linenumber << endl;
							system("PAUSE");
							exit(1);
							return;
						}
						setcvars.push_back(new CvarContainer(parameter0, parameter1));
					}
					if(opName.compare("ADDCVAR") == 0){
						if(op.parameterCount() == 2){
							parameter0 = op.getParameter(0);
							parameter1 = op.getParameter(1);
							parameter0.erase(remove( parameter0.begin(), parameter0.end(), '\'' ), parameter0.end() );
							parameter1.erase(remove( parameter1.begin(), parameter1.end(), '\'' ), parameter1.end() );
						}else{
							cout << "Invalid parameters in: " << op.getName() << " on line " << linenumber << endl;
							system("PAUSE");
							exit(1);
							return;
						}
						for(std::vector<CvarContainer*>::size_type i = 0; i != setcvars.size(); i++) {
							if(ci_find_substr( setcvars[i]->getVarName(),parameter0) != -1){
								setcvars[i]->addFindCvar(parameter1);
							}
						}
					}
					if(opName.compare("SHOWUNUSEDSTUFF") == 0){
						if(op.parameterCount() == 1){
							parameter0 = op.getParameter(0);
							std::string checktrue = "true";
							std::string checkfalse = "false";
							if(ci_find_substr(parameter0,checktrue) != -1){
								showunused = true;
							}else if(ci_find_substr(parameter0,checkfalse) != -1){
								showunused = false;
							}else{
								cout << "Invalid parameter in: " << op.getName() << " on line " << linenumber << endl;
								system("PAUSE");
							}
						}
					}
					if(opName.compare("SHOWCOMMENTS") == 0){
						if(op.parameterCount() == 2){
							parameter0 = op.getParameter(0);
							parameter1 = op.getParameter(1);
							std::string checktrue = "true";
							std::string checkfalse = "false";
							std::string originalspacing = "original";
							std::string singlespacing = "single";
							if(ci_find_substr(parameter0,checktrue) != -1){
								showcomments = true;
							}else if(ci_find_substr(parameter0,checkfalse) != -1){
								showcomments = false;
							}else{
								cout << "Invalid parameter in: " << op.getName() << " on line " << linenumber << endl;
								system("PAUSE");
							}
							if(ci_find_substr(parameter1,originalspacing) != -1){
								compressedspacing = false;
							}else if(ci_find_substr(parameter1,singlespacing) != -1){
								compressedspacing = true;
							}else{
								cout << "Invalid parameter in: " << op.getName() << " on line " << linenumber << endl;
								system("PAUSE");
							}
						}else if(op.parameterCount() == 1){
							parameter0 = op.getParameter(0);
							
							std::string checktrue = "true";
							std::string checkfalse = "false";
							if(ci_find_substr(parameter0,checktrue) != -1){
								showcomments = true;
							}else if(ci_find_substr(parameter0,checkfalse) != -1){
								showcomments = false;
							}else{
								cout << "Invalid parameter in: " << op.getName() << " on line " << linenumber << endl;
								system("PAUSE");
							}
						}
					}
				}
			}
		}
	}
}
//find vstr within lines
bool findvstr(string s){
	boost::regex exp("(?:.*?)vstr\\s+([a-z0-9_]*)",boost::regex::icase);
	boost::match_results<std::string::const_iterator> what;
	std::string::const_iterator start = s.begin() ;
	std::string::const_iterator end = s.end() ;
	while ( boost::regex_search(start, end, what, exp) )
        {
            start = what[0].second ;
			std::vector<string>::iterator it1 = std::find(nonstandardcvar.begin(),nonstandardcvar.end(),what[1]);
			if(it1!=nonstandardcvar.end()){
				vstrtoken = what[1];
				std::vector<string>::iterator it = std::find_if (nonstandard.begin(), nonstandard.end(), has_string);
				string something = *it;
				//cout << something << endl;
				it1 = nonstandardcvar.erase(it1);
				it = nonstandard.erase(it);
				findvstr(something);
				validvstr.push_back(something);
			}
			if(!boundvstr.search(what[1])){ 
				boundvstr.insert(what[1]);
				cout << what[1] << endl;
				system("PAUSE");
			}
        }
	//
	return false;
}
//find vstr in use by binds, exec, alias
bool getVstrInUse(std::string line){
	if(boost::contains(line,"vstr")){
			typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
			boost::char_separator<char> sep(" \";");
			tokenizer tokens(line, sep);
			for (tokenizer::iterator tok_iter = tokens.begin();	tok_iter != tokens.end(); ++tok_iter){
				string token = *tok_iter;
				if(token.compare("vstr")==0){
					++tok_iter;
					//get word after "vstr"
					token = *tok_iter;
					//if its not in the AVLTree, insert it
					if(!boundvstr.search(token)){ 
						boundvstr.insert(token);
						cout << token << endl;
						system("PAUSE");
					}
				}
			}
			
		}
	return true;
}
//check if valid cvar
bool is_valid_cvar(std::string line){
	//check for exec as 1st word
	if(boost::istarts_with(line,"exec",std::locale())){
		//look for vstr within the line
		getVstrInUse(line);
		exec.push_back(line);
		return false;
	}

	boost::regex e("(?:.*?)((?:[a-z_][a-z0-9_]*).*?([a-z_][a-z0-9_]*).*?(\".*?\"))(.*?)",boost::regex::icase); //icase = case insensitive
	boost::match_results<std::string::const_iterator> what;
	if(0 == boost::regex_match(line, what, e,boost::match_default | boost::match_partial)){
      return false;
	}
	if(what[0].matched){
		fullstring = what[1];
		cvar = what[2];
		param = what[3];
		string comment = what[4];
		if(compressedspacing){
			comment = single_space(comment);
		}
		
		fullstring = single_space(fullstring);

		//find removable substrings, if found, go to next line 
		string findthis;
		for(std::vector<string>::iterator it = removable.begin(); it != removable.end(); ++it) {
			findthis = *it;
			if(ci_find_substr( fullstring, findthis ) != -1){
				return false;
			}
		}
		boost::to_lower(param);
		boost::to_lower(cvar);

		if(!defaultdone){ //default cfg
			if(!boost::istarts_with(fullstring,"bind",std::locale())){
				defaultcvar.insert(cvar);
				defaultval.insert(fullstring);
				return true; //dont store default binds
			}else{
				return false;
			}
		}else{ //customized cfg
			
			if(boost::istarts_with(fullstring,"alias",std::locale())){
				getVstrInUse(param);
				if(showcomments){
					aliases.push_back(fullstring+comment);
				}else{
					aliases.push_back(fullstring);
				}
				
				return false;
			}
			//regex match starts with BIND in nondefault cfg, store
			if(boost::istarts_with(fullstring,"bind",std::locale())){
				getVstrInUse(param);
				for(std::vector<BindsContainer*>::size_type i = 0; i != setbinds.size(); i++) {
					if(setbinds[i]->findBind(fullstring,param)){
						return false;
					}
				}
				if(showcomments){
					binds.push_back(make_tuple(fullstring+comment,param));
				}else{
					binds.push_back(make_tuple(fullstring,param));
				}
				return false;
			}
			//it is not default cvar
			if(!defaultcvar.search(cvar)){
				if(boundvstr.search(cvar)){
					findvstr(param);
					if(showcomments){
						validvstr.push_back(fullstring+comment);
					}else{
						validvstr.push_back(fullstring);
					}
					
					return false;
				}else{
					nonstandardcvar.push_back(cvar);
					if(showcomments){
						nonstandard.push_back(fullstring+comment);
					}else{
						nonstandard.push_back(fullstring);
					}
					
					return false;
				}
			}
			//it is not default value
			if(!defaultval.search(fullstring)){
				for(std::vector<CvarContainer*>::size_type i = 0; i != setcvars.size(); i++) {
					if(setcvars[i]->findCvar(fullstring,param)){
						return false;
					}
				}
				if(showcomments){
					cvars.push_back(make_tuple(fullstring+comment,param));
				}else{
					cvars.push_back(make_tuple(fullstring,param));
				}
			}else{
				return false; 
			}
		}
		return true;
	}else{
	   return false;
	}
}


bool has_string(std::string s){
	return boost::starts_with(s,"seta "+vstrtoken)||boost::starts_with(s,"set "+vstrtoken);
}

void initFiles(int argc, char** argv, vector<std::string>& filenames, ifstream* inputfiles){
	vector<std::string> openTitle;
	
	openTitle.push_back("Open .script file");
	openTitle.push_back("Open remove.txt");
	openTitle.push_back("Open file containing default cvars");
	openTitle.push_back("Open custom config file");
	filenames.push_back("layout.script");	//0
	filenames.push_back("remove.txt");		//1
	filenames.push_back("cvars.txt");		//2
	filenames.push_back("custom.cfg");		//3
	filenames.push_back("output.cfg");

	if(argc == 1){
		//get all filenames through OS if not default
		for(int i = 0; i<4; i++){
			inputfiles[i].open(filenames[i].c_str());
			if(inputfiles[i].fail()){
				std::cout << "Warning: "<< filenames[i] << " not found" << endl;
				filenames[i] = OpenFile(openTitle[i].c_str());
			}
			inputfiles[i].close();
		}
	}else if(argc == 2){
		//get filename for cfg from argv
		filenames[0] = "layout.script";
		filenames[1] = "remove.txt";
		filenames[2] = "cvars.txt";
		filenames[3] = argv[1];		
	}else{
		std::cout << "Syntax is ConfigCleaner.exe <layout.script> <remove.txt> <cvars.txt> <custom.cfg> <outputfile.cfg>\n";
	}
	if(filenames[0].empty() || filenames[1].empty() || filenames[2].empty() || filenames[3].empty()){
		exit(1);
	}
}

void parseInput(std::vector<std::string>& filenames, std::ifstream* inputfiles){
	parseScript(filenames[0]);
	inputfiles[1].open(filenames[1].c_str());
	if(inputfiles[1].fail()){
		std::cout << "Error: " << filenames[1] << " not found" << endl;
		system("PAUSE");
	}else{
		string line;
		while(getline(inputfiles[1],line)){
			removable.push_back(line);
		}
		inputfiles[1].close();
	}
	inputfiles[2].open(filenames[2].c_str());
	if(inputfiles[2].fail()){
		std::cout << "Error: " << filenames[2] << " not found" << endl;
		system("PAUSE");
	}else{
		string line;
		while(getline(inputfiles[2],line)){
			is_valid_cvar(line);
		}
		inputfiles[2].close();
		std::cout << "SUCCESS: Parsed " << filenames[2] << endl;
		defaultdone = true;
		inputfiles[3].open(filenames[3].c_str());
		if(inputfiles[3].fail()){
			std::cout << "Error: "<< filenames[3] << " not found" << endl;
			system("PAUSE");
		}else{
			string line;
			while(getline(inputfiles[3],line)){
				is_valid_cvar(line);
			}
			inputfiles[3].close();
			std::cout << "SUCCESS: Parsed custom.cfg" << endl;
		}
	}
}
void generateOutput(vector<std::string>& filenames){
	ofstream output;
	output.open(filenames[4]);
	output << "//Generated from " << filenames[3] << "\n";
	if(!exec.empty()){
		std::sort(exec.begin(),exec.end());
		std::reverse(exec.begin(),exec.end());
		output << "//  ____\n" 
				<< "// (EXEC)\n" 
				<< "//  ¯¯¯¯\n";
		while(!exec.empty()){
			output << exec.back() << "\n";
			exec.pop_back();
		}
	}
	if(!aliases.empty()){
		std::sort(aliases.begin(),aliases.end());
		std::reverse(aliases.begin(),aliases.end());
		output << "//  _______\n" 
				<< "// (ALIASES)\n" 
				<< "//  ¯¯¯¯¯¯¯\n";
		while(!aliases.empty()){
			output << aliases.back() << "\n";
			aliases.pop_back();
		}
	}
	bool theresbinds = false;
	for(std::vector<BindsContainer*>::size_type i = 0; i != setbinds.size(); i++) {
		if(setbinds[i]->getContainerSize() != 0){
			theresbinds = true;
		}
	}
	if(theresbinds || !binds.empty()){
	output  << "//  _____\n" 
			<< "// (BINDS)\n" 
			<< "//  ¯¯¯¯¯\n";
	}

	for(std::vector<BindsContainer*>::size_type i = 0; i != setbinds.size(); i++) {
		if(setbinds[i]->getContainerSize() != 0){

			output << "//    ";
			for(int x = 0; x!= setbinds[i]->getName().size();x++){
				output << "_";
			}
			output << "\n" 
					<< "//   (" << setbinds[i]->getName() << ")\n" 
					<< "//    ";
			for(int x = 0; x!= setbinds[i]->getName().size();x++){
				output << "¯";
			}
			output << "\n";
			setbinds[i]->sortContainer();
			for(int j = 0; j != setbinds[i]->getContainerSize();j++){
				output << setbinds[i]->getBinds(j) << "\n";
			}
		}
	}
	if(!binds.empty()){
		std::sort(binds.begin(),binds.end(),sortbysecond);
		std::reverse(binds.begin(),binds.end());
		output  << "//    _____________\n" 
				<< "//   (REST OF BINDS)\n" 
				<< "//    ¯¯¯¯¯¯¯¯¯¯¯¯¯\n";
		while(!binds.empty()){
			output << std::get<0>(binds.back()) << "\n";
			binds.pop_back();
		}
	}
	bool therescvars = false;
	for(std::vector<CvarContainer*>::size_type i = 0; i != setcvars.size(); i++) {
		if(setcvars[i]->getContainerSize() != 0){
			therescvars = true;
		}
	}
	if(therescvars || !cvars.empty()){
	output << "//  _________________\n" 
			<< "// (NON-DEFAULT CVARS)\n" 
			<< "//  ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n";
	}
	for(std::vector<CvarContainer*>::size_type i = 0; i != setcvars.size(); i++) {
		if(setcvars[i]->getContainerSize() != 0){

			output << "//    ";
			for(int x = 0; x!= setcvars[i]->getName().size();x++){
				output << "_";
			}
			output << "\n" 
					<< "//   (" << setcvars[i]->getName() << ")\n" 
					<< "//    ";
			for(int x = 0; x!= setcvars[i]->getName().size();x++){
				output << "¯";
			}
			output << "\n";
			setcvars[i]->sortContainer();
			for(int j = 0; j != setcvars[i]->getContainerSize();j++){
				output << setcvars[i]->getCvar(j) << "\n";
			}
		}
	}
	if(!cvars.empty()){
		std::sort(cvars.begin(),cvars.end(),sortbyfirst);
		std::reverse(cvars.begin(),cvars.end());
		output  << "//    _____________\n" 
				<< "//   (REST OF CVARS)\n" 
				<< "//    ¯¯¯¯¯¯¯¯¯¯¯¯¯\n";
		while(!cvars.empty()){
			output << std::get<0>(cvars.back()) << "\n";
			cvars.pop_back();
		}
	}
	if(!validvstr.empty()){
		std::sort(validvstr.begin(),validvstr.end());
		std::reverse(validvstr.begin(),validvstr.end());
		output << "//  ______________\n" 
				<< "// (SCRIPTS IN USE)\n" 
				<< "//  ¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n";
		while(!validvstr.empty()){
			output << validvstr.back() << "\n";
			validvstr.pop_back();
		}
	}
	if(showunused){
		if(!nonstandard.empty()){
			std::sort(nonstandard.begin(),nonstandard.end());
			std::reverse(nonstandard.begin(),nonstandard.end());
			output << "//  _______________________________\n" 
					<< "// (UNUSED SCRIPTS/DEPRECATED CVARS)\n" 
					<< "//  ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n";
			while(!nonstandard.empty()){
				output << "//" << nonstandard.back() << "\n";
				nonstandard.pop_back();
			}
		}
	}
	output.close();
	std::cout << "SUCCESS: Generated " << filenames[4] << endl;

}
int main(int argc, char** argv){
	
	vector<std::string> filenames;
	std::ifstream inputfiles[4];
	initFiles(argc,argv,filenames,inputfiles);
	parseInput(filenames,inputfiles);
	generateOutput(filenames);

	//system("PAUSE");
	return 0;
	
}