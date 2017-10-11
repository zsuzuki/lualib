//
// Copyright 2017 Yoshinori Suzuki<wave.suzuki.z@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.//
//
#pragma once

#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <vector>

namespace LUA
{

// 非同期でスクリプトファイルを読む
struct FileInfo
{
  std::string        path;
  std::future<void*> file;
  std::vector<char>  buffer;
  std::streampos     fsize = 0;
  std::atomic_bool   done{false};

  FileInfo(std::string p) : path(p), file(std::async(std::launch::async, [&]() -> void* { return load(); })) {}

  void* load()
  {
    std::ifstream fst(path, std::ios::binary);
    if (fst.fail())
    {
      std::cerr << "open error: " << path << std::endl;
      return nullptr;
    }

    // ファイルサイズの取得
    fsize = fst.tellg();
    fst.seekg(0, std::ios::end);
    fsize = fst.tellg() - fsize;
    fst.seekg(0, std::ios::beg);

    // 読む
    buffer.resize(fsize);
    fst.read(buffer.data(), buffer.size());
    done = !fst.fail();

    fst.close();
    std::cout << "read[" << path << "]: " << fsize << std::endl;

    return done ? buffer.data() : nullptr;
  }
  const void* getBuffer() { return file.get(); }
  bool        isDone() const { return done; }
  const char* getPath() const { return path.c_str(); }
  size_t      getSize() const { return fsize; }
};

} // namespace LUA
