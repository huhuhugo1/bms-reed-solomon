#include <string>

#include "rs"
#include "interleaver.tt"
#include "outputFileMapper.tt"

constexpr size_t N = 255;
constexpr size_t K = 147;

int main(int argc, char *argv[]) {
  if (argc != 2)
    throw std::runtime_error("Missing arguments");

  Interleaver<N, K> interleaver(argv[1]);
  std::array<char, N> block;

  std::string output_filename = argv[1];
  output_filename += ".out";

  size_t out_size = interleaver.FILE.SIZE + interleaver.NUM_OF_BLOCKS * (N- K );
  OutputFileMapper mapper(output_filename, out_size);
  for (size_t i = 0; i < interleaver.FILE.SIZE; ++i) {
    mapper.CONTENT[i] = interleaver.FILE.CONTENT[i];
  }

  ezpwd::RS<N, K> encoder;
  size_t block_cnt = 0;
  size_t tail = interleaver.FILE.SIZE % interleaver.NUM_OF_BLOCKS;
  while (interleaver.getBlock(block)) {
    encoder.encode(block);
    for (size_t i = 0; i < N-K; ++i) {
      size_t idx = interleaver.FILE.SIZE + i * interleaver.NUM_OF_BLOCKS + block_cnt - tail;
      if (block_cnt < tail) idx += interleaver.NUM_OF_BLOCKS;
      mapper.CONTENT[idx] = block[K + i];
    }
    //out.write(block.data() + K, N-K);
    ++block_cnt;
  }
    
  return 0;
}
