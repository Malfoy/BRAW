#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <unordered_map>
#include <algorithm>



using namespace std;



void help(){
	cout<<"./correctionEvaluator perfectReads.fa erroneousReads.fa correctedReads.fa  "<<endl;
}

string intToString(uint64_t n){
	if(n<1000){
		return to_string(n);
	}
	string end(to_string(n%1000));
	if(end.size()==3){
		return intToString(n/1000)+","+end;
	}
	if(end.size()==2){
		return intToString(n/1000)+",0"+end;
	}
	return intToString(n/1000)+",00"+end;
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
	transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
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
	uint64_t TP(0),TN(0),FP(0),FN(0),errors(0),perfectReads(0),reads(0),nuc(0),readImproved(0);
	if( not (cStream.good() and eStream.good() and pstream.good()) ){
		cout<<"File problem"<<endl;
		return 0;
	}
	while(not cStream.eof() and not eStream.eof() and not pstream.eof()){
		reads++;
		//~ getline(pstream,useless);
		//~ getline(eStream,useless);
		//~ getline(cStream,useless);
		//~ getline(pstream,perfectRead);
		//~ getline(eStream,erroneousRead);
		//~ getline(cStream,correctedRead);
		erroneousRead=getLineFasta(&eStream);
		perfectRead=getLineFasta(&pstream);
		correctedRead=getLineFasta(&cStream);
		bool perfectlyCorrected(true);
		bool corrected(false);
		bool wrongCorrection(false);
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
					//~ cout<<"W";
					perfectlyCorrected=false;
				}else{
					++TN;
					//~ cout<<" ";
				}
			}else{
				//ERROR
				++errors;
				if(c==p){
					//true positive
					corrected=true;
					++TP;
					//~ cout<<" ";
				}else{
					if(c==e){
						//false negative
						perfectlyCorrected=false;
						++FN;
						//~ cout<<"n";
					}else{
						//False positive
						perfectlyCorrected=false;
						wrongCorrection=true;
						++FP;
						//~ cout<<"w";
					}
				}
			}
		}
		//~ cout<<endl;cin.get();
		if(perfectlyCorrected){
			perfectReads++;
		}
		if(corrected and not wrongCorrection){
			readImproved++;
		}
	}
	cout<<"False positive, inserted errors: ";
	//~ printInt(FP);
	cout<<intToString(FP)<<endl;
	cout<<"False Negative, non corrected errors: ";
	//~ printInt(FN);
	cout<<intToString(FN)<<endl;
	cout<<"True Positive, corrected errors: ";
	//~ printInt(TP);
	cout<<intToString(TP)<<endl;
	cout<<"Sensitivity: ";
	cout<<(double)(100*TP)/(TP+FN)<<endl;
	cout<<"Precision: ";
	cout<<(double)(100*TP)/(TP+FP)<<endl;
	cout<<"Specificity: ";
	cout<<(double)(100*TN)/(TN+FP)<<endl;
	cout<<"Errors then ";
	//~ printInt(errors);
	cout<<intToString(errors)<<endl;
	cout<<"Errors now ";
	//~ printInt(FP+FN);
	cout<<intToString(FP+FN)<<endl;
	cout<<"Error rate then: ";
	cout<<(double)(100*errors)/(nuc)<<endl;
	cout<<"Error rate now: ";
	cout<<(double)(100*(FP+FN))/(nuc)<<endl;
	cout<<"Ratio errors then/before:  "<<(double)errors/(FP+FN)<<endl;
	cout<<"Reads: ";
	//~ printInt(reads);
	cout<<intToString(reads)<<endl;
	cout<<"Erroneous reads: ";
	//~ printInt(reads-perfectReads);
	cout<<intToString(reads-perfectReads)<<endl;
	cout<<"Ratio erronenous reads:  "<<(double)100*(reads-perfectReads)/(reads)<<endl;
	cout<<"Reads improved: ";;
	//~ printInt(reads-perfectReads);
	cout<<intToString(readImproved)<<endl;

    return 0;
}
