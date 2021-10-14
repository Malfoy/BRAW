#include "strict_fstream.hpp"
#include "zstr.hpp"
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>



using namespace std;





uint32_t xs(uint32_t& y){
	y^=(y<<13); y^=(y>>17);y=(y^=(y<<15)); return y;
}




__uint128_t xs(__uint128_t& g_lehmer64_state) {
  g_lehmer64_state *= 0xda942042e4dd58b5;
  return g_lehmer64_state >> 64;
}



char randNuc(__uint128_t& seed){
	switch (xs(seed)%4){
		case 0:
				return 'A';
		case 1:
				return 'C';
		case 2:
				return 'G';
		case 3:
				return 'T';
	}
	return 'A';
}



string random_sequence(uint64_t basesnumber,string& res,__uint128_t& seed){
  res.clear();
  for(uint i(0);i<basesnumber;++i){
    res.push_back(randNuc(seed));
  }
  return res;
}




int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[#Genome] [#size] [prefix]"<<endl;
		exit(0);
	}
	uint64_t genomes(stoll(argv[1]));
	uint64_t size(stoll(argv[2]));
    string  prefix((argv[3]));
	srand (time(NULL));

	uint i(0);
	// #pragma omp parallel for
	for(i=0;i<genomes;++i){
        zstr::ofstream out(prefix+to_string(i)+".fa.gz");
        cout<<prefix+to_string(i)+".fa.gz"<<endl;
		string rs;
		__uint128_t seed=(rand());
        rs=random_sequence(size,rs,seed);
        #pragma omp critical
        {
            out<<">1\n"<<rs<<'\n';
        }
	}
}
