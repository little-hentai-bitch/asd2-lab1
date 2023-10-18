#include "application.hpp"

auto &now = chrono::high_resolution_clock::now;

Application::Application() {}

void Application::Run(fs::path input_file, fs::path output_file) {
  std::cout << fs::current_path() << std::endl;

  this->input_file.reset(new File(input_file, false));
  CreateBufferFiles();

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

void Application::Cleanup() {
  input_file->Close();
  DeleteBufferFiles();
}

void Application::SortFile() {
  std::vector<s_ptr<File>> input_files = {input_file};

  int sequences_count = MergeSequences(input_files, b_buffer_files);

  while (sequences_count > 1) {
    sequences_count = MergeSequences(b_buffer_files, c_buffer_files);
    std::cout << "seq count: " << sequences_count << std::endl;

    std::swap(b_buffer_files, c_buffer_files);
  }

  b_buffer_files[0]->SaveAs("output");
}

int Application::MergeSequences(std::vector<s_ptr<File>> &src_files,
                                std::vector<s_ptr<File>> &dst_files) {
  std::vector<s_ptr<InputBuffer<int64_t>>> src_buffers;
  for (auto &src_file : src_files) {
    src_buffers.push_back(std::make_shared<InputBuffer<int64_t>>(src_file));
  }

  std::vector<s_ptr<OutputBuffer<int64_t>>> dst_buffers;
  for (auto &dst_file : dst_files) {
    dst_buffers.push_back(std::make_shared<OutputBuffer<int64_t>>(dst_file));
  }

  std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays;
  for (auto &src_buffer : src_buffers) {
    s_ptr<SequencesArray<int64_t>> sequences_array =
        std::make_shared<SequencesArray<int64_t>>(src_buffer);
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

    s_ptr<OutputBuffer<int64_t>> dst_buffer = dst_buffers[dst_buffer_i];
    dst_buffer_i = (dst_buffer_i + 1) % dst_buffers.size();

    MergeSequenceToBuffer(sequences_arrays, dst_buffer);

    sequences_count++;
  }

  for (auto &buf : dst_buffers) {
    buf->Flush();
    buf->FlushFileSize();
  }

  return sequences_count;
}

void Application::MergeSequenceToBuffer(
    std::vector<s_ptr<SequencesArray<int64_t>>> sequences_arrays,
    s_ptr<OutputBuffer<int64_t>> &dst_buffer) {

  std::vector<int64_t> values(sequences_arrays.size());
  for (int i = 0; i < values.size(); i++) {
    values[i] = (sequences_arrays[i]->NextElement());
  }

  while (!sequences_arrays.empty()) {
    int min_i = 0;
    int64_t min_value = values[min_i];

    for (int i = 1; i < values.size(); i++) {
      int64_t n_val = values[i];

      if (n_val < min_value) {
        min_i = i;
        min_value = n_val;
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

void Application::CreateBufferFiles() {
  for (int i = 0; i < buffers_count; i++) {
    b_buffer_files.push_back(
        std::make_shared<File>(GenerateBufferFileName(), true));
    c_buffer_files.push_back(
        std::make_shared<File>(GenerateBufferFileName(), true));
  }
}

fs::path Application::GenerateBufferFileName() {
  static int next_index = 0;

  fs::path path = "buffer" + std::to_string(next_index) + ".temp";
  next_index++;

  return path;
}

void Application::DeleteBufferFiles() {}

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
