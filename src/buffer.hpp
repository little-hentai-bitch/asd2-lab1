#pragma once
#include "file.hpp"
#include "memory.hpp"

template <typename T> class Buffer {
private:
  static int buffers_count;

  u_ptr<File> file;
  fs::path path;
  size_t size;

public:
  Buffer();
  Buffer(Buffer &) = delete;
  Buffer &operator=(Buffer &) = delete;
  ~Buffer();

  size_t GetSize();
  T &operator[](size_t index);
  void PushBack(T val);
  void Resize(size_t size);
  void Clear();
};

template <typename T> Buffer<T>::Buffer() {
  size = 0;

  path = "buffer";
  path += std::to_string(buffers_count++);
  path += ".temp";

  path = fs::absolute(path);

  file.reset(new File(path, true));
}

template <typename T> Buffer<T>::~Buffer() {
  file->Close();
  file.release();

  fs::remove(path);
}

template <typename T> size_t Buffer<T>::GetSize() { return size; }

template <typename T> void Buffer<T>::PushBack(T val) {
  file->SetWritePos(size * sizeof(T));
  file->Write(&val, sizeof(T));
  size++;
}

template <typename T> void Buffer<T>::Resize(size_t size) {
  size_t filesize = file->GetSize();
  if (filesize >= size * sizeof(T)) {
    this->size = size;
    return;
  }

  file->SetWritePos(size * sizeof(T) - 1);

  unsigned char foo = 0;
  file->Write(&foo, 1);

  this->size = size;
}

template <typename T> void Buffer<T>::Clear() { size = 0; }
