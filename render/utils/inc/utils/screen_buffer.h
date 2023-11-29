#ifndef TINY_RENDER_SCREEN_BUFFER_H
#define TINY_RENDER_SCREEN_BUFFER_H

#include "utils/error_handle.h"
#include <memory>

template<typename T>
class ScreenBuffer {
  const int kWidth_;
  const int kHeight_;
  std::unique_ptr<T> buf_;

public:
  ScreenBuffer(int width, int height, const T &init_value = T())
          : kWidth_(width), kHeight_(height), buf_(new T[width * height]()) {
    InitBuf(init_value);
  }

  T *operator[](size_t row) {
    return buf_.get() + row * kWidth_;
  }

  const T *operator[](size_t row) const {
    return buf_.get() + row * kWidth_;
  }

  void SetWidth(int width) {
    const_cast<int &>(kWidth_) = width;
    // 重置buf
    auto init_value = buf_.get()[0];
    buf_ = std::make_unique<T>(kWidth_ * kHeight_);
    InitBuf(init_value);
  }

  void SetHeight(int height) {
    const_cast<int &>(kHeight_) = height;
    // 重置buf
    auto init_value = buf_.get()[0];
    buf_ = std::make_unique<T>(kWidth_ * kHeight_);
    InitBuf(init_value);
  }

private:
  void InitBuf(const T &init_value) {
    auto n = kWidth_ * kHeight_;
    auto p = buf_.get();
    for (int i = 0; i < n; ++i) p[i] = init_value;
  }

};

#endif //TINY_RENDER_SCREEN_BUFFER_H
