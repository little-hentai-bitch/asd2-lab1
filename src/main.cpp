#include "main.hpp"
#include <boost/iostreams/code_converter.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace bio = boost::iostreams;

int main() {
  Application app;

  app.Run("input", "output");
}
