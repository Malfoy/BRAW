#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



int main(int argc, char ** argv){
	bool streaming(false);
	if(argc<2){
		streaming=true;
	}
	if(not streaming){
		string input(argv[1]);
		string ref, useless,header;
		ifstream in(input);
		while(not in.eof()){
			getline(in,header);
			getline(in,ref);
			getline(in,useless);
			getline(in,useless);
			if(header.size()>1){
				if(ref.size()>1){
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}else{
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}
			}
		}
	}else{
		string ref, useless,header;
		while(true){
			getline(cin,header);
			getline(cin,ref);
			getline(cin,useless);
			getline(cin,useless);
			if(header.size()>0){
				if(ref.size()>0){
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}else{
					cout<<">"+header.substr(1)<<"\n";
					cout<<ref<<"\n";
				}
			}else{
				return 0;
			}
		}
	}
}
