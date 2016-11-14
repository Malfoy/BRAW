#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	srand (time(NULL));
	string ref, useless;
	ifstream in(input);
	vector<uint> lengths;
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(not ref.empty() and not useless.empty()){
			cout<<"@"+useless.substr(1)<<endl
			<<ref<<endl
			<<"+"<<endl
			<<string(ref.size(),'I')<<endl;
		}
	}
}
