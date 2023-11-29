#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/model.h"

Model::Model(const char *path) {
  // 解析模型
  ParseObj(path);

  // 加载纹理
  load_texture(path, "_diffuse.tga", diffusemap_);
  load_texture(path, "_nm.tga", normalmap_);
  load_texture(path, "_spec.tga", specularmap_);
}

TGAColor Model::diffuse(Vec2f uvf) {
  Vec2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
  return diffusemap_.get(uv[0], uv[1]);
}

//Vec3f Model::normal(Vec2f uv) {
//  return Vec3f();
//}

float Model::specular(Vec2f uvf) {
  Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
  return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}

void Model::load_texture(std::string path, const char *suffix, TGAImage &img) {
  std::string texfile(path);
  size_t dot = texfile.find_last_of(".");
  if (dot != std::string::npos) {
    texfile = texfile.substr(0, dot) + std::string(suffix);
    std::cerr << "texture file " << texfile << " loading "
              << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed")
              << std::endl;
    img.flip_vertically();
  }
}

void Model::ParseObj(const char *path) {
  // 打开文件
  std::ifstream in;
  in.open(path, std::ifstream::in);
  if (in.fail()) {
    std::cerr << "bad path: " << path << std::endl;
    exit(1);
  }

  // 逐行解析
  std::vector<Vec4f> verts;
  std::vector<Vec4f> norms;
  std::vector<Vec2f> uvs;
  std::vector<std::vector<Vec3i>> faces;
  std::string line;
  while (std::getline(in, line)) {
    std::istringstream iss(line);
    char trash;
    if (!line.compare(0, 2, "v ")) {
      // 顶点
      iss >> trash;
      Vec4f v;
      v.w = 1.f;
      for (int i = 0; i < 3; i++) iss >> v[i];
      verts.push_back(v);

    } else if (!line.compare(0, 3, "vn ")) {
      // 法向量
      iss >> trash >> trash;
      Vec4f n;
      for (int i = 0; i < 3; i++) iss >> n[i];
      n.w = 0.f;
      n.normalize();
      norms.push_back(n);

    } else if (!line.compare(0, 3, "vt ")) {
      // 纹理坐标
      iss >> trash >> trash;
      Vec2f uv;
      for (int i = 0; i < 2; i++) iss >> uv[i];
      uvs.push_back(uv);

    } else if (!line.compare(0, 2, "f ")) {
      // 面片
      std::vector<Vec3i> f;
      Vec3i tmp;
      iss >> trash;
      while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
        for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
        f.push_back(tmp);
      }
      faces.push_back(f);
    }
  }

  // 打印解析信息
  std::cerr << "  v# " << verts.size() << std::endl
            << "  f# " << faces.size() << std::endl
            << " vt# " << uvs.size() << std::endl
            << " vn# " << norms.size() << std::endl;

  // todo，假设不同面片相同顶点信息一致；模型提供了法向量
  // 组装顶点信息、简化面片信息
  auto fn = faces.size();
  faces_.resize(fn);

  verts_.resize(verts.size());
  std::vector<bool> visited(verts.size(), false);
  for (size_t i = 0; i < fn; ++i) {
    const auto &face = faces[i];
    for (const auto &v_info : face) {
      // 简化面片
      faces_[i].push_back(v_info[0]);
      // 顶点组装
      if (visited[v_info[0]]) continue;
      visited[v_info[0]] = true;
      verts_[v_info[0]].pos = verts[v_info[0]];
      verts_[v_info[0]].uv = uvs[v_info[1]];
      verts_[v_info[0]].norm = norms[v_info[2]];
    }
  }
}
