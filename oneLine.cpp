#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "zstr.hpp"



using namespace std;



string getLineFasta(ifstream* in){
	string line,result;
	getline(*in,line);
	char c=in->peek();
	while(c!='>' and c!=EOF){
		getline(*in,line);
		result+=line;
		c=in->peek();
	}
	return result;
}


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
			default: cout<<str[i]<<endl; str[i]='A';return;
		}
	}
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}




string remove_lowercase(string& str){
	string res;
	for(uint i(0); i< str.size(); ++i){
		switch(str[i]){
			//~ case 'a':break;
			case 'A':res+='A';break;
			//~ case 'c':break;
			case 'C':res+='C';break;
			//~ case 'g':break;
			case 'G':res+='G';break;
			//~ case 't':break;
			case 'T':res+='T';break;
			default:;
		}
	}
	return res;
}



int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] (1 for cleaning, 2 for header cleaning, 3 for both) (minimum size)"<<endl;
		exit(0);
	}
	string input(argv[1]);
	bool cleaning(false),headercleaning(false);
	if(argc>2){
		if(string(argv[2])=="1" or string(argv[2])=="3"){
			cleaning=true;
		}
		if(string(argv[2])=="2" or string(argv[2])=="3"){
			headercleaning=true;
		}
	}
	uint min_size(0);
	if(argc>3){
		min_size=(stoi(argv[3]));
	}
	//~ uint count(0);
	srand (time(NULL));
	string header, sequence,line;
	zstr::ifstream in(input);
	while(not in.eof()){
		getline(in,header);
		if(header[0]!='>'){continue;}
		char c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		if(headercleaning){
			header=">a";
		}
		if(cleaning){
			clean(sequence);
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence<<"\n";
			}
		}else{
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence<<"\n";
			}
		}
		sequence="";
	}
}
