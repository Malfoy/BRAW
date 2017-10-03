#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



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
	vector<uint> lengths;
	uint64_t size(0);
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			lengths.push_back(ref.size());
			size+=ref.size();
		}
	}
	sort(lengths.begin(),lengths.end(),greater<uint>());
	uint64_t total(0),i(0);
	while(total<size*0.5){
		total+=lengths[i];
		++i;
	}
	cout<<"#contigs: "<<intToString(lengths.size())<<endl;
	cout<<"Total size: "<<intToString(size)<<endl;
	cout<<"N50: "<<intToString(lengths[i-1])<<endl;
	cout<<"L50: "<<intToString(i)<<endl;
	total=i=0;
	while(total<size*0.80){
		total+=lengths[i];
		++i;
	}
	cout<<"N80: "<<intToString(lengths[i-1])<<endl;
	cout<<"L80: "<<intToString(i)<<endl;
	total=i=0;
	while(total<size*0.90){
		total+=lengths[i];
		++i;
	}
	cout<<"N90: "<<intToString(lengths[i-1])<<endl;
	cout<<"L90: "<<intToString(i)<<endl;
}
