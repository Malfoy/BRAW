#include <fstream>
#include <cstring>
#include <string>
#include <iostream>



using namespace std;



static unsigned int seed;


uint32_t xs(uint32_t& y){
    y^=(y<<13); y^=(y>>17);y=(y^=(y<<15)); return y;
}



char revCompChar(char c) {
    switch (c) {
        case 'A': return 'T';
        case 'C': return 'G';
        case 'G': return 'C';
    }
    return 'A';
}



string revComp(const string& s){
    string rc(s.size(),0);
    for (int i((int)s.length() - 1); i >= 0; i--){
        rc[s.size()-1-i] = revCompChar(s[i]);
    }
    return rc;
}



char randNucle(char c){
    //~ switch (rand()%4){
    switch (xs(seed)%4){
        case 0:
            if(c!='A'){
                return 'A';
            }
            return randNucle(c);
        case 1:
            if(c!='C'){
                return 'C';
            }
            return randNucle(c);
        case 2:
            if(c!='G'){
                return 'G';
            }
            return randNucle(c);
        case 3:
            if(c!='T'){
                return 'T';
            }
            return randNucle(c);
    }
    return randNucle(c);
}



int main(int argc, char ** argv){
    if(argc<7){
        cout<<"[Genome reference file] [read length] [fragment length] [coverage] [error rate] [prefix] [MP or PE]"<<endl;
        exit(0);
    }
    string input(argv[1]);
    uint length(stof(argv[2]));
    uint fragmentSize(stof(argv[3]));
    double coverage(stof(argv[4]));
    srand (time(NULL));
    ifstream in(input);
    uint errorRate(1/(stof(argv[5])));
    string prefix(argv[6]);
    string type(argv[7]);
    bool matePair(false);
    if(type=="MP"){
        matePair=true;
        cout<<"I produce mate pairs"<<endl;
    }else{
        cout<<"I produce paired ends"<<endl;
    }
    string useless, ref,read,pread;
    uint i(0);
    ofstream perfect("p."+prefix+".fa"),out(prefix+".fa");

    while(not in.eof()){
        seed=(rand());
        getline(in,useless);
        getline(in,ref);
        if(ref.size()<fragmentSize){
            break;
        }
        if(not ref.empty() and not useless.empty()){
            uint64_t nucProduced(0);
            while(nucProduced<coverage*ref.size()){
                if(i%100==0){
                    seed=(rand());
                }
                //produce a read
                uint position=xs(seed)%ref.size();
                if(position+fragmentSize<=ref.size()){
                    bool valid(true);
                    uint error(0);

                    pread=ref.substr(position,length);
                    read=pread;
                    for(uint i(0);i<read.size();++i){
                        if(read[i]=='N'){valid=false;break;}
                        if(xs(seed)%errorRate==0){
                            read[i]=randNucle(read[i]);
                            ++error;
                        }
                    }
                    if(valid){
                        //~ perfect<<">"+to_string(i)<<" "<<position<<endl;
                        //~ perfect<<pread<<endl;
                        out<<">"+to_string(i)<<"_"<<position<<"\n";
                        perfect<<">"+to_string(i)<<"_"<<position<<"\n";
                        if(matePair){
                            out<<revComp(read)<<"\n";
                            perfect<<revComp(pread)<<"\n";
                        }else{
                            out<<read<<"\n";
                            perfect<<(pread)<<"\n";
                        }
                        nucProduced+=read.size();
                        //~ ++i;
                    }

                    valid=(true);
                    error=(0);
                    pread=ref.substr(position+fragmentSize-length,length);
                    read=pread;
                    for(uint i(0);i<read.size();++i){
                        if(read[i]=='N'){valid=false;break;}
                        if(xs(seed)%errorRate==0){
                            read[i]=randNucle(read[i]);
                            ++error;
                        }
                    }
                    if(valid){
                        //~ perfect<<">"+to_string(i)<<"bis "<<(position+fragmentSize-length)<<endl;
                        //~ perfect<<pread<<endl;
                        out<<">"+to_string(i)<<"bis_"<<(position+fragmentSize-length)<<"\n";
                        perfect<<">"+to_string(i)<<"bis_"<<(position+fragmentSize-length)<<"\n";
                        if(not matePair){
                            out<<revComp(read)<<"\n";
                            perfect<<revComp(pread)<<"\n";
                        }else{
                            out<<read<<"\n";
                            perfect<<(pread)<<"\n";
                        }
                        nucProduced+=read.size();
                        ++i;
                    }
                }
            }
        }
    }

}
