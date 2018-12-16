#include <string>

// External library from https://github.com/pjkundert/ezpwd-reed-solomon/
#include "rs"

#include "interleaver.tt"
#include "outputFileMapper.tt"

// Size of ReedSolomon block
constexpr size_t N = 255;
// Number of non parity symbols per block
constexpr size_t K = 147;

int main(int argc, char *argv[]) {
  try {  
    // Check arguments
    if (argc != 2)
      throw std::runtime_error("[Encoder] Missing arguments!");

    // Prepare output file name
    std::string output_filename = argv[1];
    output_filename += ".out";
    
    // Initialize interleaver
    Interleaver<N, K> interleaver(argv[1]);
    std::array<char, N> block;

    // Calc size of encoded file
    size_t out_size = interleaver.size() + interleaver.width() * (N - K);
    OutputFileMapper mapper(output_filename, out_size);
    mapper.write(interleaver.data(), interleaver.size());

    // Initialize encoder
    ezpwd::RS<N, K> encoder;

    // Calc number of blocks with full size K
    size_t tail = interleaver.size() % interleaver.width();

    // For each block
    for (size_t block_cnt = 0; interleaver.getBlock(block); ++block_cnt) {
      // Add parity symbols
      encoder.encode(block);
      
      // Calc start position of block in output file
      size_t offset = interleaver.size() - tail + block_cnt;
      if (block_cnt < tail) 
        offset += interleaver.width();

      // Write to output file 
      mapper.write(block, N-K, K, offset, interleaver.width()); 
    }
      
    return 0;

  } catch (const std::exception& e) {
    std::cerr << "[ERROR]" << e.what() << std::endl;
    return 1;
  }
}
