#pragma once
#include "file.hpp"
#include "input_buffer.hpp"
#include "memory.hpp"
#include "output_buffer.hpp"
#include "sequences_array.hpp"
#include <chrono>
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace chrono = std::chrono;
using time_point = chrono::high_resolution_clock::time_point;
using duration = chrono::high_resolution_clock::duration;

class Application {
public:
  Application();
  Application(Application &) = delete;
  Application &operator=(Application &) = delete;

  void Run(fs::path input_file, fs::path output_file);

private:
  static constexpr int buffers_count = 12;

  s_ptr<File> input_file;
  std::vector<s_ptr<File>> b_buffer_files, c_buffer_files;

  void SortFile();

  bool CheckSort();
  void Cleanup();

  int MergeSequences(std::vector<s_ptr<File>> &src_files,
                     std::vector<s_ptr<File>> &dst_files);
  void MergeSequenceToBuffer(
      std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays,
      s_ptr<OutputBuffer<int64_t>> &dst_buffer);

  void CreateBufferFiles();
  fs::path GenerateBufferFileName();
  void DeleteBufferFiles();

  void PrintTime(duration time);
};
