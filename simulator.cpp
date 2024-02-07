#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>




using namespace std;



static uint32_t seed;



uint32_t xs(uint32_t& y){
	y^=(y<<13); y^=(y>>17);y=(y^=(y<<15)); return y;
}



char randNucle(char c='N'){
	// switch (rand()%4){
	switch (xs(seed)%4){
		case 0:
			if(c!='A'){
				return 'A';
			}
			return randNucle(c);
		case 1:
			if(c!='C'){
				return 'C';
			}
			return randNucle(c);
		case 2:
			if(c!='G'){
				return 'G';
			}
			return randNucle(c);
		case 3:
			if(c!='T'){
				return 'T';
			}
			return randNucle(c);
	}
	seed=xs(seed);
	return randNucle(c);
}



void insertion(double rate, string& result){
	unsigned int dice(rand() % 100);
	if(dice < rate){
		char newNucleotide(randNucle());
		result.push_back(newNucleotide);
		insertion(rate, result);
	}
}


uint64_t error_inserted(0);

//~ string mutateSequence(const string& referenceSequence,unsigned int mutRate, vector <double> ratioMutation={0.06,0.73,0.21}){
string mutateSequence(const string& referenceSequence,unsigned int mutRate, vector <double> ratioMutation={0.37,0.09,0.53}){//NANOPORE
	string result;
	// cout<<error_inserted<<endl;
	result.reserve(5 * referenceSequence.size());
	for(unsigned int i(0); i < referenceSequence.size(); ++i){
		// cout<<i<<endl;
		uint64_t substitutionRate(mutRate * ratioMutation[0]);
		uint64_t insertionRate(mutRate * ratioMutation[1]);
		uint64_t deletionRate(mutRate * ratioMutation[2]);
		// cout<<deletionRate<<endl;
		uint64_t bigint(1000000); 
		if(bigint<mutRate){
			cout<<"fail"<<endl;
			exit(0);
		}else{
			// cout<<bigint<<endl;
			// cout<<mutRate<<endl;
			// cin.get();
		}
		uint64_t dice(rand() % bigint);


		if (dice <substitutionRate ){
			// cout<<"SUBSTITUTION"<<endl;
			char newNucleotide(randNucle());
			while(newNucleotide == referenceSequence[i]){
				newNucleotide = randNucle();
			}
			result.push_back(newNucleotide);
			error_inserted++;
			continue;
		} else if(dice < deletionRate+substitutionRate){
			// cout<<"DELECTION"<<endl;
			uint64_t dice2(rand() % bigint);
			++i;
			error_inserted++;
			
			continue;
		} else if (dice < deletionRate + substitutionRate + insertionRate){
			// cout<<"INSERTION"<<endl;
			char newNucleotide(randNucle());
			result.push_back(referenceSequence[i]);
			result.push_back(newNucleotide);
			// insertion(deletionRate + substitutionRate + insertionRate, result); // larger than 1 insertions
			error_inserted++;
			continue;
		} else {
			result.push_back(referenceSequence[i]);
		}

	}
	return result;
}







int main(int argc, char ** argv){
	if(argc<5){
		cout<<"[Genome reference file] [read length] [coverage] [error rate] [prefix] [LR]"<<endl;
		exit(0);
	}
	seed=rand();
	bool long_reads(false);
	if(argc>6){
		long_reads=true;
		cout<<"LR"<<endl;
	}
	string input(argv[1]);
	double coverage(stof(argv[3]));
	float length(stof(argv[2]));
	srand (time(NULL));
	ifstream in(input);
	uint64_t errorRate((stof(argv[4]))*1000000);
	string prefix(argv[5]);
	string useless, ref,read,pread;
	unsigned int i(0);
	ofstream perfect("p."+prefix+".fa"),out(prefix+".fa");
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			uint64_t nucProduced(0);
			while(nucProduced<(uint64_t)(coverage*ref.size())){
				if(i%100==0){
					seed=(rand());
				}
				//produce a read
				uint64_t position=xs(seed)%ref.size();
				//~ unsigned int position=rand()%ref.size();
				if(position+length<=ref.size()){
					bool valid(true);
					unsigned int error(0);
					pread=ref.substr(position,length);
					read=pread;
					if(long_reads){
						read=mutateSequence(read,errorRate);
					}else{
						for(unsigned int i(0);i<read.size();++i){
							if(read[i]=='N' or read[i]=='n'){valid=false;break;}
							if(xs(seed)%100000<=errorRate){
							//~ if(rand()%errorRate==0){
								read[i]=randNucle(read[i]);
								++error;
							}
						}
					}
					if(valid){
						perfect<<">"+to_string(position)+"."+to_string(i)<<"\n";
						perfect<<pread<<"\n";
						out<<">"+to_string(position)+"."+to_string(i)<<"\n";
						out<<read<<"\n";
						nucProduced+=read.size();
						++i;
					}
				}
			}

		}
	}
	cout<<error_inserted<<" errors inserted"<<endl;
}
