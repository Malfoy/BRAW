#~ CC=/usr/bin/g++
CC=g++
CFLAGS= -Wall -Ofast -std=c++11  -flto -pipe -funit-at-a-time -fopenmp -lz -Isparsepp -flto
LDFLAGS=-flto -lpthread -fopenmp -lz  -Isparsepp  -flto



EXEC=downloadNCBI fredy massfqfa refSimulator n50 fa2fq unitigEvaluator unitigEvaluator_fast oneLine oneLineBreak getLargeSequences split sequenceEvaluator cluster2reads fq2fa correctionEvaluator simulator interleaver RC pairedSimulator badvisor fractionFile sortByHeader sort_by_size DBGSplitter getReadsFromHeader read_splitter clean_homopoly insertErrors random_sequence random_collection

all: $(EXEC)


downloadNCBI:   downloadNCBI.o
	$(CC) -o $@ $^ $(LDFLAGS)

downloadNCBI.o: downloadNCBI.cpp
	$(CC) -o $@ -c $< $(CFLAGS)
	
massfqfa:   massfqfa.o
	$(CC) -o $@ $^ $(LDFLAGS)

massfqfa.o: mass_fq2fa.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

sequence2kmer:   sequence2kmer.o
	$(CC) -o $@ $^ $(LDFLAGS)

sequence2kmer.o: sequence2kmer.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

random_sequence:   random_sequence.o
	$(CC) -o $@ $^ $(LDFLAGS)

random_collection.o: randomCollection.cpp
	$(CC) -o $@ -c $< $(CFLAGS)
	
random_collection:   random_collection.o
	$(CC) -o $@ $^ $(LDFLAGS)

random_sequence.o: randomSeq.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

fredy:   fredy.o
	$(CC) -o $@ $^ $(LDFLAGS)

fredy.o: fredy.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

insertErrors:   insertErrors.o
	$(CC) -o $@ $^ $(LDFLAGS)

insertErrors.o: InsertErrors.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

Harry_cover:   Harry_cover.o
	$(CC) -o $@ $^ $(LDFLAGS)

Harry_cover.o: Harry_Cover.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

read_splitter:   read_splitter.o
	$(CC) -o $@ $^ $(LDFLAGS)

read_splitter.o: read_splitter.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean_homopoly:   clean_homopoly.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean_homopoly.o: clean_homopoly.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

getReadsFromHeader:   getReadsFromHeader.o
	$(CC) -o $@ $^ $(LDFLAGS)

getReadsFromHeader.o: getReadsFromHeader.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

sort_by_size:   sort_by_size.o
	$(CC) -o $@ $^ $(LDFLAGS)

sort_by_size.o: sort_by_size.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

sort_PAF:   sort_PAF.o
	$(CC) -o $@ $^ $(LDFLAGS)

sort_PAF.o: sort_PAF.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

cluster2reads:   cluster2reads.o
	$(CC) -o $@ $^ $(LDFLAGS)

cluster2reads.o: cluster2reads.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

number2seq:   number2seq.o
	$(CC) -o $@ $^ $(LDFLAGS)

number2seq.o: number2seq.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

DBGSplitter:   DBGSplitter.o
	$(CC) -o $@ $^ $(LDFLAGS)

DBGSplitter.o: DBGSplitter.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

sortByHeader:   sortByHeader.o
	$(CC) -o $@ $^ $(LDFLAGS)

sortByHeader.o: SortByHeader.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

faToSeq:   faToSeq.o
	$(CC) -o $@ $^ $(LDFLAGS)

faToSeq.o: faToSeq.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

seqToFa:   seqToFa.o
	$(CC) -o $@ $^ $(LDFLAGS)

seqToFa.o: seqToFa.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

interleaver.o: interleaver.cpp zstr.hpp
	$(CC) -o $@ -c $< $(CFLAGS)

interleaver:  interleaver.o
	$(CC) -o $@ $^ $(LDFLAGS)

RC:   RC.o
	$(CC) -o $@ $^ $(LDFLAGS)

RC.o: RC.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

badvisor:   badvisor.o
	$(CC) -o $@ $^ $(LDFLAGS)

badvisor.o: Badvisor.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

fractionFile:   fractionFile.o
	$(CC) -o $@ $^ $(LDFLAGS)

fractionFile.o: fractionFile.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

refSimulator:   refSimulator.o
	$(CC) -o $@ $^ $(LDFLAGS)

refSimulator.o: refSimulator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

simulator:   simulator.o
	$(CC) -o $@ $^ $(LDFLAGS)

simulator.o: simulator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

pairedSimulator: pairedSimulator.o
	$(CC) -o $@ $^ $(LDFLAGS)

pairedSimulator.o: pairedSimulator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

oneLine:   oneLine.o
	$(CC) -o $@ $^ $(LDFLAGS)

oneLine.o: oneLine.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

oneLineBreak:   oneLineBreak.o
	$(CC) -o $@ $^ $(LDFLAGS)

oneLineBreak.o: oneLineBreak.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

split:   split.o
	$(CC) -o $@ $^ $(LDFLAGS)

split.o: split.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

getLargeSequences:   getLargeSequences.o
	$(CC) -o $@ $^ $(LDFLAGS)

getLargeSequences.o: getLargeSequences.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

unitigEvaluator:   unitigEvaluator.o
	$(CC) -o $@ $^ $(LDFLAGS)

unitigEvaluator.o: unitigEvaluator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

unitigEvaluator_fast:   unitigEvaluator_fast.o
	$(CC) -o $@ $^ $(LDFLAGS)

unitigEvaluator_fast.o: unitigEvaluator_fast.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

sequenceEvaluator:   sequenceEvaluator.o
	$(CC) -o $@ $^ $(LDFLAGS)

sequenceEvaluator.o: sequenceEvaluator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

fa2fq:   fa2fq.o
	$(CC) -o $@ $^ $(LDFLAGS)

fa2fq.o: fatofq.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

fq2fa:   fq2fa.o
	$(CC) -o $@ $^ $(LDFLAGS)

fq2fa.o: fq2fa.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

n50:   n50.o
	$(CC) -o $@ $^ $(LDFLAGS)

n50.o: N50.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

correctionEvaluator:   correctionEvaluator.o
	$(CC) -o $@ $^ $(LDFLAGS)

correctionEvaluator.o: correctionEvaluator.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf $(EXEC)


rebuild: clean $(EXEC)
