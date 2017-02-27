#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[Fasta file Forward] [Fasta file Reverse]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	string input2(argv[2]);
	srand (time(NULL));
	string line,head,head2, sequence;
	ifstream in(input.c_str());
	ifstream in2(input2.c_str());
	vector<uint> lengths;
	while(not in.eof()){
		getline(in,head);
		getline(in2,head2);
		int c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		cout<<head<<endl;
		transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);
		cout<<sequence<<endl;
		sequence="";
		c=in2.peek();
		while(c!='>' and c!=EOF){
			getline(in2,line);
			sequence+=line;
			c=in2.peek();
		}
		cout<<head2<<endl;
		transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);
		cout<<sequence<<endl;
		sequence="";
	}
}
