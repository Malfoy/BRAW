#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>


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


void clean(string& str, unsigned int sizeMin){
	string out;
	for(unsigned int i(0); i< str.size(); ++i){
		switch(str[i]){
			case 'a':out+="A";break;
			case 'A':out+="A";break;
			case 'c':out+="C";break;
			case 'C':out+="C";break;
			case 'g':out+="G";break;
			case 'G':out+="G";break;
			case 't':out+="T";break;
			case 'T':out+="T";break;
			default: if (out.size()> sizeMin){cout<<">x\n"<<out<<"\n";}out="";break;
		}
	}
	if (out.size()> sizeMin){cout<<">x\n"<<out<<"\n";}
}


int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] [minSize]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	unsigned int cleaning(stoi(argv[2]));
	srand (time(NULL));
	string header, sequence,line;
	ifstream in(input);
	vector<unsigned int> lengths;
	while(not in.eof()){
		getline(in,header);
		if(header[0]!='>'){continue;}
		char c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		clean(sequence,cleaning);
		sequence="";
	}
}
