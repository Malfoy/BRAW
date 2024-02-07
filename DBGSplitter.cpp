#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include "sparsepp/spp.h"
#include <cstdint>
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
	for(unsigned int i(0);i<str.size();i++){
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


uint32_t xs(uint32_t& y){
	y^=(y<<13); y^=(y>>17);y=(y^=(y<<15)); return y;
}


string min_accordingtoXS(const string& seq1,const string& seq2){
	uint32_t u1(str2num(seq1)),u2(str2num(seq2));
	if(xs(u1)<xs(u2)){
		return seq1;
	}
	return seq2;
}



int main(int argc, char ** argv){
	if(argc<4){
		cout<<"[unitig file] [k value]  [m value]"<<endl;
		exit(0);
	}
	string inputUnitig(argv[1]);
	unsigned int k(stoi(argv[2]));
	unsigned int m(stoi(argv[3]));
	srand (time(NULL));
	string ref, useless;
	ifstream inUnitigs(inputUnitig);
	if( not inUnitigs.good()){
		cout<<"Problem with files opening"<<endl;
		exit(1);
	}

	string super_minimizer,kmer,minimizer,mmer;
	while(not inUnitigs.eof()){
		getline(inUnitigs,useless);
		getline(inUnitigs,ref);
		//FOREACH UNITIG
		if(not ref.empty() and not useless.empty()){
			super_minimizer=kmer=minimizer=mmer="";
			unsigned int last_position(0);
			//FOREACH KMER
			unsigned int i(0);
			for(;i+k<=ref.size();++i){
				kmer=getCanonical(ref.substr(i,k));
				minimizer=(getCanonical(kmer.substr(0,m)));
				//COMPUTE KMER MINIMIZER
				for(unsigned int j(1);j+m<=kmer.size();++j){
					mmer=getCanonical(kmer.substr(j,m));
					minimizer=min_accordingtoXS(minimizer,mmer);
				}
				if(super_minimizer==""){
					super_minimizer=minimizer;
				}else{
					if(super_minimizer!=minimizer){
						//~ cerr<<super_minimizer<<" "<<minimizer<<endl;
						cout<<">\n"<<ref.substr(last_position,i-last_position+k-1)<<"\n";
						last_position=i;
						super_minimizer=minimizer;
					}
				}
			}
			if(i-last_position+k-1!=0){
				cout<<">\n"<<ref.substr(last_position,i-last_position+k-1)<<"\n";
			}
		}
	}


}
