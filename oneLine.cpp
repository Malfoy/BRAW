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
	string line, sequence;
	ifstream in(input);
	vector<uint> lengths;
	while(not in.eof()){
		getline(in,line);
		cout<<line<<endl;
		int c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);
		cout<<sequence<<endl;
		sequence="";
	}
}
