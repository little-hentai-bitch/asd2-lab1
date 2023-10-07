#pragma once
#include <filesystem>
#include <fstream>
#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

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

  template <typename T> T Read();
  template <typename T> void Write(const T *data);

  void Flush();

  size_t BytesToReadLeft();
  size_t GetSize();

  size_t GetReadPos();
  size_t GetWritePos();

  void SetReadPos(size_t pos);
  void SetWritePos(size_t pos);

  void MoveWriteCursorToEnd();

  bool IsOpen();
  void Close();
};

template <typename T> T File::Read() {
  T val;
  Read(&val, sizeof(T));

  return val;
}

template <typename T> void File::Write(const T *data) {
  Write(data, sizeof(T));
}
