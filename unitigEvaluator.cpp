#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "sparsepp/spp.h"

using spp::sparse_hash_map;





using namespace std;


string intToString(uint64_t n){
	if(n<1000){
		return to_string(n);
	}
	string end(to_string(n%1000));
	if(end.size()==3){
		return intToString(n/1000)+","+end;
	}
	if(end.size()==2){
		return intToString(n/1000)+",0"+end;
	}
	return intToString(n/1000)+",00"+end;
}



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



string getCanonical(const string& str){
	return (min(str,revComp(str)));
}



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
	if(not inRef.good() or not inUnitigs.good()){
		cout<<"Problem with files opening"<<endl;
		exit(1);
	}
	vector<uint> lengths;
	sparse_hash_map<string, bool> genomicKmers;
	uint64_t size(0),number(0);
	while(not inRef.eof()){
		getline(inRef,useless);
		getline(inRef,ref);
		if(not ref.empty() and not useless.empty()){
			for(uint i(0);i+k<=ref.size();++i){
				genomicKmers[getCanonical(ref.substr(i,k))]=false;
			}
		}
	}
	uint64_t FP(0),TP(0),FN(0);
	while(not inUnitigs.eof()){
		getline(inUnitigs,useless);
		getline(inUnitigs,ref);
		if(not ref.empty() and not useless.empty()){
			size+=ref.size();
			number++;
			for(uint i(0);i+k<=ref.size();++i){
				if(genomicKmers.count(getCanonical(ref.substr(i,k)))==0){
					FP++;
				}else{
					if(not genomicKmers[getCanonical(ref.substr(i,k))]){
						genomicKmers[getCanonical(ref.substr(i,k))]=true;
						TP++;
					}
				}
			}
		}
	}
	FN=genomicKmers.size()-TP;
	cout<<"Unitig number: "<<intToString(number)<< " Total size: "<<intToString(size)<<" Mean: "<<intToString(size/number)<<endl;
	cout<<"Genomic kmer in the reference: "<<intToString(genomicKmers.size())<<endl;
	cout<<"True positive (kmers in the unitig and the references, good kmers): "<<intToString(TP)<<endl;
	cout<<"False positive (kmers in the unitig and NOT in the references, erroneous kmers): "<<intToString(FP)<<endl;
	cout<<"False Negative (kmers NOT in the unitig but in the references, missed kmers): "<<intToString(FN)<<endl;
	cout<<"Erroneous kmer rate (*1000): "<<(double)1000*FP/(FP+TP)<<endl;
	cout<<"Missing kmer rate (*1000): "<<(double)1000*FN/genomicKmers.size()<<endl;

}
