all: bms2A bms2B

bms2A: encoder.cc interleaver.tt rs rs_base
	g++ -O3 -o $@ encoder.cc

bms2B: decoder.cc interleaver.tt rs rs_base
	g++ -O3 --std=c++17 -o $@ decoder.cc

clean:
	rm -rf bms2A bms2B
