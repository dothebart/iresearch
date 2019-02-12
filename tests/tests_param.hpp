////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrey Abramov
/// @author Vasiliy Nabatchikov
////////////////////////////////////////////////////////////////////////////////

#ifndef IRESEARCH_TESTS_PARAM_H
#define IRESEARCH_TESTS_PARAM_H

#include "store/directory.hpp"
#include "store/directory_attributes.hpp"
#include <memory>

class test_base;

namespace tests {

class rot13_cipher final : public irs::cipher {
 public:
  static std::shared_ptr<irs::cipher> make(size_t block_size) {
    return std::make_shared<rot13_cipher>(block_size);
  }

  explicit rot13_cipher(size_t block_size) NOEXCEPT
    : block_size_(block_size) {
  }

  size_t block_size() const NOEXCEPT {
    return block_size_;
  }

  virtual bool decrypt(irs::byte_type* data) const override {
    for (size_t i = 0; i < block_size_; ++i) {
      data[i] -= 13;
    }
    return true;
  }

  virtual bool encrypt(irs::byte_type* data) const override {
    for (size_t i = 0; i < block_size_; ++i) {
      data[i] += 13;
    }
    return true;
  }

 private:
  size_t block_size_;
}; // rot13_cipher

typedef std::pair<std::shared_ptr<irs::directory>, std::string>(*dir_factory_f)(const test_base*);
std::pair<std::shared_ptr<irs::directory>, std::string> memory_directory(const test_base*);
std::pair<std::shared_ptr<irs::directory>, std::string> fs_directory(const test_base* test);
std::pair<std::shared_ptr<irs::directory>, std::string> mmap_directory(const test_base* test);

template<dir_factory_f DirectoryGenerator, size_t BlockSize>
std::pair<std::shared_ptr<irs::directory>, std::string> rot13_cipher_directory(const test_base* ctx) {
  auto info = DirectoryGenerator(ctx);

  if (info.first) {
    info.first->attributes().emplace<rot13_cipher>(BlockSize);
  }

  return std::make_pair(info.first, info.second + "_cipher_rot13");
}

} // tests

#endif
