#include "file.hpp"

class BinaryFile : public File {
private:
public:
  BinaryFile(fs::path path, bool trunc);

  int32_t ReadInt32();
  void WriteInt32(int32_t x);
};
