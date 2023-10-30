#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "zstr.hpp"
#include "strict_fstream.hpp"
#include <cstdint>


using namespace std;



string fq2fa_name(string str){
    for(unsigned int i=0; i <str.length(); i++){
        if(str[i] == '.'){
            return str.substr(0,i)+".fa.gz";
        }
    }
    return str+".fa.gz";
}


void fq2fa_file_keep_header(string input){
    string ref, useless,header;
    zstr::ifstream in(input);
    string outfile(fq2fa_name(input));
    zstr::ofstream out(outfile);
    while(not in.eof()){
        getline(in,header);
        getline(in,ref);
        getline(in,useless);
        getline(in,useless);
        if(header.size()>1){
            if(ref.size()>1){
                out<<">"+header.substr(1)<<"\n";
                out<<ref<<"\n";
            }else{
                out<<">"+header.substr(1)<<"\n";
                out<<ref<<"\n";
            }
        }
    }
}


void fq2fa_file_no_header(string input){
    string ref, useless,header;
    zstr::ifstream in(input);
    string outfile(fq2fa_name(input));
    cout<<outfile<<endl;
    zstr::ofstream out(outfile);
    while(not in.eof()){
        getline(in,header);
        getline(in,ref);
        getline(in,useless);
        getline(in,useless);
        if(header.size()>1){
            if(ref.size()>1){
                out<<">A\n";
                out<<ref<<"\n";
            }else{
                out<<">A\n";
                out<<ref<<"\n";
            }
        }
    }
}




int main(int argc, char ** argv){
    bool keep_header(false);
	if(argc<2){
		keep_header=true;
        cout<<"I keep the headers"<<endl;
	}
    string input(argv[1]);

    ifstream in(input);
    #pragma omp parallel
    {
        string file;
        while(not in.eof()){
        #pragma omp critical (inputfile)
        {
            getline(in,file);
        }
        if(file.size()<2){continue;}
        if(keep_header){
            fq2fa_file_keep_header(file);
        }else{
            fq2fa_file_no_header(file);
        }
        remove(file.c_str());
    }
    }


}
