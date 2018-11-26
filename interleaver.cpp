#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "zstr.hpp"



using namespace std;



int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[Fasta file Forward] [Fasta file Reverse] [1 for fastq]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	string input2(argv[2]);
	bool fastq(false);
	if(argc>=4){
		fastq=true;
	}
	srand (time(NULL));
	string line,head,head2, sequence,junk;
	istream* in;
	istream* in2;
	in=new zstr::ifstream(input);
	in2=new zstr::ifstream(input2);
	vector<uint> lengths;
	while(not in->eof()){
		getline(*in,head);
		getline(*in2,head2);
		//~ int c=in.peek();
		//~ while(c!='>' and c!=EOF){
			getline(*in,sequence);
			//~ sequence+=line;
			//~ c=in.peek();
		//~ }
		if(fastq){
			getline(*in,junk);
			getline(*in,junk);
		}
		cout<<head<<"\n";
		transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);
		cout<<sequence<<"\n";
		sequence="";
		//~ c=in2.peek();
		//~ while(c!='>' and c!=EOF){
			getline(*in2,sequence);
			//~ sequence+=line;
			//~ c=in2.peek();
		//~ }
		if(fastq){
			getline(*in2,junk);
			getline(*in2,junk);
		}
		cout<<head2<<"\n";
		transform(sequence.begin(), sequence.end(), sequence.begin(), ::toupper);
		cout<<sequence<<"\n";
		sequence="";
	}
	cout<<flush;
	return 0;
}
