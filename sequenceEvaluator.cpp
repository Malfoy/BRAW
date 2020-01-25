#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include "robin_hood.h"
#include "omp.h"




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





uint64_t inAnotinB(const robin_hood::unordered_map<string,bool>& A, const robin_hood::unordered_map<string,bool>& B){
	uint64_t res(0);
	for(auto it=A.begin(); it!=A.end(); ++it){
		if(B.count(it->first)==0){
			++res;
		}
	}
	return res;
}



uint64_t inAnotinBFULL(const vector<robin_hood::unordered_map<string,bool>>& A, const vector<robin_hood::unordered_map<string,bool>>& B,uint64_t& cardkmer){
	uint64_t res(0);
	#pragma omp parallel for
	for(uint64_t i=(0);i<1024;++i){
		uint64_t local(inAnotinB(A[i],B[i]));
		#pragma omp atomic
		res+=local;
		#pragma omp atomic
		cardkmer+=B[i].size();
	}
	return res;
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
	omp_set_nested(2);
	if(argc<4){
		cout<<"[unitig file] [reference file] [k value]"<<endl;
		exit(0);
	}
	string inputUnitig(argv[1]);
	string inputRef(argv[2]);
	uint64_t k(stoi(argv[3]));
	srand (time(NULL));

	ifstream inRef(inputRef),inSequences(inputUnitig);
	if(not inRef.good() or not inSequences.good()){
		cout<<"Problem with files opening"<<endl;
		exit(1);
	}
	vector<uint64_t> lengths;
	vector<robin_hood::unordered_map<string,bool>> genomicKmers(1024);
	vector<robin_hood::unordered_map<string,bool>> seenKmers(1024);
	vector<omp_lock_t> locks(1024);
	for(uint i(0);i<1024;++i){
		omp_init_lock(&locks[i]);
	}
	uint64_t size(0),number(0);
	std::chrono::duration<double> elapsed_seconds;
	uint64_t referenceKmers(0);
	uint64_t targetKmer(0);

	auto start = std::chrono::system_clock::now();
	#pragma omp parallel
	#pragma omp sections
	{
		#pragma omp section
		{
			#pragma omp parallel
			{
				string ref, useless,kmer;
				while(not inRef.eof()){
					#pragma omp critical (file1)
					{
						getline(inRef,useless);
						getline(inRef,ref);
					}
					if(not ref.empty() and not useless.empty()){
						for(uint64_t i(0);i+k<=ref.size();++i){
							kmer=getCanonical(ref.substr(i,k));
							uint64_t ilock(str2num(kmer.substr(0,5)));
							omp_set_lock(&locks[ilock]);
							genomicKmers[ilock][kmer]=true;
							omp_unset_lock(&locks[ilock]);
						}
					}
				}
			}
		}
		#pragma omp section
		{
			#pragma omp parallel
			{
				string ref, useless,kmer;
				while(not inSequences.eof()){
					#pragma omp critical (file2)
					{
						getline(inSequences,useless);
						getline(inSequences,ref);
					}
					if(not ref.empty() and not useless.empty()){
						for(uint64_t i(0);i+k<=ref.size();++i){
							kmer=getCanonical(ref.substr(i,k));
							uint64_t ilock(str2num(kmer.substr(0,5)));
							omp_set_lock(&locks[ilock]);
							seenKmers[ilock][kmer]=true;
							omp_unset_lock(&locks[ilock]);
						}
					}
				}
			}
		}
	}
	auto point2 = std::chrono::system_clock::now();
	elapsed_seconds = point2 - start;
	cout<<"Files loaded: "<<elapsed_seconds.count()<<endl;

	uint64_t FP(0),TP(0),FN(0);
	FP=inAnotinBFULL(seenKmers,genomicKmers,referenceKmers);
	auto point3 = std::chrono::system_clock::now();
	elapsed_seconds = point3 - point2;
	cout<<"FP computed: "<<elapsed_seconds.count()<<endl;
	FN=inAnotinBFULL(genomicKmers,seenKmers,targetKmer);
	auto point4 = std::chrono::system_clock::now();
	elapsed_seconds = point4 - point3;
	cout<<"FN computed: "<<elapsed_seconds.count()<<endl;
	TP=referenceKmers-FN;
	cout<<"Unitig number: "<<number<< " Total size: "<<size<<" Mean: "<<size/number<<endl;
	cout<<"Genomic kmer in the reference: "<<intToString(referenceKmers)<<endl;
	cout<<"Genomic kmer in the target: "<<intToString(targetKmer)<<endl;
	cout<<"True positive (kmers in the sequences and the references, good kmers): "<<intToString(TP)<<endl;
	cout<<"False positive (kmers in the sequences and NOT in the references, erroneous kmers): "<<intToString(FP)<<endl;
	cout<<"False Negative (kmers NOT in the sequences but in the references, missed kmers): "<<intToString(FN)<<endl;
	cout<<"Erroneous kmer rate (*1000): "<<(double)1000*FP/(FP+TP)<<endl;
	cout<<"Missing kmer rate (*1000): "<<(double)1000*FN/genomicKmers.size()<<endl;
	exit(0);
}
