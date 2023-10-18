#pragma once
#include "input_buffer.hpp"
#include <iostream>
#include <vector>

template <typename T> class SequencesArray {
private:
  s_ptr<InputBuffer<T>> buffer;
  T current_element;
  size_t pos;

public:
  SequencesArray(s_ptr<InputBuffer<T>> buffer);
  SequencesArray(SequencesArray<T> &) = delete;
  SequencesArray<T> &operator=(SequencesArray<T> &) = delete;

  T NextElement();
  bool IsSequenceEnd();
  bool IsArrayEnd();
  size_t GetPos();
};

template <typename T>
SequencesArray<T>::SequencesArray(s_ptr<InputBuffer<T>> buffer) {
  this->buffer = buffer;
  pos = 0;
}

template <typename T> T SequencesArray<T>::NextElement() {
  current_element = buffer->Get(pos);

  pos++;

  return current_element;
}

template <typename T> bool SequencesArray<T>::IsSequenceEnd() {
  if (pos == buffer->GetSize() || !pos) {
    return pos;
  }

  return buffer->Get(pos) < current_element;
}

template <typename T> bool SequencesArray<T>::IsArrayEnd() {
  return pos == buffer->GetSize();
}

template <typename T> size_t SequencesArray<T>::GetPos() { return pos; }
