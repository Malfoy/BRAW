#include <fstream>
#include <cstring>
#include <string>
#include <iostream>



using namespace std;



char revCompChar(char c) {
	switch (c) {
		case 'A': return 'T';
		case 'C': return 'G';
		case 'G': return 'C';
	}
	return 'A';
}



string revComp(const string& s){
	string rc(s.size(),0);
	for (int i((int)s.length() - 1); i >= 0; i--){
		rc[s.size()-1-i] = revCompChar(s[i]);
	}
	return rc;
}



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
	if(argc<6){
		cout<<"[Genome reference file] [read length] [fragment length] [coverage] [error rate] [prefix]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	uint length(stof(argv[2]));
	uint fragmentSize(stof(argv[3]));
	float coverage(stof(argv[4]));
	srand (time(NULL));
	ifstream in(input);
	uint errorRate(1/(stof(argv[5])));
	string prefix(argv[6]);
	string useless, ref,read,pread;
	uint i(0);
	ofstream perfect("p."+prefix+".fa"),out(prefix+".fa");

	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			uint nucProduced(0);
			while(nucProduced<coverage*ref.size()){
				//produce a read
				uint position=rand()%ref.size();
				if(position+fragmentSize<=ref.size()){
					bool valid(true);
					uint error(0);

					pread=ref.substr(position,length);
					read=pread;
					for(uint i(0);i<read.size();++i){
						if(read[i]=='N'){valid=false;break;}
						if(rand()%errorRate==0){
							read[i]=randNucle(read[i]);
							++error;
						}
					}
					if(valid){
						//~ perfect<<">"+to_string(i)<<" "<<position<<endl;
						//~ perfect<<pread<<endl;
						out<<">"+to_string(i)<<" "<<position<<endl;
						out<<read<<endl;
						nucProduced+=read.size();
						//~ ++i;
					}

					valid=(true);
					error=(0);
					pread=ref.substr(position+fragmentSize-length,length);
					read=pread;
					for(uint i(0);i<read.size();++i){
						if(read[i]=='N'){valid=false;break;}
						if(rand()%errorRate==0){
							read[i]=randNucle(read[i]);
							++error;
						}
					}
					if(valid){
						//~ perfect<<">"+to_string(i)<<"bis "<<(position+fragmentSize-length)<<endl;
						//~ perfect<<pread<<endl;
						out<<">"+to_string(i)<<"bis "<<(position+fragmentSize-length)<<endl;
						out<<revComp(read)<<endl;
						nucProduced+=read.size();
						++i;
					}
				}
			}
		}
	}

}
