#pragma once
#include "buffer.hpp"
#include "file.hpp"
#include "memory.hpp"
#include "sequences_array.hpp"
#include <iostream>
#include <span>
#include <string>
#include <vector>

class Application {
private:
  static constexpr int buffers_count = 3;

  s_ptr<File> input_file;
  s_ptr<File> output_file;

  void SortFile();

  bool CheckSort();
  void Cleanup();

  int MergeSequences(std::vector<s_ptr<Buffer<int64_t>>> src_buffers,
                      std::vector<s_ptr<Buffer<int64_t>>> dst_buffers);
  void MergeSequenceToBuffer(
      std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays,
      s_ptr<Buffer<int64_t>> dst_buffer);
  void PrintBuffer(s_ptr<Buffer<int64_t>> buffer);
  
public:
  Application();
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;

  void Run(fs::path input_file, fs::path output_file);
};
