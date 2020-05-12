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


void print_color(color c, int n){
	for(int i(0);i<n;++i){
		cout<<c%2;
		c>>=1;
	}
	cout<<" "<<flush;
}



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
		if (ref.size()>=(uint)k) {
			kmer seq(str2num(ref.substr(0, k)));
			kmer rcSeq(rcb(seq, k));
			kmer canon(min(seq, rcSeq));
			uint Hache(hash64shift(canon) % 16);
			color c(0);
			bool done=false;
			nutex[Hache].lock();
			if (map[Hache].count(canon) != 0) {
				c = map[Hache][canon].first;
				done=map[Hache][canon].second;
				map[Hache][canon].second=true;
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
					c = map[Hache][canon].first;
					done=map[Hache][canon].second;
					map[Hache][canon].second=true;
				}else{
					c=0;
					done=false;
				}
				nutex[Hache].unlock();
				if(not done){
					#pragma omp atomic
					++result[c];
				}
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
		cout<<" "<<intToString(venn[i])<<endl;
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
		cout<<"Kmers  found from file:	" << i << "	" <<intToString(counted[i])  << endl;
		// cout<<"Card  of file:	" << i << "	" << cardinalities[i]  << endl;
		cout<<"Completness % for file:	" << i << "	" << (double)100 * counted[i] / cardinalities[i] << endl;
	}
}


int64_t contig_break(const string& ref, int64_t start_position,Map map[]){
	kmer seq(str2num(ref.substr(start_position, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	color c(0);
	color minimal_color(-1);
	if (map[Hache].count(canon) != 0) {
		c = map[Hache][canon].first;
		if (c != 0) {
			minimal_color&=c;
		}
	}
	for (uint j(start_position); j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			c = map[Hache][canon].first;
		}
		if (c != 0 and c!=minimal_color) {
			minimal_color&=c;
			if(minimal_color==0){
				return (int)j+1;
			}else{
			}
		}
	}
	return -1;
}



int64_t error_in_contigs(const string& ref,Map map[]){
	int64_t result(0);
	kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	color c(0);
	if (map[Hache].count(canon) != 0) {
		c = map[Hache][canon].first;
		if (c == 0) {
			result++;
		}
	}
	for (uint j(0); j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			c = map[Hache][canon].first;
		}
		if (c ==0) {
			result++;
		}
	}
	return result;
}



void count_break_and_errors(Map map[], const string& file_name) {
	uint64_t broke_contigs(0);
	uint64_t breaks(0);
	uint64_t Erroneous_contigs(0);
	uint64_t errors(0);
	uint64_t perfect_contigs(0);
	uint64_t perfect_contigs_size(0);
	uint64_t total_contigs(0);
	uint64_t total_nuc(0);
	vector<uint64_t> distrib_breaks(11);
	vector<uint64_t> distrib_errors(11);
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
		if (ref.size()>(uint)k) {
			total_contigs++;
			total_nuc+=ref.size();
			int64_t local_errors=error_in_contigs(ref,map);
			bool broke(false);
			uint64_t local_breaks(0);
			int64_t position(0);
			while(position>=0){
				position=contig_break(ref,position, map);
				if(position>0){
					broke=true;
					local_breaks++;
					position+=1;
				}
			}
			#pragma omp critical(update)
			{
				if(broke){
					broke_contigs++;
					breaks+=local_breaks;
				}else{
					if(local_errors==0){
						perfect_contigs++;
						perfect_contigs_size+=ref.size();
					}
				}
				if(local_breaks>=distrib_breaks.size()){
					distrib_breaks[distrib_breaks.size()-1]++;
				}else{
					distrib_breaks[local_breaks]++;
				}
				Erroneous_contigs++;
				errors+=local_errors;
				if(local_errors>=(int)distrib_errors.size()){
					distrib_errors[distrib_errors.size()-1]++;
				}else{
					distrib_errors[local_errors]++;
				}

			}
		}
	}
	cout<<"\nContigs with breaks:	"<<intToString(broke_contigs)<<"	Phasing breaks (total):	" << intToString(breaks)<< endl;
	cout<<"Breaks distribution"<<endl;

	for(uint i(0);i<distrib_breaks.size()-1;++i){
		if(distrib_breaks[i]!=0){
			cout<<intToString(distrib_breaks[i])<<"		contigs have	"<<intToString(i)<<"	phase breaks"<<endl;
		}
	}
	if(distrib_breaks[distrib_breaks.size()-1]!=0){
		cout<<intToString(distrib_breaks[distrib_breaks.size()-1])<<"		contigs have	"<<intToString(distrib_breaks.size()-1)<<"	phase breaks (OR MORE)"<<endl;
	}

	cout<<"\nContigs with errors:	"<<intToString(Erroneous_contigs)<<"	Errors (total):	" << intToString(errors) << endl;
	cout<<"Errors distribution"<<endl;
	for(uint i(0);i<distrib_errors.size()-1;++i){
		if(distrib_errors[i]!=0){
			cout<<intToString(distrib_errors[i])<<"		contigs have	"<<i<<" sequencing errors"<<endl;
		}
	}
	if(distrib_errors[distrib_errors.size()-1]!=0){
		cout<<intToString(distrib_errors[distrib_errors.size()-1])<<"		contigs have	"<<distrib_errors.size()-1<<" sequencing errors(OR MORE)"<<endl;
	}
	cout<<intToString(perfect_contigs)<<"	perfect contigs totalling "<<intToString(perfect_contigs_size)<<" bases"<<endl;
	cout<<intToString(perfect_contigs)<<"	out of "<<intToString(total_contigs)<<" mean "<<(double)100*perfect_contigs/total_contigs<<" % perfect contigs"<<endl;
	cout<<intToString(perfect_contigs_size)<<"	bases out of  "<<intToString(total_nuc)<<" mean "<<(double)100*perfect_contigs_size/total_nuc<<" % perfect contigs in bases"<<endl;
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
	vector<uint64_t> venn(Venn_evaluation(map, inputRef,1<<(cardinalities.size())));
	cout<<"Completness EVALUATION"<<endl;
	evaluate_completness(cardinalities, venn);
	cout<<"BREAKS EVALUATION"<<endl;
	count_break_and_errors(map, inputRef);
	// cout<<"Contigs with breaks:	"<<intToString(breaks.first)<<"	Phasing breaks (total):	" << intToString(breaks.second )<< endl;
	// cout<<"ERRORS EVALUATION"<<endl;
	// auto errors(count_errors(map, inputRef));
	// cout<<"Contigs with errors:	"<<intToString(errors.first)<<"	Errors (total):	" << intToString(errors.second) << endl;
	auto end                                 = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	time_t end_time                          = chrono::system_clock::to_time_t(end);

	cout << "\nFinished computation at " << ctime(&end_time) << "Elapsed time: " << intToString(elapsed_seconds.count()) << "s\n";
}
