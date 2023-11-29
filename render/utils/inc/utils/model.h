#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include <iostream>
#include "my_math.h"
#include "tga_image.h"

struct Vertex {
  // 3d空间坐标
  Vec4f pos3d;

  // 规范化坐标
  Vec4f pos;
  float w;  // 齐次分量备份

  Vec4f norm;  // 法向量
  Vec2f uv;  // 纹理坐标

  bool cut = false;
};

class Model {
  std::vector<std::vector<int>> faces_;
  std::vector<Vertex> verts_;

  TGAImage diffusemap_;
  TGAImage normalmap_;
  TGAImage specularmap_;

public:
  explicit Model(const char *path);
  ~Model() = default;

  int NVert() const { return (int) verts_.size(); }

  int NFace() const { return (int) faces_.size(); }

  const std::vector<int> &Face(int i) const { return faces_[i]; }

  Vertex &Vert(int i) { return verts_[i]; }

  const Vertex &Vert(int i) const { return verts_[i]; }

  // 返回纹理信息
  TGAColor diffuse(Vec2f uv);
  Vec3f normal(Vec2f uv);
  float specular(Vec2f uv);

private:
  void ParseObj(const char *path);
  void load_texture(std::string path, const char *suffix, TGAImage &img);
};

#endif //__MODEL_H__
