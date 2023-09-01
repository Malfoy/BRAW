#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "zstr.hpp"



using namespace std;


void clean(string& str){
	for(uint i(0); i< str.size(); ++i){
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
	if(argc<2){
		cout<<"[Fasta file] (clean)"<<endl;
		exit(0);
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
	string input(argv[1]);
	srand (time(NULL));
	string ref, useless,toprint;
	zstr::ifstream in(input);
	vector<uint> lengths;
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if( not useless.empty()){
			if(not ref.empty()){
				toprint+="@"+useless.substr(1)+"\n"+ref+'\n'+'+'+'\n'+string(ref.size(),'I')+'\n';
			}else{
				toprint+="@"+useless.substr(1)+"\n"+ref+'\n'+'+'+'\n'+string(ref.size(),'I')+'\n';
			}
			if(toprint.size()>10000){
				cout<<toprint;
				toprint="";
			}
		}
	}
	cout<<toprint;
}
