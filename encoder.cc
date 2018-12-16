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
    
    Interleaver<N, K> interleaver(argv[1]);
    std::array<char, N> block;

    size_t out_size = interleaver.FILE.SIZE + interleaver.NUM_OF_BLOCKS * (N - K);
    OutputFileMapper mapper(output_filename, out_size);
    mapper.write(interleaver.FILE.CONTENT, interleaver.FILE.SIZE);

    ezpwd::RS<N, K> encoder;
    size_t tail = interleaver.FILE.SIZE % interleaver.NUM_OF_BLOCKS;
    for (size_t block_cnt = 0; interleaver.getBlock(block); ++block_cnt) {
      encoder.encode(block);
      
      size_t offset = interleaver.FILE.SIZE - tail + block_cnt;
      if (block_cnt < tail) 
        offset += interleaver.NUM_OF_BLOCKS;
      mapper.write(block, N-K, K, offset, interleaver.NUM_OF_BLOCKS); 
    }
      
    return 0;

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
