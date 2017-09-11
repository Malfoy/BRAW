#~ CC=/usr/bin/g++
CC=g++
CFLAGS=  -Wall -Wextra  -Ofast -std=c++11 -march=native -pthread -pipe
LDFLAGS=-pthread


ifeq ($(gprof),1)
CFLAGS=-std=c++0x -pg -O3  -march=native
LDFLAGS=-pg
endif

ifeq ($(valgrind),1)
CFLAGS=-std=c++0x -O3 -g
LDFLAGS=-g
endif


EXEC=refSimulator n50 fa2fq unitigEvaluator oneLine getLargeSequences split sequenceEvaluator fq2fa correctionEvaluator simulator interleaver pairedSimulator badvisor

all: $(EXEC)

interleaver:   interleaver.o
	$(CC) -o $@ $^ $(LDFLAGS)

badvisor:   badvisor.o
	$(CC) -o $@ $^ $(LDFLAGS)

badvisor.o: Badvisor.cpp
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
