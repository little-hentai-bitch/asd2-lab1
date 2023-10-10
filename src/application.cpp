#include "application.hpp"

auto &now = chrono::high_resolution_clock::now;

Application::Application() {}

void Application::Run(fs::path input_file, fs::path output_file) {
  this->input_file.reset(new File(input_file, false));

  time_point sort_start_time = now();

  SortFile();

  time_point sort_end_time = now();

  duration sort_time = sort_end_time - sort_start_time;
  PrintTime(sort_time);

  bool is_sorted = CheckSort();
  if (is_sorted) {
    std::cout << "file sorted" << std::endl;
  } else {
    std::cout << "error: file is not sorted" << std::endl;
  }

  Cleanup();
}

void Application::PrintTime(duration time) {
  const long nanoseconds_in_second =
      chrono::duration_cast<chrono::nanoseconds>(chrono::seconds(1)).count();

  long sort_time_in_nanoseconds =
      chrono::duration_cast<chrono::nanoseconds>(time).count();

  long double sort_time_in_seconds =
      sort_time_in_nanoseconds / (long double)nanoseconds_in_second;

  std::cout << "sort time is " << sort_time_in_seconds << " seconds"
            << std::endl;
}

void Application::SortFile() {
  std::vector<s_ptr<Buffer<int64_t>>> input_buffer(1);
  input_buffer[0].reset(new Buffer<int64_t>(input_file));

  std::vector<s_ptr<Buffer<int64_t>>> b_buffers;
  std::vector<s_ptr<Buffer<int64_t>>> c_buffers;

  for (int i = 0; i < buffers_count; i++) {
    b_buffers.push_back(std::make_shared<Buffer<int64_t>>());
    c_buffers.push_back(std::make_shared<Buffer<int64_t>>());
  }

  int sequences_count = MergeSequences(input_buffer, b_buffers);

  while (sequences_count > 1) {
    for (int i = 0; i < c_buffers.size(); i++) {
      c_buffers[i]->Clear();
    }

    sequences_count = MergeSequences(b_buffers, c_buffers);

    std::swap(b_buffers, c_buffers);
  }

  b_buffers[0]->SaveAs("output");
}

int Application::MergeSequences(
    std::vector<s_ptr<Buffer<int64_t>>> src_buffers,
    std::vector<s_ptr<Buffer<int64_t>>> dst_buffers) {

  std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays;

  for (int i = 0; i < src_buffers.size(); i++) {
    s_ptr<SequencesArray<int64_t>> sequences_array =
        std::make_shared<SequencesArray<int64_t>>(src_buffers[i]);
    sequences_arrays.push_back(sequences_array);
  }

  int dst_buffer_i = 0;
  int sequences_count = 0;

  while (true) {
    for (int i = 0; i < sequences_arrays.size(); i++) {
      if (!sequences_arrays[i]->IsArrayEnd()) {
        continue;
      }

      sequences_arrays.erase(sequences_arrays.begin() + i);
      i--;
    }

    if (sequences_arrays.empty()) {
      break;
    }

    s_ptr<Buffer<int64_t>> dst_buffer = dst_buffers[dst_buffer_i];
    dst_buffer_i = (dst_buffer_i + 1) % dst_buffers.size();

    MergeSequenceToBuffer(sequences_arrays, dst_buffer);

    sequences_count++;
  }

  for (auto &buf : dst_buffers) {
    buf->Flush();
  }

  return sequences_count;
}

void Application::MergeSequenceToBuffer(
    std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays,
    s_ptr<Buffer<int64_t>> dst_buffer) {

  std::vector<int64_t> values;
  for (auto &i : sequences_arrays) {
    values.push_back(i->NextElement());
  }

  while (!sequences_arrays.empty()) {
    int min_i = 0;
    int64_t min_value = values[min_i];
    for (int i = 1; i < values.size(); i++) {
      if (values[i] < min_value) {
        min_i = i;
        min_value = values[i];
      }
    }

    if (sequences_arrays[min_i]->IsSequenceEnd()) {
      values.erase(values.begin() + min_i);
      sequences_arrays.erase(sequences_arrays.begin() + min_i);
    } else {
      values[min_i] = sequences_arrays[min_i]->NextElement();
    }

    dst_buffer->PushBack(min_value);
  }
}

void Application::PrintBuffer(s_ptr<Buffer<int64_t>> buffer) {
  for (int i = 0; i < buffer->GetSize(); i++) {
    std::cout << buffer->Get(i) << " ";
  }

  std::cout << std::endl;
}

bool Application::CheckSort() {
  u_ptr<File> output_file;
  output_file.reset(new File("output", false));

  int numbers = output_file->GetSize() / sizeof(uint64_t);
  output_file->SetReadPos(0);

  int64_t prev_x = output_file->Read<int64_t>();
  for (int i = 1; i < numbers; i++) {
    int64_t x = output_file->Read<int64_t>();
    if (prev_x > x) {
      return false;
    }

    prev_x = x;
  }

  return true;
}

void Application::Cleanup() { input_file->Close(); }
