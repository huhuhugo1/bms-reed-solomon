#include <string>
#include <fstream>
#include <cmath>

std::ifstream::pos_type filesize(const std::string& filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

constexpr int N = 7;
constexpr int K = 4;

int main(int argc,char *argv[]) {
    std::string output_filename = argv[1];
    output_filename += ".ok";
    size_t wr_size = filesize(argv[1]);
    auto orig_size = wr_size - (std::ceil(static_cast<double>(wr_size) / N) * (N - K));
    return 0;
}
