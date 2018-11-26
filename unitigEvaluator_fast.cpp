#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <array>
#include <unordered_map>



__uint128_t offsetUpdateAnchors;
__uint128_t anchorSize;



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



__uint128_t	 str2num(const string& str){
	__uint128_t res(0);
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


__uint128_t nuc2int(char c){
	switch(c){
		//~ case 'A': return 0;
		case 'C': return 1;
		case 'G': return 2;
		case 'T': return 3;
	}
	return 0;
}



__uint128_t nuc2intrc(char c){
	switch(c){
		case 'A': return 3;
		case 'C': return 2;
		case 'G': return 1;
		//~ case 'T': return 0;
	}
	return 0;
}


__uint128_t rcb(__uint128_t min,uint n){
	__uint128_t res(0);
	__uint128_t offset(1);
	offset<<=(2*n-2);
	for(uint i(0); i<n;++i){
		res+=(3-(min%4))*offset;
		min>>=2;
		offset>>=2;
	}
	return res;
}





void updateK(__uint128_t& min, char nuc){
	min<<=2;
	min+=nuc2int(nuc);
	min%=offsetUpdateAnchors;
}



void updateRCK(__uint128_t& min, char nuc){
	min>>=2;
	min+=(nuc2intrc(nuc)<<(2*(anchorSize-1)));
}



struct KeyHasher
{
  std::size_t operator()(__uint128_t y) const
  {
    y^=(y<<13); y^=(y>>17); return (y^=(y<<15));
  }
};



hash<uint64_t> bhash;



int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[FILE file] [reference file] [k value] [n for 2^n pass]"<<endl;
		exit(0);
	}
	auto start = chrono::system_clock::now();
	string inputFILE(argv[1]);
	string inputRef(argv[2]);
	uint k(stoi(argv[3]));
	anchorSize=k;
	offsetUpdateAnchors=1;
	offsetUpdateAnchors<<=(2*(anchorSize));
	uint n(0);
	if(argc>4){
		 n=(stoi(argv[4]));
	}
	uint nbHash=1<<n;
	//~ cout<<"number of pass"<<nbHash<<endl;
	srand (time(NULL));
	ifstream inRef(inputRef),inFILE(inputFILE);
	if(not inRef.good()){
		cout<<"Problem with ref file opening"<<endl;
		exit(1);
	}

	if(not inRef.good()){
		cout<<"Problem with file of file opening"<<endl;
		exit(1);
	}
	uint64_t genomicKmersNum(0);
	vector<unordered_map<__uint128_t, bool,KeyHasher>> genomicKmers;
	array<mutex, 1024> nutex;
	genomicKmers.resize(nbHash);
	//~ for(uint HASH(0);HASH<nbHash;++HASH)
	{

		#pragma omp parallel num_threads(20)
		while(not inRef.eof()){
			string ref, useless;
			#pragma omp critical(file_ref)
			{
				getline(inRef,useless);
				getline(inRef,ref);
			}
			if(not ref.empty() and not useless.empty()){
				__uint128_t seq(str2num(ref.substr(0,anchorSize))),rcSeq(rcb(seq,anchorSize)),canon(min(seq,rcSeq));
				uint Hache(bhash((uint64_t)canon)%nbHash);
				//~ #pragma omp critical(Hache)
				{
					nutex[Hache].lock();
					//~ cout<<Hache<<endl;
					genomicKmers[Hache][canon]=true;
					nutex[Hache].unlock();
				}
				#pragma omp atomic
				genomicKmersNum++;
				for(uint j(0);j+anchorSize<ref.size();++j){
					updateK(seq,ref[j+anchorSize]);
					updateRCK(rcSeq,ref[j+anchorSize]);
					canon=(min(seq, rcSeq));
					uint Hache(bhash((uint64_t)canon)%nbHash);
					//~ #pragma omp critical(Hache)
					{
						nutex[Hache].lock();
						genomicKmers[Hache][canon]=true;
						nutex[Hache].unlock();
					}
					#pragma omp atomic
					genomicKmersNum++;
				}
			}
			//~ if((genomicKmersNum/1000)%100==0)
				//~ cout<<genomicKmersNum<<endl;
		}
		cout<<"Ref indexed"<<endl;

		#pragma omp parallel num_threads(20)
		while (not inFILE.eof()){
			uint64_t size(0),number(0);
			uint64_t FP(0),TP(0),FN(0);
			string fileName;
			#pragma omp critical(FF)
			{
				getline(inFILE,fileName);
				if(fileName.size()>2){
					cout<<"I evaluate "+fileName<<endl;
				}
			}
			ifstream inUnitigs(fileName);
			if(fileName.size()<2){
				continue;
			}


			uint t;
			#pragma omp parallel for num_threads(20)
			for(t=0;t<20;++t){
				string seq_str,useless;
				uint size_local(0),number_local(0),FP_local(0),TP_local(0);
				while(not inUnitigs.eof()){

					#pragma omp critical(read_file)
					{
						getline(inUnitigs,useless);
						getline(inUnitigs,seq_str);
					}
					if(not seq_str.empty() and not useless.empty()){
						size_local+=seq_str.size();
						number_local++;
						__uint128_t seq(str2num(seq_str.substr(0,anchorSize))),rcSeq(rcb(seq,anchorSize)),canon(min(seq,rcSeq));
						//~ if(bhash((uint64_t)canon)%nbHash==HASH
							uint Hache(bhash((uint64_t)canon)%nbHash);
							if(genomicKmers[Hache].count(canon)==0){
								FP_local++;
							}else{
								TP_local++;
							}
						//~ }
						for(uint j(0);j+anchorSize<seq_str.size();++j){
							updateK(seq,seq_str[j+anchorSize]);
							updateRCK(rcSeq,seq_str[j+anchorSize]);
							canon=(min(seq, rcSeq));
							uint Hache(bhash((uint64_t)canon)%nbHash);
							//~ if(bhash((uint64_t)canon)%nbHash==HASH){
								if(genomicKmers[Hache].count(canon)==0){
									FP_local++;
								}else{
									TP_local++;
								}
							//~ }
						}

					}
				}
				#pragma omp critical(result)
				{
					size+=size_local;
					number+=number_local;
					FP+=FP_local;
					TP+=TP_local;
				}

			}
			inUnitigs.clear();
			inUnitigs.seekg(0, std::ios::beg);
			inRef.clear();
			inRef.seekg(0, std::ios::beg);
			FN=genomicKmersNum-TP;
			#pragma omp critical(output)
			{
				cout<<"\nUnitig number: "<<intToString(number)<< " Total size: "<<intToString(size)<<" Mean: "<<intToString(size/number)<<endl;
				cout<<"Genomic kmer in the reference: "<<intToString(genomicKmersNum)<<endl;
				cout<<"True positive (kmers in the unitig and the references) 		GOOD kmers:	"<<intToString(TP)<<endl;
				cout<<"False positive (kmers in the unitig and NOT in the references)	ERRONEOUS kmers:	"<<intToString(FP)<<endl;
				cout<<"False Negative (kmers NOT in the unitig but in the references)	MISSING kmers:	"<<intToString(FN)<<endl;
				cout<<"Erroneous kmer rate (*10,000): "<<(double)10000*FP/(FP+TP)<<endl;
				cout<<"Missing kmer rate (*10,000): "<<(double)10000*FN/genomicKmersNum<<endl;
				auto end = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = end - start;
				time_t end_time = chrono::system_clock::to_time_t(end);
				cout << "Finished pass at " << ctime(&end_time)<< "Elapsed time: " << elapsed_seconds.count() << "s\n";
				cout<<"It was "+fileName<<"\n\n"<<endl;
			}
		}
	}
	//~ FN=genomicKmersNum-TP;
	//~ cout<<"Unitig number: "<<intToString(number)<< " Total size: "<<intToString(size)<<" Mean: "<<intToString(size/number)<<endl;
	//~ cout<<"Genomic kmer in the reference: "<<intToString(genomicKmersNum)<<endl;
	//~ cout<<"True positive (kmers in the unitig and the references) 		GOOD kmers:	"<<intToString(TP)<<endl;
	//~ cout<<"False positive (kmers in the unitig and NOT in the references)	ERRONEOUS kmers:	"<<intToString(FP)<<endl;
	//~ cout<<"False Negative (kmers NOT in the unitig but in the references)	MISSING kmers:	"<<intToString(FN)<<endl;
	//~ cout<<"Erroneous kmer rate (*10,000): "<<(double)10000*FP/(FP+TP)<<endl;
	//~ cout<<"Missing kmer rate (*10,000): "<<(double)10000*FN/genomicKmersNum<<endl;

	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    cout << "\nFinished computation at " << ctime(&end_time)<< "Elapsed time: " << elapsed_seconds.count() << "s\n";

}
