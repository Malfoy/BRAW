#include <fstream>
#include <string.h>
#include <iostream>
#include <list>
#include "robin_hood.h"



using namespace std;



typedef robin_hood::unordered_flat_map<string,bool> Map;



string split_1(string chaine, char del ){
  string re ="";
  int size(chaine.size());
  int i(0);
  while(chaine[i] != del and i<size){
    re += chaine[i];
    i++;
  }
  return re;
}



int main(int argc, char** argv){
    if(argc<2){
        cout<<"[sequence name file] [fasta file]"<<endl;
        return 0;
    }
	string file_names_seq=argv[1];
	string file_fasta=argv[2];
	Map names_seq;

	ifstream flux(file_names_seq.c_str());
    string ligne;
	if(flux){
		ligne.clear();
		while(getline(flux, ligne)){
            if(ligne.size()>0){
			    names_seq[ligne]=true;
                // cout<<"insert"<<ligne<<"end"<<endl;
            }
		}
	}else{
		cout << "ERROR: Unable to open the file." << endl;
		exit(0);
	}
	flux.close();

	flux.open(file_fasta.c_str());
    string name_seq_fasta;
	if(flux){
		ligne.clear();
		bool found(false);
		while(getline(flux, ligne)){
            if(ligne.size()>0){
                if (ligne[0] == '>'){
                    found=false;
                    name_seq_fasta=split_1(ligne.substr(1, ligne.size()-1),' ');

                    if (names_seq[name_seq_fasta]) {
                        found=true;
                        // cout<<"FOUND"<<endl;
                        cout << ligne << '\n';
                    }else{
                        // cout<<"NOP"<<ligne<<"END"<<endl;
                    }
                }
                else{
                    if (found) {
                        cout << ligne << '\n';
                    }
                }
            }
		}
	}
	else{
		cout << "ERROR: Unable to open the file." << endl;
		exit(0);
	}
	flux.close();

  return 0;
}