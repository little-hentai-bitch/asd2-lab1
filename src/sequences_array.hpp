#pragma once
#include "buffer.hpp"
#include <iostream>
#include <vector>

template <typename T> class SequencesArray {
private:
  s_ptr<Buffer<T>> buffer;
  T prev_element;
  T current_element;
  size_t pos;

public:
  SequencesArray(s_ptr<Buffer<T>> buffer);
  SequencesArray(SequencesArray<T> &) = delete;
  SequencesArray<T> &operator=(SequencesArray<T> &) = delete;

  T NextElement();
  bool IsSequenceEnd();
  bool IsArrayEnd();
  size_t GetPos();
};

template <typename T>
SequencesArray<T>::SequencesArray(s_ptr<Buffer<T>> buffer) {
  this->buffer = buffer;
  pos = 0;
}

template <typename T> T SequencesArray<T>::NextElement() {
  current_element = prev_element;
  current_element = buffer->Get(pos);

  pos++;

  return current_element;
}

template <typename T> bool SequencesArray<T>::IsSequenceEnd() {
  if (!pos) {
    return false;
  }

  if (pos == buffer->GetSize()) {
    return true;
  }

  return buffer->Get(pos) < current_element;
}

template <typename T> bool SequencesArray<T>::IsArrayEnd() {
  return pos == buffer->GetSize();
}

template <typename T> size_t SequencesArray<T>::GetPos() { return pos; }
