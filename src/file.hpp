#pragma once
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class File {
private:
  std::fstream file;

public:
  File(fs::path path, bool trunc);
  File(File &) = delete;
  File &operator=(File &) = delete;
  ~File();

  void Read(void *data, size_t size);
  void Write(const void *data, size_t size);

  void Flush();
  
  size_t BytesToReadLeft();
  size_t GetSize();

  size_t GetReadPos();
  size_t GetWritePos();

  void SetReadPos(size_t pos);
  void SetWritePos(size_t pos);
  
  bool IsOpen();
  void Close();
};
