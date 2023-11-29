#ifndef TINY_RENDER_MY_GL_H
#define TINY_RENDER_MY_GL_H

#include "utils/my_math.h"
#include "utils/model.h"
#include "utils/screen_buffer.h"
#include "utils/error_handle.h"
#include <vector>
#include <string>
#include <memory>

namespace df {

// camera初始
constexpr float kNear = -20.f;
constexpr float kFar = -200.f;
constexpr float kFov = 60.f;
constexpr float kAspect = 1.f;

// screen初始
constexpr int kWidth = 1000;
constexpr int kHeight = 1000;

// blinn phone光照
constexpr float kDiffuse = 1.f;
constexpr float kSpec = .6f;
constexpr float kEnvironment = 5.f;

}

////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 光源
 */
struct Light {
  Vec4f pos;
  float constant,linear,quadratic;
  explicit Light(float x = 1000.f, float y = 1000.f, float z = 1000.f)
          : pos(x, y, z, 1.f) {}
};

////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 组件（模型）
 */
class Component {
  Vec3f scale_{1.f, 1.f, 1.f};
  Vec3f rotation_{0.f, 0.f, 0.f};
  Vec3f translation_{0.f, 0.f, 0.f};

public:
  std::unique_ptr<Model> model_;

public:
  explicit Component(const std::string &model_path) : model_(new Model(model_path.c_str())) {}

  void SetScale(float s) {
    if (s <= 0.f) return;
    for (int i = 0; i < 3; ++i) scale_[i] = s;
  }

  void SetXRotation(const float r) { rotation_.x = r; }

  void SetYRotation(const float r) { rotation_.y = r; }

  void SetZRotation(const float r) { rotation_.z = r; }

  void AddTranslation(const Vec3f &t) { translation_ = translation_ + t; }

  M4f GetModelMatrix();

};

////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 渲染器
 * 内含相机、屏幕
 * 模型支持：三角形、有法向量、有纹理贴图
 */
class Render {
  /**
   * 相机
   */
  struct Camera {
    Vec3f eye{0.f, 0.f, 0.f};
    Vec4f up{0.f, 1.f, 0.f, 0.f};
    Vec4f look{0.f, 0.f, -1.f, 0.f};

    float near = df::kNear;
    float far = df::kFar;
    float fov = df::kFov;
    float aspect = df::kAspect;

    M4f GetViewMatrix();
    M4f GetProjMatrix();
  };

  /**
   * 屏幕，包含深度缓冲、颜色缓冲
   */
  class Screen {
    const int kWidth_;
    const int kHeight_;

  public:
    explicit Screen(int width = df::kWidth, int height = df::kHeight)
            : kWidth_(width), kHeight_(height),
              zbuf_(width, height, -std::numeric_limits<float>::max()),
              cbuf_(width, height) {}

    int GetWidth() const { return kWidth_; }

    void SetWidth(int width) {
      if (width == kWidth_) return;
      const_cast<int &>(kWidth_) = width;
      zbuf_.SetWidth(width);
      cbuf_.SetWidth(width);
    }

    int GetHeight() const { return kHeight_; }

    void SetHeight(int height) {
      if (height == kHeight_) return;
      const_cast<int &>(kHeight_) = height;
      zbuf_.SetHeight(height);
      cbuf_.SetHeight(height);
    }

    M4f GetScreenMatrix();

  public:
    ScreenBuffer<float> zbuf_;  // 深度缓冲
    ScreenBuffer<TGAColor> cbuf_;  // 颜色缓冲（帧缓冲）
  };

public:
  // todo 提供顶点着色器、片元着色器
  Render() = default;

  Render(int width, int height) : screen(width, height) {
    CheckError(width < 1, "bad width - " + std::to_string(width));
    CheckError(height < 1, "bad height - " + std::to_string(height));
  }

  /**
   * 光源（多个）
   * 模型（多个）
   * @return
   */
  int Rendering(std::vector<Light> &lights, std::vector<Component> &components);

  const ScreenBuffer<TGAColor> &GetFrame() const { return screen.cbuf_; }

  void SetScreenWidth(int width) {
    if (width < 1) {
      std::cout << "bad width" << std::endl;
      return;
    }
    screen.SetWidth(width);
  }

  void SetScreenHeight(int height) {
    if (height < 1) {
      std::cout << "bad height" << std::endl;
      return;
    }
    screen.SetHeight(height);
  }

  void SetCameraEye(const Vec3f &eye) { camera.eye = eye; }

  void SetCameraLook(const Vec3f &up, const Vec3f &look) {
    if (std::abs(up * look - 0.f) > 1e-6f) {
      std::cout << "bad direction" << std::endl;
      return;
    }
    camera.up = {up.x, up.y, up.z, 0.f};
    camera.look = {look.x, look.y, look.z, 0.f};
  }

  void SetCameraPlane(float near, float far) {
    if (near >= 0.f || far >= 0.f || near <= far) {
      std::cout << "bad plane" << std::endl;
      return;
    }
    camera.near = near;
    camera.far = far;
  }

private:
  // 获取包围盒
  void GetBox(std::unique_ptr<Model> &model, const std::vector<int> &face,
              Vec2i &box_min, Vec2i &box_max);
  // 获取重心坐标
  Vec3f Barycenter(const Vertex &vert0, const Vertex &vert1, const Vertex &vert2,
                   int x, int y);

  // 顶点着色器：模型变换、观察变换、投影变换
  void VertexShader(std::vector<Light> &lights, std::vector<Component> &components);
  // 裁剪、屏幕映射
  void ScreenMapping(std::vector<Component> &components);
  // 光栅化（三角形遍历、片元着色器）
  void Rasterization(std::vector<Light> &lights, std::vector<Component> &components);
  void FragmentShader(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                      const Vec3f &correct, TGAColor &color,
                      const std::vector<Light> &lights);

private:
  Camera camera;
  Screen screen;
};

#endif //TINY_RENDER_MY_GL_H
