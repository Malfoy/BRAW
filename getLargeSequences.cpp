#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[Fasta file] [Required length] "<<endl;
		exit(0);
	}
	string input(argv[1]);
	unsigned int length(stoi(argv[2]));
	srand (time(NULL));
	string ref, useless;
	ifstream in(input);
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if(ref.size()>length){
			cout<<useless<<endl<<ref<<endl;
		}
	}
}
