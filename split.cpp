#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>


using namespace std;



int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] "<<endl;
		exit(0);
	}
	string input(argv[1]);
	srand (time(NULL));
	string ref, useless;
	ifstream in(input);
	ofstream out1("1.fa"),out2("2.fa");
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		out1<<useless<<endl<<ref<<endl;
		getline(in,useless);
		getline(in,ref);
		out2<<useless<<endl<<ref<<endl;
	}
}
