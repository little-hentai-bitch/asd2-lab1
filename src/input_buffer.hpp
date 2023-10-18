#pragma once
#include "file.hpp"
#include "memory.hpp"
#include <iostream>

template <typename T> class InputBuffer {
private:
  static constexpr size_t cache_elements_count = 1024 * 4;

  struct CachedData {
    std::vector<char> data;
    size_t offset = 0;
    size_t size = 0;
  };

  static int buffers_count;

  s_ptr<File> file;
  size_t size;

  CachedData read_cache;

  bool IsInCache(CachedData &cache, size_t index);
  void LoadToReadCache(size_t index);
  size_t ByteIndexInCahce(CachedData &cache, size_t index);

public:
  InputBuffer(s_ptr<File> file);
  InputBuffer(InputBuffer &) = delete;
  InputBuffer &operator=(InputBuffer &) = delete;
  ~InputBuffer();

  T Get(int index);

  size_t GetSize();
};

template <typename T> InputBuffer<T>::InputBuffer(s_ptr<File> file) {
  size = file->GetSize() / sizeof(T);

  file->SetReadPos(0);
  file->SetWritePos(0);

  this->file = file;
}

template <typename T> InputBuffer<T>::~InputBuffer() {}

template <typename T> size_t InputBuffer<T>::GetSize() { return size; }

template <typename T> T InputBuffer<T>::Get(int index) {
  if (!IsInCache(read_cache, index)) {
    LoadToReadCache(index);
  }

  size_t byte_index_in_cache = ByteIndexInCahce(read_cache, index);
  T *val_ptr = (T *)&read_cache.data[byte_index_in_cache];

  return *val_ptr;
}

template <typename T>
bool InputBuffer<T>::IsInCache(CachedData &cache, size_t index) {
  if (!cache.size) {
    return false;
  }

  size_t byte_index = index * sizeof(T);

  bool is_after_offset = (byte_index >= cache.offset);
  bool is_before_end = (byte_index <= cache.offset + cache.size - sizeof(T));

  return is_after_offset && is_before_end;
}

template <typename T> void InputBuffer<T>::LoadToReadCache(size_t index) {
  read_cache.offset = sizeof(T) * (index == 0 ? index : index - 1);

  size_t size_in_bytes = cache_elements_count * sizeof(T);
  read_cache.size =
      std::min(size_in_bytes, file->GetSize() - read_cache.offset);

  read_cache.data.resize(read_cache.size);

  file->SetWritePos(read_cache.offset);
  file->Read(read_cache.data.data(), read_cache.size);
}

template <typename T>
size_t InputBuffer<T>::ByteIndexInCahce(CachedData &cache, size_t index) {
  size_t absolute_byte_index = index * sizeof(T);
  size_t byte_index_in_cache = absolute_byte_index - cache.offset;

  return byte_index_in_cache;
}

template <typename T> int InputBuffer<T>::buffers_count = 0;
