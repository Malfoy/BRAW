#include <fstream>
#include <cstring>
#include <string>
#include <iostream>



using namespace std;


static unsigned int seed;


uint32_t xs(uint32_t& y){
	y^=(y<<13); y^=(y>>17); return (y^=(y<<15));
}



char randNucle(char c){
	//~ switch (rand()%4){
	switch (xs(seed)%4){
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
		cout<<"[Genome reference file] [read length] [coverage] [error rate] [prefix]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	float coverage(stof(argv[3]));
	float length(stof(argv[2]));
	srand (time(NULL));
	ifstream in(input);
	uint errorRate(1/(stof(argv[4])));
	string prefix(argv[5]);
	string useless, ref,read,pread;
	uint i(0);
	ofstream perfect("p."+prefix+".fa"),out(prefix+".fa");
	while(not in.eof()){
		seed=(rand());
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			uint64_t nucProduced(0);
			while(nucProduced<coverage*ref.size()){
				//produce a read
				uint position=xs(seed)%ref.size();
				//~ uint position=rand()%ref.size();
				if(position+length<=ref.size()){
					bool valid(true);
					uint error(0);
					pread=ref.substr(position,length);
					read=pread;
					for(uint i(0);i<read.size();++i){
						if(read[i]=='N' or read[i]=='n'){valid=false;break;}
						if(xs(seed)%errorRate==0){
						//~ if(rand()%errorRate==0){
							read[i]=randNucle(read[i]);
							++error;
						}
					}
					if(valid){
						perfect<<">"+to_string(i)<<"\n";
						perfect<<pread<<"\n";
						out<<">"+to_string(i)<<"\n";
						out<<read<<"\n";
						nucProduced+=read.size();
						++i;
					}
				}
			}

		}
	}
}
