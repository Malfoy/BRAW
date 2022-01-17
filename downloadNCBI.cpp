#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>



using namespace std;



string get_name_ncbi(const string& str){
  uint lastposition(0);
  for(uint i(0);i<str.size()-3;++i){
    if(str[i]=='/'){
      lastposition=i;
    }
  }
  lastposition++;
  return str.substr(lastposition,str.size()-lastposition);
}



void download_ncbi(const string& filename){
    //~ cout<<filename<<endl;
    string cmd("wget -q  "+filename+"/"+get_name_ncbi(filename)+"_genomic.fna.gz");
    //~ cout<<cmd<<endl;
    
    system(cmd.c_str());
    //~ cin.get();
}



int main(int argc, char ** argv){

	if(argc<1){
		cout<<"[genome file]"<<endl;
		exit(0);
	}
	string input(argv[1]);
	uint i(0);
	ifstream in(input);
    #pragma omp parallel
    {
        string line;
        while(not in.eof()){
            #pragma omp critical (in)
            {
                getline(in,line);
            }
            download_ncbi(line);
            #pragma omp atomic
            i++;
            //~ #pragma omp critical (out)
            //~ cout<<"Got "<<i<<" files"<<endl;
        }
    }
    cout<<"DONE! Got "<<i<<" files"<<endl;
}
