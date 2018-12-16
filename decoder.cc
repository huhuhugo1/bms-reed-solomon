#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

// External library from https://github.com/pjkundert/ezpwd-reed-solomon/
#include "rs"

#include "interleaver.tt"
#include "outputFileMapper.tt"

std::ifstream::pos_type filesize(const std::string& filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

constexpr int N = 255;
constexpr int K = 147;

int main(int argc,char *argv[]) {
  try {
    // Check arguments
    if (argc != 2)
      throw std::runtime_error("[Decoder] Missing arguments!");

    // Prepare output file name
    std::string output_filename = argv[1];
    output_filename += ".ok";

    const size_t wr_size = filesize(argv[1]);
    const size_t B = wr_size - (N - 1) * std::ceil((double)wr_size / N);
    const size_t S = N * std::ceil((double)wr_size / N) - wr_size;
    const size_t orig_size = wr_size - (N - K) * (B+ S);

    Interleaver<N, K> interleaver(argv[1], orig_size);
    std::array<char, N> block;

    OutputFileMapper mapper(output_filename, orig_size);
    ezpwd::RS<N, K> decoder;
    size_t tail = orig_size % interleaver.NUM_OF_BLOCKS;
    for (size_t i = 0; i < B + S; ++i) {
      auto x = interleaver.getBlock(block);
      for (size_t j = 0; j < N-K; ++j) {
        size_t idx = orig_size + j * interleaver.NUM_OF_BLOCKS + i - tail;
        if (i < tail) idx += interleaver.NUM_OF_BLOCKS;
        block[K+j] = interleaver.FILE.CONTENT[idx];
      }
      decoder.decode(block);
      mapper.write(block, x, 0, i, B + S);
    }
    
    return 0; 
  
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
