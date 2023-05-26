#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



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
			default: str="";return;
		}
	}
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}


int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] (1 for cleaning) (minimum size)"<<endl;
		exit(0);
	}
	string input(argv[1]);
	bool cleaning(false);
	if(argc>2){
		if(string(argv[2])=="1"){
			cleaning=true;
		}
	}
	unsigned int min_size(0);
	if(argc>3){
		min_size=(stoi(argv[3]));
	}
	srand (time(NULL));
	string header, sequence,line;
	ifstream in(input);
	while(not in.eof()){
		getline(in,header);
		if(header[0]!='>'){continue;}
		char c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		if(cleaning){
			clean(sequence);
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence.substr(0,sequence.size()/2)<<"\n";
				cout<<header<<'\n'<<sequence.substr(sequence.size()/2)<<"\n";
			}
		}else{
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence.substr(0,sequence.size()/2)<<"\n";
				cout<<header<<'\n'<<sequence.substr(sequence.size()/2)<<"\n";
			}
		}
		sequence="";
	}
}
