#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>


using namespace std;


string getLineFasta(ifstream* in){
    string line,result;
    getline(*in,line);
    char c=in->peek();
    while(c!='>' and c!=EOF){
        getline(*in,line);
        result+=line;
        c=in->peek();
    }
    return result;
}


void clean(string& str){
    for(unsigned int i(0); i< str.size(); ++i){
        switch(str[i]){
            case 'a':break;
            case 'A':break;
            case 'c':break;
            case 'C':break;
            case 'g':break;
            case 'G':break;
            case 't':break;
            case 'T':break;
            default: str="";return;
        }
    }
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}


bool myfunction (pair<string,string> i,pair<string,string> j) { return (i.second.size()>j.second.size()); }


int main(int argc, char ** argv){
    vector<pair<string,string>> reads;
    if(argc<2){
        cout<<"[Fasta file] "<<endl;
        exit(0);
    }
    string input(argv[1]);
    bool cleaning(false);
    if(argc>2){
        cleaning=true;
    }
    srand (time(NULL));
    string header, sequence,line;
    ifstream in(input);
    vector<unsigned int> lengths;
    while(not in.eof()){
        getline(in,header);
        if(header[0]!='>'){continue;}
        char c=in.peek();
        while(c!='>' and c!=EOF){
            getline(in,line);
            sequence+=line;
            c=in.peek();
        }

        //~ cout<<header<<'\n'<<sequence<<"\n";
        reads.push_back({(header),sequence});
        sequence="";
    }

    sort(reads.begin(),reads.end(),myfunction);
    for(unsigned int i(0);i<reads.size();++i){
        cout<<">"<<reads[i].first<<"\n"<<reads[i].second<<"\n";
    }
}
