#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <unordered_map>



using namespace std;



void help(){
	cout<<"./correctionEvaluator perfectReads.fa erroneousReads.fa correctedReads.fa  "<<endl;
}



int main(int argc, char *argv[]) {
	if(argc<3){
		help();
		exit(0);
	}
	string perfect(argv[1]);
	string erroneous(argv[2]);
	string corrected((argv[3]));
	string perfectRead,erroneousRead,correctedRead,useless;
	ifstream pstream(perfect);
	ifstream eStream(erroneous);
	ifstream cStream(corrected);
	uint TP(0),FP(0),FN(0),errors(0);
	while(not pstream.eof()){
		getline(pstream,useless);
		getline(eStream,useless);
		getline(cStream,useless);
		getline(pstream,perfectRead);
		getline(eStream,erroneousRead);
		getline(cStream,correctedRead);
		for(uint i(0);i<perfectRead.size();++i){
			char p(perfectRead[i]);
			char e(erroneousRead[i]);
			char c(correctedRead[i]);
			if(p==e){
				//NO ERROR
				if(e!=c){
					//False positive
					++FP;
				}
			}else{
				//ERROR
				++errors;
				if(c==p){
					//true positive
					++TP;
				}else{
					if(c==e){
						//false negative
						++FN;
					}else{
						//False positive
						++FN;
					}
				}
			}
		}
	}
	cout<<"False positive, inserted errors"<<FP<<endl;
	cout<<"False Negative, non corrected errors"<<FN<<endl;
	cout<<"True Positive, corrected errors"<<TP<<endl;
	cout<<"Errors then "<<errors<<endl;
	cout<<"Errors now "<<FP+FN<<endl;

    return 0;
}
