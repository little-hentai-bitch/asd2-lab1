#pragma once
#include "file.hpp"
#include "memory.hpp"
#include <iostream>

template <typename T> class OutputBuffer {
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

  CachedData write_cache;

  bool IsInCache(CachedData &cache, size_t index);
  void LoadToWriteCache(size_t index);
  size_t ByteIndexInCahce(CachedData &cache, size_t index);

public:
  OutputBuffer();
  OutputBuffer(s_ptr<File> file);
  OutputBuffer(OutputBuffer &) = delete;
  OutputBuffer &operator=(OutputBuffer &) = delete;
  ~OutputBuffer();

  void PushBack(T val);

  size_t GetSize();

  void Flush();
  void FlushFileSize();

  void SaveAs(fs::path path);
};

template <typename T>
OutputBuffer<T>::OutputBuffer(s_ptr<File> file) {
  this->file = file;
  size = 0;
}

template <typename T> OutputBuffer<T>::~OutputBuffer() {}

template <typename T> size_t OutputBuffer<T>::GetSize() { return size; }

template <typename T> void OutputBuffer<T>::PushBack(T val) {
  if (!IsInCache(write_cache, size)) {
    if (write_cache.size) {
      Flush();
    }
    LoadToWriteCache(size);
  }

  size_t byte_index_in_cache = ByteIndexInCahce(write_cache, size);
  *(T *)&write_cache.data[byte_index_in_cache] = val;

  size++;
}

template <typename T> void OutputBuffer<T>::SaveAs(fs::path path) {
  Flush();
  file->Close();

  path = this->path.parent_path() / path.filename();
  fs::rename(this->path, path);

  this->path.clear();
}

template <typename T> void OutputBuffer<T>::Flush() {
  size_t bytes_to_write =
      std::min((size * sizeof(T) - write_cache.offset), write_cache.size);

  file->SetWritePos(write_cache.offset);
  file->Write(write_cache.data.data(), bytes_to_write);
  file->Flush();
}

template <typename T> void OutputBuffer<T>::FlushFileSize() {
  Flush();

  if (file->GetSize() / sizeof(T) > size) {
    file->ShrinkToSize(size * sizeof(T));
  }
}

template <typename T>
bool OutputBuffer<T>::IsInCache(CachedData &cache, size_t index) {
  if (!cache.size) {
    return false;
  }

  size_t byte_index = index * sizeof(T);

  bool is_after_offset = (byte_index >= cache.offset);
  bool is_before_end = (byte_index <= cache.offset + cache.size - sizeof(T));

  return is_after_offset && is_before_end;
}

template <typename T> void OutputBuffer<T>::LoadToWriteCache(size_t index) {
  write_cache.offset = sizeof(T) * (index == 0 ? index : index - 1);
  write_cache.size = cache_elements_count * sizeof(T);
  write_cache.data.resize(write_cache.size);

  file->SetReadPos(write_cache.offset);
  size_t bytes_to_read = std::min(file->BytesToReadLeft(), write_cache.size);

  file->Read(write_cache.data.data(), bytes_to_read);
}

template <typename T>
size_t OutputBuffer<T>::ByteIndexInCahce(CachedData &cache, size_t index) {
  size_t absolute_byte_index = index * sizeof(T);
  size_t byte_index_in_cache = absolute_byte_index - cache.offset;

  return byte_index_in_cache;
}

template <typename T> int OutputBuffer<T>::buffers_count = 0;
