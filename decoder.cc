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

    // Calc necessary paramters for decoding
    const size_t wr_size = filesize(argv[1]);
    const size_t B = wr_size - (N - 1) * std::ceil((double)wr_size / N);
    const size_t S = N * std::ceil((double)wr_size / N) - wr_size;
    const size_t orig_size = wr_size - (N - K) * (B+ S);

    // Initialize interleaver
    Interleaver<N, K> interleaver(argv[1], orig_size);
    std::array<char, N> block;

    OutputFileMapper mapper(output_filename, orig_size);
    
    // Initialize decoder
    ezpwd::RS<N, K> decoder;
    
    // Calc number of blocks with full size K
    size_t tail = orig_size % interleaver.width();
    
    // For each block
    for (size_t block_cnt = 0; block_cnt < B + S; ++block_cnt) {
      // Get data symbols
      auto x = interleaver.getBlock(block);
      
      // Calc start position of block in output file
      size_t offset = orig_size - tail + block_cnt;
      if (block_cnt < tail) 
        offset += interleaver.width();
      
      // Read parity symbols
      interleaver.read(block, N-K, K, offset, interleaver.width());
      
      // Fix errors
      decoder.decode(block);

      // Write to output file
      mapper.write(block, x, 0, block_cnt, B + S);
    }
    
    return 0; 
  
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
