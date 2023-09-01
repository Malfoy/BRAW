#include "robin_hood.h"
#include "strict_fstream.hpp"
#include "zstr.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>



using namespace std;



typedef uint64_t kmer;
typedef uint8_t color;
// key = kmer, value = pair (color: genomes where the kmer occurs. Max 8 genomes, boolean: has been seen (for feeding venn diagrams))
typedef robin_hood::unordered_flat_map<kmer, pair<color,bool>> Map;


// severe limitation here, todo: authorize more than 8 colors.
int max_color(8);
int color_number(0);
int k(31);
kmer offsetUpdateAnchors = 1;
array<mutex, 1024> nutex;



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


void set_color(color& c, int indice) {
	c |= (1 << indice);
}



bool is_set(int indice, color c) {
	c >>= indice;
	return c % 2 == 1;
}


string get_color_code(color c, int n=color_number){
	string res="";
	for(int i(0);i<n;++i){
		// cout<<c%2;
		if (c%2) res+= '1';
		else  res+= '0';
		c>>=1;
	}
	return res;
}


string print_color(color c, int n){
	string res="";
	for(int i(0);i<n;++i){
		cout<<c%2;
		res += c%2;
		c>>=1;
	}
	cout<<" "<<flush;
	return res;
}


// checks if c2 is included in c1. "included" means that all 1's in c1 are also in c2.
// this is not the case on this example as the second bit is 1 in c1 and 0 in c2
// 01010110 c1
// 00110111 c2
bool is_included(color c1, color c2) {
	for (int i(0); i < max_color; ++i) {
		if (c2 % 2 == 0 and c1 % 2 == 1) {
			return false;
		}
		c2 >>= 1;
		c1 >>= 1;
	}
	return true;
}



// optimisable (see eg BankBinary from GATB)
kmer str2num(const string& str) {
	kmer res(0);
	for (uint64_t i(0); i < str.size(); i++) {
		res <<= 2;
		switch (str[i]) {
			case 'A':
				res += 0;
				break;
			case 'C':
				res += 1;
				break;
			case 'G':
				res += 2;
				break;
			default:
				res += 3;
				break;
		}
	}
	return res;
}


/**
 * Reverse complement of a kmer.
 */
kmer rcb(kmer min, uint n) {
	kmer res(0);
	kmer offset(1);
	offset <<= (2 * n - 2);
	for (uint i(0); i < n; ++i) {
		res += (3 - (min % 4)) * offset;
		min >>= 2;
		offset >>= 2;
	}
	return res;
}



kmer nuc2int(char c) {
	switch (c) {
		//~ case 'A': return 0;
		case 'C':
			return 1;
		case 'G':
			return 2;
		case 'T':
			return 3;
	}
	return 0;
}



kmer nuc2intrc(char c) {
	switch (c) {
		case 'A':
			return 3;
		case 'C':
			return 2;
		case 'G':
			return 1;
			//~ case 'T': return 0;
	}
	return 0;
}


/**
 * Add a nucleotide to an already computed kmer
 */
void updateK(kmer& min, char nuc) {
	min <<= 2;
	min += nuc2int(nuc);
	min %= offsetUpdateAnchors;
}


/**
 * Add a nucleotide to an already computed reverse complement of a kmer
 */
void updateRCK(kmer& min, char nuc) {
	min >>= 2;
	min += (nuc2intrc(nuc) << (2 * (k - 1)));
}


/**
 * hash value for a kmer
 */
kmer hash64shift(kmer key) {
	key = (~key) + (key << 21); // key = (key << 21) - key - 1;
	key = key ^ (key >> 24);
	key = (key + (key << 3)) + (key << 8); // key * 265
	key = key ^ (key >> 14);
	key = (key + (key << 2)) + (key << 4); // key * 21
	key = key ^ (key >> 28);
	key = key + (key << 31);
	return key;
}




/**
 * Index all canonical kmers from the fasta file (file_name)
 * map then contains for each hashed kmer, the canonical value and its color, updated with this reference_number
 * returns the number of read kmers.
 */
uint64_t load_reference(Map map[], const string file_name, int reference_number) {
	uint64_t result(0);// nb indexed kmers for this reference.
	if (reference_number >= max_color) {
		cout << "Too much references, max is: " << max_color << endl;
		cout << "I ignore " << file_name << endl;
		return 0;
	}
	zstr::ifstream in(file_name);
	if (not in.good()) {
		cout << "Problem with ref file opening:" << file_name << endl;
		return 0;
	}
	#pragma omp parallel
	while (not in.eof()) {
		string ref, useless;
#pragma omp critical(file_ref)
		{
			getline(in, useless);   // read a comment, useless
			getline(in, ref);		// read the ACGT sequence
		}
		if (not ref.empty()) {
			// read all kmers from the ref sequence
			kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
			uint Hache(hash64shift(canon) % 16);
			nutex[Hache].lock();
			// for this kmer, set its indexed value adding the reference_number
			set_color(map[Hache][canon].first, reference_number);
			nutex[Hache].unlock();
#pragma omp atomic
			result++;
			for (uint j(0); j + k < ref.size(); ++j) {
				updateK(seq, ref[j + k]);
				updateRCK(rcSeq, ref[j + k]);
				canon = (min(seq, rcSeq));
				uint Hache(hash64shift(canon) % 16);
				nutex[Hache].lock();
				// for this kmer, set its indexed value adding the reference_number
				set_color(map[Hache][canon].first, reference_number);
				nutex[Hache].unlock();
#pragma omp atomic
				result++;
			}
		}
	}
	return result;
}



/**
 * Read the file of file referencing fasta files
 * Returns a vector of the number of read kmers per file
 */
vector<uint64_t> load_reference_file(Map map[], const string& file_name) {
	vector<uint64_t> result;
	zstr::ifstream in(file_name);
	if (not in.good()) {
		cout << "Problem with ref file opening:" << file_name << endl;
		exit(1);
	}
	string ref_file;
	int reference_number(0);
	while (not in.eof()) {
		getline(in, ref_file);
		if (ref_file.size() > 1) {
			result.push_back(load_reference(map, ref_file, reference_number));
			color_number++;
		}
		reference_number++;
	}
	return result;
}



void color_union_graph(const string& file_name,Map map[],const string& outfile){
    zstr::ofstream out(outfile);
	zstr::ifstream in(file_name);
	if (not in.good()) {
		cout << "Problem with ref file opening:" << file_name << endl;
		return;
	}
	#pragma omp parallel
    {
        string ref, useless,color;
        while (not in.eof()) {
            
            #pragma omp critical(file_ref)
            {
                getline(in, useless);   // read a comment, useless
                getline(in, ref);		// read the ACGT sequence
            }
            color="";
            uint8_t only_color;
            bool monocolor(true);
            if (not ref.empty()) {
                // read all kmers from the ref sequence
                kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
                uint Hache(hash64shift(canon) % 16);
                if (map[Hache].count(canon) != 0) {
                    only_color=map[Hache][canon].first;
                        color+=to_string(only_color);
                    }		
                

                for (uint j(0); j + k < ref.size(); ++j) {
                    updateK(seq, ref[j + k]);
                    updateRCK(rcSeq, ref[j + k]);
                    canon = (min(seq, rcSeq));
                    uint Hache(hash64shift(canon) % 16);
                    if (map[Hache].count(canon) != 0) {
                        uint8_t c(map[Hache][canon].first);
                        color+=","+to_string(c);
                        if(c!=only_color){
                            monocolor=false;
                        }
                    }	
                }
            }
            #pragma omp critical(out)
            {
                if(monocolor){
                    out<<useless<<" C:"<<to_string(only_color)<<"\n"<<ref<<"\n";
                }else{
                    out<<useless<<" C:"<<color<<"\n"<<ref<<"\n";
                }
            }
        }
    }
}



int main(int argc, char** argv) {
	if (argc < 5) {
                cout << "[Reference file of file] [union file] [outfile] [kmer size]" << endl;
		exit(0);
	}
    k = (stoi(argv[4]));
	offsetUpdateAnchors <<= (2 * (k));
	auto start = chrono::system_clock::now();
	string inputFILE(argv[1]);
	string inputRef(argv[2]);
    string output(argv[3]);
    

	Map map[16];
	cout<<"LOAD REFERENCES"<<endl;
	vector<uint64_t> cardinalities(load_reference_file(map, inputFILE));
    cout<<"COLORING"<<endl;
    color_union_graph(inputRef,map,output);
	
	auto end                                 = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	time_t end_time                          = chrono::system_clock::to_time_t(end);

	cout << "\nFinished computation at " << ctime(&end_time) << "Elapsed time: " << intToString(elapsed_seconds.count()) << "s\n";
}
