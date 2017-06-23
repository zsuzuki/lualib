//
// copyright Y.Suzuki 2017/06/20
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
