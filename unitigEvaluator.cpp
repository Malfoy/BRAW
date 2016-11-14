#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>



using namespace std;



int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[unitig file] [reference file] [k value]"<<endl;
		exit(0);
	}
	string inputUnitig(argv[1]);
	string inputRef(argv[2]);
	uint k(stoi(argv[3]));
	srand (time(NULL));
	string ref, useless;
	ifstream inRef(inputRef),inUnitigs(inputUnitig);
	vector<uint> lengths;
	unordered_set<string> genomicKmers;
	uint size(0),number(0);
	while(not inRef.eof()){
		getline(inRef,useless);
		getline(inRef,ref);
		if(not ref.empty() and not useless.empty()){
			for(uint i(0);i+k<ref.size();++i){
				genomicKmers.insert(ref.substr(i,k));
			}
		}
	}
	uint FP(0),TP(0),FN(0);
	while(not inUnitigs.eof()){
		getline(inUnitigs,useless);
		getline(inUnitigs,ref);
		if(not ref.empty() and not useless.empty()){
			size+=ref.size();
			number++;
			for(uint i(0);i+k<ref.size();++i){
				if(genomicKmers.count(ref.substr(i,k))==0){
					FP++;
				}else{
					TP++;
				}
			}
		}
	}
	FN=genomicKmers.size()-TP;
	cout<<"Unitig number: "<<number<< " Total size: "<<size<<" Mean: "<<size/number<<endl;
	cout<<"True positive (kmers in the unitig and the references, good kmers): "<<TP<<endl;
	cout<<"False positive (kmers in the unitig and NOT in the references, erroneous kmers): "<<FP<<endl;
	cout<<"False Negative (kmers NOT in the unitig but in the references, missed kmers): "<<FN<<endl;
}
