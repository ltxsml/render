#include "my_gl.h"
#include <limits>

int Render::Rendering(std::vector<Light> &lights, std::vector<Component> &components) {
  // 顶点着色器：模型变换、观察变换、投影变换
  VertexShader(lights, components);

  // 裁剪、屏幕映射
  ScreenMapping(components);

  // 光栅化（三角形遍历、片元着色器）
  Rasterization(lights, components);

  return 0;
}

void Render::GetBox(std::unique_ptr<Model> &model, const std::vector<int> &face,
                    Vec2i &box_min, Vec2i &box_max) {
  // 获取包围盒
  Vec2f bmin{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
  Vec2f bmax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
  for (int j = 0; j < 3; ++j) {
    const auto &v = model->Vert(face[j]);
    bmin.x = std::min(bmin.x, v.pos.x);
    bmin.y = std::min(bmin.y, v.pos.y);
    bmax.x = std::max(bmax.x, v.pos.x);
    bmax.y = std::max(bmax.y, v.pos.y);
  }
  box_min = {static_cast<int>(bmin.x), static_cast<int>(bmin.y)};
  box_max = {static_cast<int>(bmax.x), static_cast<int>(bmax.y)};
}

Vec3f
Render::Barycenter(const Vertex &vert0, const Vertex &vert1, const Vertex &vert2,
                   int x, int y) {
  Vec2f P{static_cast<float>(x), static_cast<float>(y)};
  Vec2f A{vert0.pos.x, vert0.pos.y};
  Vec2f B{vert1.pos.x, vert1.pos.y};
  Vec2f C{vert2.pos.x, vert2.pos.y};

  // 计算向量
  auto v0 = C - A;
  auto v1 = B - A;
  auto v2 = P - A;

  // 计算点积
  auto dot00 = v0 * v0;
  auto dot01 = v0 * v1;
  auto dot02 = v0 * v2;
  auto dot11 = v1 * v1;
  auto dot12 = v1 * v2;

  // 计算重心坐标
  auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
  auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  // (1-u-v)A + vB + uC
  return {1.f - u - v, v, u};
}

void Render::VertexShader(std::vector<Light> &lights, std::vector<Component> &components) {
  auto view_matrix = camera.GetViewMatrix();
  auto proj_matrix = camera.GetProjMatrix();
  // 光源变换
  for (auto &light : lights) {
    light.pos = view_matrix * light.pos;
  }
  // 模型顶点变换
  for (auto &c : components) {
    auto m = c.GetModelMatrix() * view_matrix;
    for (int i = 0; i < c.model_->NVert(); ++i) {
      auto &v = c.model_->Vert(i);
      // 模型、观察变换（法向量需变换）
      v.pos = m * v.pos;
      v.norm = m * v.norm;
      v.norm.normalize();
      v.pos3d = v.pos;  // 暂存3d空间位置
      // 投影变换
      v.pos = proj_matrix * v.pos;
      v.w = v.pos.w;  // 暂存w分量，用于透视插值矫正
    }
  }
}

void Render::ScreenMapping(std::vector<Component> &components) {
  auto screen_matrix = screen.GetScreenMatrix();
  for (auto &c : components) {
    for (int i = 0; i < c.model_->NVert(); ++i) {
      auto &v = c.model_->Vert(i);
      // 简单裁剪（丢弃三角形）
      float w = std::abs(v.w), mw = -w;
      float z = -v.pos.z;
      if (v.pos.x < mw || v.pos.x > w || v.pos.y < mw || v.pos.y > w ||
          z > camera.near || z < camera.far) {
        v.cut = true;
        continue;
      }
      // 透视除法
      v.pos = v.pos / v.w;
      // 屏幕映射
      v.pos = screen_matrix * v.pos;
    }
  }
}

void Render::Rasterization(std::vector<Light> &lights, std::vector<Component> &components) {
  for (auto &c : components) {
    auto &model = c.model_;
    auto nface = model->NFace();
    for (int i = 0; i < nface; ++i) {
      const auto &face = model->Face(i);
      bool cut = false;
      for (int j = 0; j < 3; ++j) {
        if (model->Vert(face[j]).cut) {
          cut = true;
          break;
        }
      }
      if (cut) continue;
      // 获取包围盒
      Vec2i box_min, box_max;
      GetBox(model, face, box_min, box_max);

      // 逐像素处理
      const auto &v0 = model->Vert(face[0]);
      const auto &v1 = model->Vert(face[1]);
      const auto &v2 = model->Vert(face[2]);
      for (int x = box_min.x; x <= box_max.x; ++x) {
        for (int y = box_min.y; y <= box_max.y; ++y) {
          // 计算重心坐标 (1-u-v)A + vB + uC
          auto bary = Barycenter(v0, v1, v2, x, y);
          // 检测是否在三角形内
          bool good = bary.x > 0.f && bary.y >= 0.f && bary.z >= 0.f;
          if (!good) continue;

          // 计算透视插值矫正
          Vec3f correct{
                  bary.x / v0.w,
                  bary.y / v1.w,
                  bary.z / v2.w
          };
          float z = 1.f / (correct.x + correct.y + correct.z);
          correct = correct * z;

          // z buffer 算法
          if (x < 0 || x >= screen.GetWidth() ||
              y < 0 || y >= screen.GetHeight() ||
              z < screen.zbuf_[y][x]) {
            continue;
          }
          screen.zbuf_[y][x] = z;

          // 纹理坐标插值
          Vec2f uv = v0.uv * correct.x + v1.uv * correct.y + v2.uv * correct.z;
          // 颜色计算
          auto color = model->diffuse(uv);

          // 片元着色
          FragmentShader(v0, v1, v2, correct, color, lights);

          // 颜色填充
          screen.cbuf_[y][x] = color;
        }
      }
    }
  }
}

void Render::FragmentShader(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                            const Vec3f &correct, TGAColor &color,
                            const std::vector<Light> &lights) {
  // blinn phone光照
  // 3d空间坐标
  auto P = v0.pos3d * correct.x + v1.pos3d * correct.y + v2.pos3d * correct.z;
  P.w = 1.f;
  // 法向量
  auto norm = v0.norm * correct.x + v1.norm * correct.y + v2.norm * correct.z;
  
  norm.normalize();
  // 视向量（经过观察变换，eye位于原点）
  auto view = Vec4f(0.f, 0.f, 0.f, 1.f) - P;
  view.normalize();
  // 逐光源计算光照
  for (const auto &light: lights) {
    // 光向量
    auto lightv = light.pos - P;
    auto distance = sqrt(lightv.x*lightv.x+lightv.y*lightv.y+lightv.z*lightv.z);
    lightv.normalize();
    float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    // 漫反射
    auto diffuse = df::kDiffuse * std::max(0.f, norm * lightv)*attenuation;
    // 镜面反射
    auto halfv = view + lightv;
    auto spec_cos = norm * halfv.normalize()*attenuation;
    auto spec = df::kSpec * (
            spec_cos <= 0.f ? 0.f :
            static_cast<float>(pow(spec_cos, 128.))
    );
    // 颜色计算
    for (int i = 0; i < 3; ++i) {
      auto c = static_cast<float>(color[i]) * (diffuse + spec) + df::kEnvironment;
      if (c > 255.f) c = 255.f;
      color[i] = static_cast<unsigned char>(c);
    }
  }
}

M4f Component::GetModelMatrix() {
  M4f sm = GetScaleMatrix(scale_);

  M4f rx_m = GetXRotateMatrix(rotation_[0]);
  M4f ry_m = GetYRotateMatrix(rotation_[1]);
  M4f rz_m = GetZRotateMatrix(rotation_[2]);

  M4f tm = GetTranslationMatrix(translation_);

  return tm * rz_m * ry_m * rx_m * sm;
}

M4f Render::Camera::GetViewMatrix() {
  auto &y = up;
  auto z = look * -1.f;
  auto x = Cross(y, z);

  // 平移矩阵
  auto tm = GetTranslationMatrix(eye * -1.f);

  // 计算旋转矩阵
  M4f rm;
  rm[0] = x;
  rm[1] = y;
  rm[2] = z;
  rm[3][3] = 1.f;

  // 先平移，再旋转
  return rm * tm;
}

M4f Render::Camera::GetProjMatrix() {
  auto A = near + far;
  auto B = -2.f * near * far;
  auto D = near - far;
  auto ct = static_cast<float>(1. / tan(fov * M_PI / 180. / 2.));

  // 投影变换，至齐次裁剪
  M4f m;
  m[0] = {-ct / aspect, 0, 0, 0};
  m[1] = {0, -ct, 0, 0};
  m[2] = {0, 0, A / D, B / D};
  m[3] = {0, 0, 1, 0};
  return m;
}

M4f Render::Screen::GetScreenMatrix() {
  M4f sm = GetScaleMatrix({kWidth_ / 2.f, kHeight_ / 2.f, 1.f});
  M4f tm = GetTranslationMatrix({kWidth_ / 2.f, kHeight_ / 2.f, 0.f});
  return tm * sm;
}
