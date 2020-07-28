#include <fstream>
#include <cstring>
#include <string>
#include <iostream>



using namespace std;





uint32_t xs(uint32_t& y){
	y^=(y<<13); y^=(y>>17);y=(y^=(y<<15)); return y;
}





char randNuc(uint32_t& seed){
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



string random_sequence(uint64_t basesnumber,string& res,uint32_t& seed){
  res.clear();
  for(uint i(0);i<basesnumber;++i){
    res.push_back(randNuc(seed));
  }
  return res;
}




int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[#Bases] "<<"[#sequences]"<<endl;
		exit(0);
	}
	uint64_t basesnumber(stoi(argv[1]));
	uint64_t seqnumber(stoi(argv[2]));
	srand (time(NULL));
	
	uint i(0);
	#pragma omp parallel for
	for(i=0;i<seqnumber/1000;++i){
		string rs;
		uint32_t seed=(rand());
		for(int j(0);j<1000;++j){
			rs=random_sequence(basesnumber,rs,seed);
			#pragma omp critical
			{
				cout<<">1\n"<<rs<<'\n';
			}
		}
	}
}
