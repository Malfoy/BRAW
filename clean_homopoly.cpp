#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>



using namespace std;



string clean_prefix(string str,unsigned int min_length=15,unsigned int max_missmatch=0){
	char c(str[0]);
	unsigned int prefix_length(1);
	unsigned int i(1);
	unsigned int miss(0);
	unsigned int con_match(0);

	for(;i< str.size();++i){
		if(str[i]==c){
			prefix_length++;
			con_match++;
		}else{
			con_match=0;
			if(++miss>max_missmatch){
				break;
			}
		}
	}
	if(con_match<5){
		prefix_length-=con_match+1;
	}
	if(prefix_length>min_length){
		str=str.substr(i);
	}
	return str;
}



string clean_suffix(string str,unsigned int min_length=15,unsigned int max_missmatch=0){
	unsigned int j(1);
	unsigned int suffix_length(0);
	unsigned int miss(0);
	unsigned int con_match(0);
	char c=(str[str.size()-1]);
	for(;j< str.size();++j){
		if(str[str.size()-1-j]==c){
			suffix_length++;
			con_match++;
		}else{
			con_match=0;
			if(++miss>max_missmatch){
				break;
			}
		}
	}
	if(con_match<5){
		suffix_length-=con_match+1;
	}
	if(suffix_length>min_length){
		str=str.substr(0,str.size()-j);
	}
	return str;
}



string clean_homo(string str,unsigned int advanced=0, unsigned int min_length=15,unsigned int max_missmatch=0){
	str=clean_prefix(str,10,1);
	if(str.size()<2){
		return "";
	}
	//~ string str2=clean_prefix(str.substr(1),10);
	//~ if(str2.size()+1<str.size()){
		//~ str=str2;
	//~ }
	//~ if(str.size()<2){
		//~ return "";
	//~ }
	str=clean_suffix(str,10,1);
	//~ if(str.size()<2){
		//~ return "";
	//~ }
	//~ str2=clean_suffix(str.substr(0,str.size()-1),10);
	//~ if(str2.size()+1<str.size()){
		//~ str=str2;
	//~ }
	return str;
}


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


int main(int argc, char ** argv){
	if(argc<2){
		cout<<"[Fasta file] (minimum size)"<<endl;
		exit(0);
	}
	string input(argv[1]);
	bool cleaning(true);
	unsigned int min_size(0);
	if(argc>2){
		min_size=(stoi(argv[2]));
	}
	srand (time(NULL));
	string header, sequence,line;
	ifstream in(input);
	while(not in.eof()){
		getline(in,header);
		if(header[0]!='>'){continue;}
		char c=in.peek();
		while(c!='>' and c!=EOF){
			getline(in,line);
			sequence+=line;
			c=in.peek();
		}
		if(cleaning){
			clean(sequence);
			sequence=clean_homo(sequence,1);
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence<<"\n";
			}
		}else{
			if(sequence.size()>min_size){
				cout<<header<<'\n'<<sequence<<"\n";
			}
		}
		sequence="";
	}
}
