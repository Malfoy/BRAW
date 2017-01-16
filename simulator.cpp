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



int main(int argc, char ** argv){
	if(argc<5){
		cout<<"[Genome reference file] [read length] [coverage] [error rate]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	float coverage(stof(argv[3]));
	float length(stof(argv[2]));
	srand (time(NULL));
	ifstream in(input);
	uint errorRate(1/(stof(argv[4])));
	string useless, ref,read;
	uint i(0);
	ofstream perfect("perfectReads.fa"),out("reads.fa");
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			uint nucProduced(0);
			while(nucProduced<coverage*ref.size()){
				uint position=rand()%ref.size();
				if(position+length<=ref.size()){
					read=ref.substr(position,length);
					perfect<<">"+i<<endl;
					perfect<<read<<endl;
					for(uint i(0);i<read.size();++i){
						if(rand()%errorRate==0){
							read[i]=randNucle(read[i]);
						}
					}
					out<<">"+i<<endl;
					out<<read<<endl;
					nucProduced+=read.size();
				}
			}

		}
	}
}
