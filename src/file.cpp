#include "file.hpp"

File::File(fs::path path, bool trunc) {
  this->path = path;

  std::ios::openmode trunc_mode =
      trunc ? std::ios::trunc : (std::ios::openmode)0;

  file.open(path, std::ios::binary | std::ios::in | std::ios::out | trunc_mode);
}

File::~File() {
  if (IsOpen()) {
    Close();
  }
}

void File::SaveAs(fs::path path) {
  file.close();
  fs::rename(this->path, path);
}

void File::Read(void *data, size_t size) { file.read((char *)data, size); }

void File::Write(const void *data, size_t size) {
  file.write((char *)data, size);
}

void File::Flush() { file.flush(); }

size_t File::BytesToReadLeft() { return GetSize() - GetReadPos(); }

size_t File::GetSize() {
  size_t initial_pos = GetReadPos();

  file.seekg(0, std::ios::end);
  size_t filesize = GetReadPos();

  SetReadPos(initial_pos);

  return filesize;
}

size_t File::GetReadPos() { return (size_t)file.tellg(); }

size_t File::GetWritePos() { return (size_t)file.tellp(); }

void File::SetReadPos(size_t pos) { file.seekg(pos, std::ios::beg); }

void File::SetWritePos(size_t pos) { file.seekp(pos, std::ios::beg); }

void File::MoveWriteCursorToEnd() { file.seekp(0, std::ios::end); }

void File::ShrinkToSize(size_t size) {
  file.close();

  fs::resize_file(path, size);

  file.open(path, std::ios::binary | std::ios::in | std::ios::out);
}

bool File::IsOpen() { return file.is_open(); }

void File::Close() { file.close(); }
