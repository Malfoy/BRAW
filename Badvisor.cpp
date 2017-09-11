#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <unordered_map>



using namespace std;



void help(){
	cout<<"./badvisor reads.fa"<<endl;
}



void printInt(const int value){
	string numWithCommas = to_string(value);
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition-=3;
	}
	cout<<numWithCommas<<endl;
}



int main(int argc, char *argv[]) {
	if(argc<2){
		help();
		exit(0);
	}
	string readsFile(argv[1]),line,lineF0,lineF1;
	vector<vector<uint>> histograms;
	vector<uint> numberKmerDistinct,minimumList;
	uint frac(1000);
	for(uint k(21);k<201;k+=10){
		ifstream stream(readsFile+"_k"+to_string(k)+".hist");
		if(not stream.is_open()){
			cout<<"no file "+readsFile+"_k"+to_string(k)+".hist"<<endl;
			break;
		}
		getline(stream,lineF1,'	');
		getline(stream,lineF1);
		getline(stream,lineF0,'	');
		lineF0="";
		getline(stream,lineF0);
		//~ uint64_t numberKmer(stoi(lineF1));
		numberKmerDistinct.push_back(stoi(lineF0));
		vector<uint> abundances;
		while(not stream.eof()){
			getline(stream,line,'	');
			getline(stream,line);
			if(line.size()>0){
				abundances.push_back(stoi(line));
			}
		}
		histograms.push_back(abundances);
	}
	bool cont(true);
	for(int i(0);i<histograms.size() and cont;++i){
		for(uint ii(0);ii<histograms[i].size() and cont;++ii){
			if(histograms[i][ii]<=histograms[i][ii+1]){
				if(ii>=5){
					minimumList.push_back(ii);
				}else{
					cont=false;
				}
				break;
			}
		}
	}
	cout<<minimumList.size()*10+1<<endl;
    return 0;
}
