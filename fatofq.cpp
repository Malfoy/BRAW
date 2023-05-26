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
	string ref, useless,toprint;
	ifstream in(input);
	vector<unsigned int> lengths;
	while(not in.eof()){
		getline(in,useless);
		getline(in,ref);
		if( not useless.empty()){
			if(not ref.empty()){
				toprint+="@"+useless.substr(1)+"\n"+ref+'\n'+'+'+'\n'+string(ref.size(),'I')+'\n';
			}else{
				toprint+="@"+useless.substr(1)+"\n"+ref+'\n'+'+'+'\n'+string(ref.size(),'I')+'\n';
			}
			if(toprint.size()>10000){
				cout<<toprint;
				toprint="";
			}
		}
	}
	cout<<toprint;
}
