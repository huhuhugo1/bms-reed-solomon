#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "interleaver.tt"

std::ifstream::pos_type filesize(const std::string& filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

constexpr int N = 255;
constexpr int K = 147;

class OutputFileMapper {
public:
  char* const CONTENT;
  const size_t SIZE;
private:
  int fd;
public:
  OutputFileMapper(const std::string& file_path, size_t size):
  CONTENT([=] {
    fd = open(file_path.c_str(), O_RDWR | O_CREAT, (mode_t)0600);
    if (fd < 0)
      throw std::runtime_error("Unable to create file " + file_path + "!");

    lseek(fd, size - 1, SEEK_SET);
    write(fd, "", 1);
    auto mapped = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED)
      throw std::runtime_error("Uname to map file " + file_path + "!");
    return static_cast<char*>(mapped);
  }()),
  
  SIZE(size) {}

  ~OutputFileMapper() {
    msync((void*)CONTENT, SIZE, MS_SYNC);
    munmap((void*)CONTENT, SIZE);
    close(fd);
  }
};

int main(int argc,char *argv[]) {
    std::string output_filename = argv[1];
    output_filename += ".ok";
    const size_t wr_size = filesize(argv[1]);
    const size_t B = wr_size - (N - 1) * std::ceil((double)wr_size / N);
    const size_t S = N * std::ceil((double)wr_size / N) - wr_size;
    const size_t orig_size = wr_size - (N - K) * (B+ S);

    std::array<char, N> block;
    Interleaver<N, K> interleaver(argv[1], orig_size);
    OutputFileMapper mapper(output_filename, orig_size);
    ezpwd::RS<N, K> decoder;
    for (size_t i = 0; i < B + S; ++i) {
      auto x = interleaver.getBlock(block);
      memcpy(block.data() + K, interleaver.FILE.CONTENT + orig_size + i * (N - K), N - K);
      decoder.decode(block);
      for (size_t line = 0; line < x; ++line) {
        mapper.CONTENT[line * (B + S) + i] = block[line];
      }
    }
    
    return 0; 
}
