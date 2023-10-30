#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>



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



int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	srand (time(NULL));
	string ref, useless;
	ifstream in(input);
	if(not in){
		cout<<"Problem opening file"<<endl;
		return 0;
	}
	vector<unsigned int> lengths;
	uint64_t size(0);
	char c;
	uint32_t min(1000000000);
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		c=in.peek();
			while(c!='>' and not in.eof()){
				getline(in,useless);
				ref+=useless;
				c=in.peek();
			}
		if(not ref.empty() and not useless.empty()){
			lengths.push_back(ref.size());
			if(ref.size()<min){
				min=ref.size();
			}
			size+=ref.size();
		}
	}
	sort(lengths.begin(),lengths.end(),greater<unsigned int>());

	cout<<"#contigs: "<<intToString(lengths.size())<<endl;
	cout<<"Total size: "<<intToString(size) <<" bases (or "<<intToString(size-(min-1)*lengths.size())<<" "<<min<<"mer)"<<endl;

	vector<double> proportions_to_compute({0.5,0.75,0.9,0.99});
	vector<string> N,L;
	uint64_t total(0),i(0);
	for(unsigned int I(0);I<proportions_to_compute.size();++I){
		total=i=0;
		while(total<size*proportions_to_compute[I]){
			total+=lengths[i];
			++i;
		}
		N.push_back(intToString(lengths[i-1]));
		L.push_back(intToString(i));
	}
	cout<<"N50: "<<N[0]<<"		"<<"L50: "<<L[0]<<endl;
	cout<<"N75: "<<N[1]<<"		"<<"L75: "<<L[1]<<endl;
	cout<<"N90: "<<N[2]<<"		"<<"L90: "<<L[2]<<endl;
	cout<<"N99: "<<N[3]<<"		"<<"L99: "<<L[3]<<endl;
	cout<<"Average: "<<intToString(size/lengths.size())<<endl;

}
