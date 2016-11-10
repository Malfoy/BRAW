#include <fstream>
#include <cstring>
#include <string>
#include <iostream>


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



// cmdline: ./bmean [<genome.fasta	xxhash]
int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[Genome reference file] "<<"[Heterozygous rate]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	float hetero(stof(argv[2]));
	uint64_t heteroRate;
	if(hetero==0){
		heteroRate=-1;
	}else{
		heteroRate=(1/hetero);
	}
	srand (time(NULL));
	ifstream in(input);
	//~ uint rate(10);
	string useless, ref,ref2;
	string fileName(input+(string)argv[2]+".fa");
	cout<<fileName<<endl;
	ofstream out(fileName,ios::trunc);
	uint nimp(0);
	while(not in.eof()){
		//~ cout<<"yo"<<endl;
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			ref2=ref;
			for(uint i(0);i<ref.size();++i){
				if(rand()%heteroRate==0){
					ref2[i]=randNucle(ref[i]);
				}
			}
			out<<">Reference"+to_string(nimp)<<endl;
			out<<ref<<endl;
			out<<">AlternativeReference"+to_string(nimp++)<<endl;
			out<<ref2<<endl;
		}
	}
}
