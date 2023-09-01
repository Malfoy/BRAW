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
  for(unsigned int i(0);i<basesnumber;++i){
    res.push_back(randNuc(seed));
  }
  return res;
}




int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[#Bases] "<<"[#sequences]"<<endl;
		exit(0);
	}
	uint64_t basesnumber(stoll(argv[1]));
	uint64_t seqnumber(stoll(argv[2]));
	srand (time(NULL));
	// cout<<basesnumber<<" "<<seqnumber<<endl;

	unsigned int i(0);
	#pragma omp parallel for
	for(i=0;i<seqnumber/100;++i){
		string rs;
		__uint128_t seed=(rand());
		for(int j(0);j<100;++j){
			rs=random_sequence(basesnumber,rs,seed);
			#pragma omp critical
			{
				cout<<">1\n"<<rs<<'\n';
			}
		}
	}
}
