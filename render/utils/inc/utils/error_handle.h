#ifndef TINY_RENDER_ERROR_HANDLE_H
#define TINY_RENDER_ERROR_HANDLE_H

#include <iostream>
#include <string>

inline void CheckError(bool err, const std::string &msg = "") {
  if (!err) return;
  if (msg.empty()) {
    std::cerr << "Bad" << std::endl;
  } else {
    std::cerr << "Bad: " << msg << std::endl;
  }
  exit(1);
}

#endif //TINY_RENDER_ERROR_HANDLE_H
