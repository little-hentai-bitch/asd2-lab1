#pragma once
#include "file.hpp"
#include "memory.hpp"
#include <iostream>

template <typename T> class Buffer {
private:
  static constexpr size_t cache_elements_count = 1024 * 4;

  struct CachedData {
    std::vector<char> data;
    size_t offset = 0;
    size_t size = 0;
  };

  static int buffers_count;

  s_ptr<File> file;
  fs::path path;
  size_t size;

  CachedData read_cache, write_cache;

  bool IsInCache(CachedData &cache, size_t index);
  void LoadToReadCache(size_t index);
  void LoadToWriteCache(size_t index);
  size_t ByteIndexInCahce(CachedData &cache, size_t index);

public:
  Buffer();
  Buffer(s_ptr<File> file);
  Buffer(Buffer &) = delete;
  Buffer &operator=(Buffer &) = delete;
  ~Buffer();

  T Get(int index);
  void PushBack(T val);

  size_t GetSize();
  void Resize(size_t size);

  void Flush();

  void Clear();
  void SaveAs(fs::path path);
};

template <typename T> Buffer<T>::Buffer() {
  size = 0;

  path = "buffer";
  path += std::to_string(buffers_count++);
  path += ".temp";

  path = fs::absolute(path);

  file.reset(new File(path, true));
}

template <typename T> Buffer<T>::Buffer(s_ptr<File> file) {
  size = file->GetSize() / sizeof(T);

  file->SetReadPos(0);
  file->SetWritePos(0);

  this->file = file;
  path.clear();
}

template <typename T> Buffer<T>::~Buffer() {
  if (path.empty()) {
    return;
  }

  file->Close();
  file.reset();

  fs::remove(path);
}

template <typename T> size_t Buffer<T>::GetSize() { return size; }

template <typename T> T Buffer<T>::Get(int index) {
  if (IsInCache(write_cache, index)) {
    size_t byte_index_in_cache = ByteIndexInCahce(write_cache, index);
    return *(T *)&write_cache.data[byte_index_in_cache];
  }

  if (!IsInCache(read_cache, index)) {
    LoadToReadCache(index);
  }

  size_t byte_index_in_cache = ByteIndexInCahce(read_cache, index);
  T *val_ptr = (T *)&read_cache.data[byte_index_in_cache];

  return *val_ptr;
}

template <typename T> void Buffer<T>::PushBack(T val) {
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

template <typename T> void Buffer<T>::Clear() {
  size = 0;
  write_cache = CachedData{};
  read_cache = CachedData{};
}

template <typename T> void Buffer<T>::SaveAs(fs::path path) {
  Flush();
  file->Close();

  path = this->path.parent_path() / path.filename();
  fs::rename(this->path, path);

  this->path.clear();
}

template <typename T> void Buffer<T>::Flush() {
  size_t bytes_to_write =
      std::min((size * sizeof(T) - write_cache.offset), write_cache.size);

  file->SetWritePos(write_cache.offset);
  file->Write(write_cache.data.data(), bytes_to_write);
  file->Flush();

  read_cache = CachedData{};
}

template <typename T>
bool Buffer<T>::IsInCache(CachedData &cache, size_t index) {
  if (!cache.size) {
    return false;
  }

  size_t byte_index = index * sizeof(T);

  bool is_after_offset = (byte_index >= cache.offset);
  bool is_before_end = (byte_index <= cache.offset + cache.size - sizeof(T));

  return is_after_offset && is_before_end;
}

template <typename T> void Buffer<T>::LoadToReadCache(size_t index) {
  read_cache.offset = sizeof(T) * (index == 0 ? index : index - 1);

  size_t size_in_bytes = cache_elements_count * sizeof(T);
  read_cache.size =
      std::min(size_in_bytes, file->GetSize() - read_cache.offset);

  read_cache.data.resize(read_cache.size);

  file->SetWritePos(read_cache.offset);
  file->Read(read_cache.data.data(), read_cache.size);
}

template <typename T> void Buffer<T>::LoadToWriteCache(size_t index) {
  write_cache.offset = sizeof(T) * (index == 0 ? index : index - 1);
  write_cache.size = cache_elements_count * sizeof(T);
  write_cache.data.resize(write_cache.size);

  file->SetReadPos(write_cache.offset);
  size_t bytes_to_read = std::min(file->BytesToReadLeft(), write_cache.size);

  file->Read(write_cache.data.data(), bytes_to_read);
}

template <typename T>
size_t Buffer<T>::ByteIndexInCahce(CachedData &cache, size_t index) {
  size_t absolute_byte_index = index * sizeof(T);
  size_t byte_index_in_cache = absolute_byte_index - cache.offset;

  return byte_index_in_cache;
}

template <typename T> int Buffer<T>::buffers_count = 0;
