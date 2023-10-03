#include <memory>

template <typename T>
using s_ptr = std::shared_ptr<T>;


template <typename T>
using u_ptr = std::unique_ptr<T>;
