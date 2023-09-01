#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include "zstr.hpp"



using namespace std;



char randNucle(char c){
	switch (rand()%4){
		case 0:
			if(c!='A'){
				return 'A';
			}
		case 1:
			if(c!='C'){
				return 'C';
			}
		case 2:
			if(c!='G'){
				return 'G';
			}
		case 3:
			if(c!='T'){
				return 'T';
			}
	}
	return randNucle(c);
}



char randNuc(char c){
	switch (rand()%4){
		case 0:
				return 'A';
		case 1:
				return 'C';
		case 2:
				return 'G';
		case 3:
				return 'T';
	}
	return randNucle(c);
}

zstr::ifstream* openFile(const string& input_file){
	zstr::ifstream* input_stream = new zstr::ifstream(input_file);
	if(not input_stream-> good()){
		cout << "Problem with file opening" << endl;
        return NULL;
	}
	return input_stream;
}

int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[Genome reference file] "<<"[Heterozygous rate]"<<"[Out file name]"<<endl;
		exit(0);
	}
	unsigned int snp1(0),snp2(0);
	string input(argv[1]);
	float hetero(stof(argv[2])/2);
	string outName(argv[3]);
	uint64_t heteroRate;
	if(hetero==0){
		heteroRate=-1;
	}else{
		heteroRate=(1/hetero);
	}
	srand (time(NULL));
	zstr::ifstream* in = openFile(input);
	if(in==NULL){
        cout<<"Can't open file: "<<input<<endl;
        return -1;
    }
    if(not in->good()){
        cout<<"Can't open file: "<<input<<endl;
        return -1;
    }
	//~ unsigned int rate(10);
	string useless, ref,ref2,ref3;
	//~ cout<<fileName<<endl;
	zstr::ofstream* out = (new zstr::ofstream(outName));
	unsigned int nimp(0);
	while(not in->eof()){
		getline(*in,useless);
		getline(*in,ref);
		if(not ref.empty() and not useless.empty()){
			ref3=ref2=ref;
			for(unsigned int i(0);i<ref.size();++i){
				if(rand()%heteroRate==0){
					ref2[i]=randNucle(ref[i]);
					snp1++;
				}
				if(rand()%heteroRate==0){
					ref3[i]=randNucle(ref[i]);
					snp2++;
				}
			}
			ref2+="\n";
			string header = ">AlternativeReference1:"+to_string(snp1)+"\n";
			out->write(header.c_str(), header.size());
			out->write(ref2.c_str(), ref2.size());
			//header = ">AlternativeReference2:" + to_string(snp2) + "\n";
			//out->write(header.c_str(), header.size());
			//out->write(ref3.c_str(), ref3.size());
		}
	}
	delete in;
	delete out;
}
