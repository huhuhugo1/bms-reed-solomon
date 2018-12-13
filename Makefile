all: bms2A bms2B

bms2A: encoder.cc
	g++ -O3 -o $@ $^

bms2B: decoder.cc
	g++ -O3 --std=c++17 -o $@ $^

clean:
	rm -rf bms2A bms2B
