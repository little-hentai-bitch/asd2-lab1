#include "binary_file.hpp"

BinaryFile::BinaryFile(fs::path path, bool trunc) : File(path, trunc) {}

int32_t BinaryFile::ReadInt32() {
  int32_t x;
  Read(&x, sizeof(x));

  return x;
}

void BinaryFile::WriteInt32(int32_t x) { Write(&x, sizeof(x)); }
