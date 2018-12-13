#include <string>

#include "rs"
#include "interleaver.tt"

constexpr size_t N = 255;
constexpr size_t K = 147;

int main(int argc, char *argv[]) {
  if (argc != 2)
    throw std::runtime_error("Missing arguments");

  Interleaver<N, K> interleaver(argv[1]);
  std::array<char, N> block;

  std::string output_filename = argv[1];
  output_filename += ".out";
  
  std::ofstream out(output_filename, std::ios_base::out|std::ios_base::binary);
  out.write(interleaver.FILE.CONTENT, interleaver.FILE.SIZE);

  ezpwd::RS<N, K> encoder;
  while (interleaver.getBlock(block)) {
    encoder.encode(block);
    out.write(block.data() + K, N-K);
  }
    
  return 0;
}
