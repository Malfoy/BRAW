#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
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



uint64_t str2num(const string& str){
	uint64_t res(0);
	for(uint64_t i(0);i<str.size();i++){
		res<<=2;
		switch (str[i]){
			case 'A':res+=0;break;
			case 'C':res+=1;break;
			case 'G':res+=2;break;
			default:res+=3;break;
		}
	}
	return res;
}



int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[unitig file] [reference file] [k value] [n for 2^n pass]"<<endl;
		exit(0);
	}
	auto start = chrono::system_clock::now();
	string inputUnitig(argv[1]);
	string inputRef(argv[2]);
	uint k(stoi(argv[3]));
	uint n(0);
	if(argc>4){
		 n=(stoi(argv[4]));
	}
	uint nbHash=1<<n;
	//cout<<nbHash<<endl;
	srand (time(NULL));
	string ref, useless;
	ifstream inRef(inputRef),inUnitigs(inputUnitig);
	if(not inRef.good() or not inUnitigs.good()){
		cout<<"Problem with files opening"<<endl;
		exit(1);
	}
	uint64_t size(0),number(0),genomicKmersNum(0);
	uint64_t FP(0),TP(0),FN(0);

	for(uint HASH(0);HASH<nbHash;++HASH){
		sparse_hash_map<string, bool> genomicKmers;
		while(not inRef.eof()){
			getline(inRef,useless);
			getline(inRef,ref);
			if(not ref.empty() and not useless.empty()){
				if (useless[0] != '>') 
				{
					cout << "reference file needs to be oneline'd (use BRAW/oneLine)" << endl; cout << useless << endl; exit(1);
				} 

				for(uint i(0);i+k<=ref.size();++i){
					if(str2num(getCanonical(ref.substr(i,k)))%nbHash==HASH){
						genomicKmers[getCanonical(ref.substr(i,k))]=false;
						//genomicKmersNum++; // this method of determining genomicKmersNum includes duplicates
					}else{
					}
				}
			}
		}

		while(not inUnitigs.eof()){
			getline(inUnitigs,useless);
			getline(inUnitigs,ref);
			if(not ref.empty() and not useless.empty()){
				if (useless[0] != '>') 
				{
					cout << "unitigs file needs to be oneline'd (use BRAW/oneLine)" << endl; exit(1);
				} 

				size+=ref.size();
				number++;
				for(uint i(0);i+k<=ref.size();++i){
					if(str2num(getCanonical(ref.substr(i,k)))%nbHash==HASH){
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
		}
		inUnitigs.clear();
		inUnitigs.seekg(0, std::ios::beg);
		inRef.clear();
		inRef.seekg(0, std::ios::beg);
		genomicKmersNum += genomicKmers.size(); // taking all distinct kmers in the reference
	}
	FN=genomicKmersNum-TP;
	cout<<"Unitig number: "<<intToString(number)<< " Total size: "<<intToString(size)<<" Mean: "<<intToString(size/number)<<endl;
	cout<<"Genomic kmer in the reference: "<<intToString(genomicKmersNum)<<endl;
	cout<<"True positive (kmers in the unitig and the references) 		GOOD kmers:	"<<intToString(TP)<<endl;
	cout<<"False positive (kmers in the unitig and NOT in the references)	ERRONEOUS kmers:	"<<intToString(FP)<<endl;
	cout<<"False Negative (kmers NOT in the unitig but in the references)	MISSING kmers:	"<<intToString(FN)<<endl;
	cout<<"Erroneous kmer rate (*1000): "<<(double)1000*FP/(FP+TP)<<endl;
	cout<<"Missing kmer rate (*1000): "<<(double)1000*FN/genomicKmersNum<<endl;

	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    cout << "\nFinished computation at " << ctime(&end_time)<< "Elapsed time: " << elapsed_seconds.count() << "s\n";

}
