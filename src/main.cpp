#include "main.hpp"

int main() {
  const uint32_t data[] = {1, 2, 33, 228, 69};
  uint32_t read_data[sizeof(data) / sizeof(data)];

  s_ptr<File> file;
  file.reset(new File("test.txt", true));

  file->Write(data, sizeof(data));

  file->Flush();
  file->Close();

  file.reset(new File("test.txt", false));

  int ints_in_file = file->GetSize() / sizeof(uint32_t);

  std::cout << "ints in file: " << ints_in_file << std::endl;

  std::cout << "read pos: " << file->GetReadPos() << std::endl;

  file->Read(read_data, file->GetSize());

  for (int i = 0; i < ints_in_file; i++) {
    std::cout << read_data[i] << " ";
  }

  std::cout << std::endl;

  std::cout << "read pos: " << file->GetReadPos() << std::endl;
}
