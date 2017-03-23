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

void printInt(const int value){
	string numWithCommas = to_string(value);
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition-=3;
	}
	cout<<numWithCommas<<endl;
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
	uint64_t TP(0),TN(0),FP(0),FN(0),errors(0),perfectReads(0),reads(0),nuc(0);
	while(not pstream.eof()){
		reads++;
		getline(pstream,useless);
		getline(eStream,useless);
		getline(cStream,useless);
		getline(pstream,perfectRead);
		getline(eStream,erroneousRead);
		getline(cStream,correctedRead);
		bool perfectlyCorrected(true);
		for(uint i(0);i<perfectRead.size();++i){
			++nuc;
			char p(perfectRead[i]);
			char e(erroneousRead[i]);
			char c(correctedRead[i]);
			if(p==e){
				//NO ERROR
				if(e!=c){
					//False positive
					++FP;
					perfectlyCorrected=false;
				}else{
					++TN;
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
						perfectlyCorrected=false;
						++FN;
					}else{
						//False positive
						perfectlyCorrected=false;
						++FP;
					}
				}
			}
		}
		if(perfectlyCorrected){
			perfectReads++;
		}
	}
	cout<<"False positive, inserted errors: ";
	printInt(FP);
	cout<<"False Negative, non corrected errors: ";
	printInt(FN);
	cout<<"True Positive, corrected errors: ";
	printInt(TP);
	cout<<"Sensitivity: ";
	cout<<(double)(100*TP)/(TP+FN)<<endl;
	cout<<"Specificity: ";
	cout<<(double)(100*TN)/(TN+FP)<<endl;
	cout<<"Errors then ";
	printInt(errors);
	cout<<"Errors now ";
	printInt(FP+FN);
	cout<<"Error rate then: ";
	cout<<(double)(100*errors)/(nuc)<<endl;
	cout<<"Error rate now: ";
	cout<<(double)(100*(FP+FN))/(nuc)<<endl;
	cout<<"Ratio errors then/before:  "<<(double)errors/(FP+FN)<<endl;
	cout<<"Reads: ";
	printInt(reads);
	cout<<"Erroneous reads: ";
	printInt(reads-perfectReads);
	cout<<"Ratio erronenous reads:  "<<(double)100*(reads-perfectReads)/(reads)<<endl;


    return 0;
}
