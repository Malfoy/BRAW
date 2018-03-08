#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <unordered_set>



using namespace std;



void get_reads_from_header_file_fastq(const string& file,unordered_set<string>& wanted_header){
	ifstream in(file);
	string header_str,read_str,useless;
	if(not in.good()){
		return;
	}
	while(not in.eof()){
		getline(in,header_str);
		getline(in,read_str);
		getline(in,useless);
		getline(in,useless);
		if(header_str.size()>1 and read_str.size()>1)
		if(wanted_header.count(header_str)==1){
			cout<<header_str<<'\n';
			cout<<read_str<<'\n';
			wanted_header.erase(header_str);
		}
		header_str=read_str=useless="";
	}
}



int main(int argc, char ** argv){
	if(argc<3){
		cout<<"[header file] [file of file] "<<endl;
		exit(0);
	}
	unordered_set<string> wanted_header;
	string header_file(argv[1]);
	string file_file(argv[2]);
	string file_name,header;

	//LOAD WANTED HEADER
	ifstream in_header(header_file);
	while(not in_header.eof()){
		getline(in_header,header);
		wanted_header.insert(header);
	}

	ifstream in_file(file_file);
	while(not in_file.eof()){
		getline(in_file,file_name);
		if(not wanted_header.empty()){
			get_reads_from_header_file_fastq(file_name,wanted_header);
		}
	}
	return 0;
}
