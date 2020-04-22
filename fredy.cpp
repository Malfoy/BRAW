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
typedef robin_hood::unordered_flat_map<kmer, pair<color,bool>> Map;



int max_color(8);
int k(31);
kmer offsetUpdateAnchors = 1;
array<mutex, 1024> nutex;


void set_color(color& c, int indice) {
	c += (1 << indice);
}



bool is_set(int indice, color c) {
	c >>= indice;
	return c % 2 == 1;
}


void print_color(color c, int n){
	for(int i(0);i<n;++i){
		cout<<c%2;
		c>>=1;
	}
	cout<<" "<<flush;
}



bool is_included(color c1, color c2) {
	// print_color(c2,8);
	// print_color(c1,8);
	for (int i(0); i < max_color; ++i) {
		if (c2 % 2 == 0 and c1 % 2 == 1) {
			// cout<<"false"<<endl;
			return false;
		}
		c2 >>= 1;
		c1 >>= 1;
	}
	// cout<<"true"<<endl;
	return true;
}



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



void updateK(kmer& min, char nuc) {
	min <<= 2;
	min += nuc2int(nuc);
	min %= offsetUpdateAnchors;
}



void updateRCK(kmer& min, char nuc) {
	min >>= 2;
	min += (nuc2intrc(nuc) << (2 * (k - 1)));
}



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





uint64_t load_reference(Map map[], const string file_name, int reference_number) {
	uint64_t result(0);
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
			getline(in, useless);
			getline(in, ref);
		}
		if (not ref.empty()) {
			kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
			uint Hache(hash64shift(canon) % 16);
			nutex[Hache].lock();
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
				set_color(map[Hache][canon].first, reference_number);
				nutex[Hache].unlock();
#pragma omp atomic
				result++;
			}
		}
	}
	return result;
}



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
		}
		reference_number++;
	}
	return result;
}



vector<uint64_t> Venn_evaluation(Map map[], const string& file_name, int size_result) {
	vector<uint64_t> result(size_result);
	zstr::ifstream in(file_name);
	if (not in.good()) {
		cout << "Problem with ref file opening:" << file_name << endl;
		exit(1);
	}
	#pragma omp parallel
	while (not in.eof()) {
		string ref, useless;
		#pragma omp critical(file_ref)
		{
			getline(in, useless);
			getline(in, ref);
		}
		if (not ref.empty()) {
			// cout<<"go line"<<endl;
			kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
			uint Hache(hash64shift(canon) % 16);
			color c(0);
			bool done;
			nutex[Hache].lock();
			if (map[Hache].count(canon) != 0) {
				// cout<<"go kmer fund"<<endl;
				c = map[Hache][canon].first;
				done=map[Hache][canon].second;
				map[Hache][canon].second=true;
			}else{
					// cout<<"go kmerNOT fund"<<endl;
				c=0;
			}
			nutex[Hache].unlock();
			if(not done){
				#pragma omp atomic
				++result[c];
			}

			for (uint j(0); j + k < ref.size(); ++j) {
				updateK(seq, ref[j + k]);
				updateRCK(rcSeq, ref[j + k]);
				canon = (min(seq, rcSeq));
				uint Hache(hash64shift(canon) % 16);

				nutex[Hache].lock();
				if (map[Hache].count(canon) != 0) {
					// cout<<"go kmer fund"<<endl;
					c = map[Hache][canon].first;
					done=map[Hache][canon].second;
					map[Hache][canon].second=true;
				}else{
					// cout<<"go kmerNOT fund"<<endl;
					c=0;
				}
				nutex[Hache].unlock();
				if(not done){
					#pragma omp atomic
					++result[c];
				}
				// cout<<"THE END"<<endl;
			}
		}
	}
	return result;
}



void evaluate_completness(const vector<uint64_t>& cardinalities, const vector<uint64_t>& venn) {
	vector<uint64_t> counted(cardinalities.size());
	cout<<"Venn:	"<<endl;
	for (uint64_t i(0); i < venn.size(); ++i) {
		// print_color(i,2);
		cout<<" "<<venn[i]<<endl;
		uint64_t i_bin(i);
		uint64_t id(0);
		while (i_bin != 0) {
			if (i_bin % 2 == 1) {
				counted[id] += venn[i];
			}
			i_bin >>= 1;
			++id;
		}
	}
	for (uint64_t i(0); i < counted.size(); ++i) {
		cout<<"Kmers  found from file:	" << i << "	" <<counted[i]  << endl;
		// cout<<"Card  of file:	" << i << "	" << cardinalities[i]  << endl;
		cout<<"Completness % for file:	" << i << "	" << 100 * counted[i] / cardinalities[i] << endl;
	}
}



uint64_t count_break(Map map[], const string& file_name) {
	uint64_t result;
	zstr::ifstream in(file_name);
	if (not in.good()) {
		cout << "Problem with ref file opening:" << file_name << endl;
		exit(1);
	}
	// #pragma omp parallel
	while (not in.eof()) {
		string ref, useless;
#pragma omp critical(file_ref)
		{
			getline(in, useless);
			getline(in, ref);
		}
		color minimal_color(-1);
		// print_color(minimal_color,2);
		// cout<<endl;
		if (not ref.empty()) {
			kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
			uint Hache(hash64shift(canon) % 16);
			color c(0);
			nutex[Hache].lock();
			if (map[Hache].count(canon) != 0) {
				c = map[Hache][canon].first;
			}
			nutex[Hache].unlock();
			if (c != 0 and c!=minimal_color) {
				minimal_color&=c;
				if(minimal_color==0){
					++result;
					continue;
				}
				// if (is_included(c, minimal_color)) {
				// 	minimal_color = c;
				// 	// print_color(minimal_color,2);
				// 	// cout<<endl;
				// } else {
				//
				// 	if (not is_included( minimal_color,c)) {
				// 		cout<<"STOP"<<endl;
				// 		print_color(c,8);
				// 		print_color(minimal_color,8);
				// 		cout<<endl;
				// 		cin.get();
				// 		++result;
				// 		continue;
				// 	}
				// }
			}
			for (uint j(0); j + k < ref.size(); ++j) {
				updateK(seq, ref[j + k]);
				updateRCK(rcSeq, ref[j + k]);
				canon = (min(seq, rcSeq));
				uint Hache(hash64shift(canon) % 16);

				nutex[Hache].lock();
				if (map[Hache].count(canon) != 0) {
					c = map[Hache][canon].first;
				}
				nutex[Hache].unlock();
				if (c != 0 and c!=minimal_color) {
					minimal_color&=c;
					if(minimal_color==0){
						++result;
						break;
					}
					// if (is_included(c, minimal_color)) {
					// 	minimal_color = c;
					// } else {
					// 	if (not is_included( minimal_color,c)) {
					// 		cout<<"STOP"<<endl;
					// 		print_color(c,8);
					// 		print_color(minimal_color,8);
					// 		cout<<endl;
					// 		cin.get();
					// 		++result;
					// 		break;
					// 	}
					// }
				}
			}
		}
	}
	return result;
}



int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "[Reference file of file] [query file] (kmer size) (perform 2^n pass) " << endl;
		exit(0);
	}
	if (argc > 3) {
		k = (stoi(argv[3]));
	}
	offsetUpdateAnchors <<= (2 * (k));
	auto start = chrono::system_clock::now();
	string inputFILE(argv[1]);
	string inputRef(argv[2]);

	Map map[16];
	cout<<"LOAD REFERENCES"<<endl;
	vector<uint64_t> cardinalities(load_reference_file(map, inputFILE));
	cout<<"VENN EVALUATION"<<endl;
	vector<uint64_t> venn(Venn_evaluation(map, inputRef,1<<cardinalities.size()));
	cout<<"Completness EVALUATION"<<endl;
	evaluate_completness(cardinalities, venn);
	cout<<"BREAKS EVALUATION"<<endl;
	uint64_t breaks(count_break(map, inputRef));
	cout<<"Phasing breaks:	" << breaks << endl;
	auto end                                 = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	time_t end_time                          = chrono::system_clock::to_time_t(end);

	cout << "\nFinished computation at " << ctime(&end_time) << "Elapsed time: " << elapsed_seconds.count() << "s\n";
}
