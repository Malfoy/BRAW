#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "zstr.hpp"




using namespace std;



void clean(string& str){
	for(unsigned int i(0); i< str.size(); ++i){
		switch(str[i]){
			case 'a':break;
			case 'A':break;
			case 'c':break;
			case 'C':break;
			case 'g':break;
			case 'G':break;
			case 't':break;
			case 'T':break;
			case 'N':break;
			case 'n':break;
			default: str.clear();return;
		}
	}
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}



int main(int argc, char ** argv){
	bool streaming(false);
	if(argc<2){
		streaming=true;
	}
	bool cleaning(false),headercleaning(false);
	if(argc>2){
		if(string(argv[2])=="1" or string(argv[2])=="3"){
			cleaning=true;
		}
		if(string(argv[2])=="2" or string(argv[2])=="3"){
			headercleaning=true;
		}
	}
	if(not streaming){
		string input(argv[1]);
		string ref, useless,header;
		zstr::ifstream in(input);
		while(not in.eof()){
			getline(in,header);
			getline(in,ref);
			getline(in,useless);
			getline(in,useless);
			if(header.size()>1){
				if(ref.size()>1){
					if(headercleaning){
						cout<<">A"<<endl;
					}else{
						cout<<">"+header.substr(1)<<"\n";
					}
					if(cleaning){
						clean(ref);
					}
					cout<<ref<<"\n";
				}else{

				}
			}
		}
	}else{
		string ref, useless,header;
		while(true){
			getline(cin,header);
			getline(cin,ref);
			getline(cin,useless);
			getline(cin,useless);
			if(header.size()>0){
				if(ref.size()>0){
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}else{
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}
			}else{
				return 0;
			}
		}
	}
}
