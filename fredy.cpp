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



/**
 * For each potential color, store the number of corresponding kmers
 */
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

			// enables to store in results once all kmers.
			// optimisable (eg with a set once initialy reading kmers)

			if (map[Hache].count(canon) != 0) { // should always be true (?pierre?)
				c = map[Hache][canon].first;
				done=map[Hache][canon].second;
				map[Hache][canon].second=true; // validate if this kmer has already been seen
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


/**
 * prints the cardinality of each color
 */
void evaluate_completness(const vector<uint64_t>& cardinalities, const vector<uint64_t>& venn, int size_result, const string& venout_file_name = "venn_out.txt") {

	ofstream out(venout_file_name);
	if (not out.good()) {
		cout << "Problem opening file:" << venout_file_name << " for writing the venn resutls"<<endl;
		exit(1);
	}

	vector<uint64_t> counted(cardinalities.size());
	cout<<"Venn:	"<<endl;
	out<<"#Venn:	"<<endl;
	for (uint64_t i(0); i < venn.size(); ++i) {
		string colors = get_color_code(i,size_result);
		cout<<colors<<" "<<intToString(venn[i])<<endl;
		out<<colors<<" "<<venn[i]<<endl;
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
	out.close();
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



int64_t error_in_contigs_kmers(const string& ref,Map map[]){
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



int64_t error_in_contigs_positions(const string& ref,Map map[]){
	int64_t result(0);
	kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	color c(0);
	uint j(0);
	if (map[Hache].count(canon) != 0) {
		c = map[Hache][canon].first;
		if (c == 0) {
			cout<<"SHOULD NOT HAPPEN WTF"<<endl;
		}
	}else{
		result++;
		j+=k;
		if(j+1+k>=ref.size()){
			return result;
		}
		seq=str2num(ref.substr(j+1,k));
		rcSeq=rcb(seq, k);
		canon=min(seq,rcSeq);
	}
	for (; j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			c = map[Hache][canon].first;
			if (c ==0) {
				cout<<"SHOULD NOT HAPPEN WTF"<<endl;
			}
		}else{
			result++;
			j+=k;
			if(j+1+k>=ref.size()){
				return result;
			}
			seq=str2num(ref.substr(j+1,k));
			rcSeq=rcb(seq, k);
			canon=min(seq,rcSeq);
		}
	}
	return result;
}



int64_t get_major_color(const string& ref,Map map[]){
	vector<int> color_count(color_number,0);
	int64_t total(0);
	kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	string colorstr;
	if (map[Hache].count(canon) != 0) {
		colorstr=get_color_code( map[Hache][canon].first);
		for(int i(0);i<color_number;++i){
			if(colorstr[i]=='1'){
				color_count[i]++;
			}
		}
		total++;
	}
	for (uint j(0); j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			colorstr=get_color_code( map[Hache][canon].first);
			for(int i(0);i<color_number;++i){
				if(colorstr[i]=='1'){
					color_count[i]++;
				}
			}
		}
		total++;
	}
	int64_t max(0),max_id(0);
	for(int i(0);i<color_number;++i){
		if(color_count[i]>max){
			max=color_count[i];
			max_id=i;
		}
	}
	return max_id;
}



int64_t phasing_error_in_contigs_positions(const string& ref,Map map[]){
	vector<int> color_count(color_number,0);
	int result(0);
	int64_t mc(get_major_color(ref,map));
	kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	uint j(0);
	if (map[Hache].count(canon) != 0) {
		color c = map[Hache][canon].first;
		if(not is_set(mc,c)){
			++result;
			j+=k;
			if(j+1+k>=ref.size()){
				return result;
			}
			seq=str2num(ref.substr(j+1,k));
			rcSeq=rcb(seq, k);
			canon=min(seq,rcSeq);;
		}
	}
	for (; j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			color c = map[Hache][canon].first;
			if(not is_set(mc,c)){
				++result;
				j+=k;
				if(j+1+k>=ref.size()){
					return result;
				}
				seq=str2num(ref.substr(j+1,k));
				rcSeq=rcb(seq, k);
				canon=min(seq,rcSeq);
			}
		}
	}
	return result;
}




int64_t phasing_error_in_contigs_kmers(const string& ref,Map map[]){
	vector<int> color_count(color_number,0);
	int64_t total(0);
	kmer seq(str2num(ref.substr(0, k))), rcSeq(rcb(seq, k)), canon(min(seq, rcSeq));
	uint Hache(hash64shift(canon) % 16);
	string colorstr;
	if (map[Hache].count(canon) != 0) {
		colorstr=get_color_code( map[Hache][canon].first);
		for(int i(0);i<color_number;++i){
			if(colorstr[i]=='1'){
				color_count[i]++;
			}
		}
		total++;
	}
	for (uint j(0); j + k < ref.size(); ++j) {
		updateK(seq, ref[j + k]);
		updateRCK(rcSeq, ref[j + k]);
		canon = (min(seq, rcSeq));
		uint Hache(hash64shift(canon) % 16);
		if (map[Hache].count(canon) != 0) {
			colorstr=get_color_code( map[Hache][canon].first);
			for(int i(0);i<color_number;++i){
				if(colorstr[i]=='1'){
					color_count[i]++;
				}
			}
		}
		total++;
	}
	return total - *max_element(color_count.begin(),color_count.end());
}



void count_break_and_errors(Map map[], const string& file_name) {
	uint64_t broke_contigs(0);
	uint64_t breaks(0);
	uint64_t Erroneous_contigs(0);
	uint64_t errors(0);
	uint64_t phasing_errors(0);
	uint64_t perfect_contigs(0);
	uint64_t perfect_contigs_size(0);
	uint64_t total_contigs(0);
	uint64_t total_nuc(0);
	vector<uint64_t> distrib_breaks(11);
	vector<uint64_t> distrib_errors(11);
	vector<uint64_t> distrib_phasing_errors(11);
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
			// int64_t local_errors=error_in_contigs(ref,map);
			int64_t local_errors=error_in_contigs_positions(ref,map);
			int64_t local_phasing_errors=phasing_error_in_contigs_positions(ref,map);
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
					Erroneous_contigs++;
				}else{
					if(local_phasing_errors==0 and local_errors==0){
						perfect_contigs++;
						perfect_contigs_size+=ref.size();
					}else{
						Erroneous_contigs++;
					}
				}
				if(local_breaks>=distrib_breaks.size()){
					distrib_breaks[distrib_breaks.size()-1]++;
				}else{
					distrib_breaks[local_breaks]++;
				}

				errors+=local_errors;
				if(local_errors>=(int)distrib_errors.size()){
					distrib_errors[distrib_errors.size()-1]++;
				}else{
					distrib_errors[local_errors]++;
				}
				phasing_errors+=local_phasing_errors;
				if(local_phasing_errors>=(int)distrib_phasing_errors.size()){
					distrib_phasing_errors[distrib_phasing_errors.size()-1]++;
				}else{
					distrib_phasing_errors[local_phasing_errors]++;
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
	cout<<"Sequencing errors distribution"<<endl;
	for(uint i(0);i<distrib_errors.size()-1;++i){
		if(distrib_errors[i]!=0){
			cout<<intToString(distrib_errors[i])<<"		contigs have	"<<i<<" sequencing errors"<<endl;
		}
	}
	if(distrib_errors[distrib_errors.size()-1]!=0){
		cout<<intToString(distrib_errors[distrib_errors.size()-1])<<"		contigs have	"<<distrib_errors.size()-1<<" sequencing errors(OR MORE)"<<endl;
	}

	cout<<"Phasing Errors distribution"<<endl;
	for(uint i(0);i<distrib_phasing_errors.size()-1;++i){
		if(distrib_phasing_errors[i]!=0){
			cout<<intToString(distrib_phasing_errors[i])<<"		contigs have	"<<i<<" phasing errors"<<endl;
		}
	}
	if(distrib_phasing_errors[distrib_phasing_errors.size()-1]!=0){
		cout<<intToString(distrib_phasing_errors[distrib_phasing_errors.size()-1])<<"		contigs have	"<<distrib_errors.size()-1<<" phasing errors(OR MORE)"<<endl;
	}

	cout<<"Perfect contigs stats"<<endl;
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
	evaluate_completness(cardinalities, venn, cardinalities.size());
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
