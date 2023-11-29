#include "my_gl.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " obj_files" << std::endl;
    exit(1);
  }

  // 默认屏幕大小：1000 * 1000
  // 默认摄像机：原点，向-z看
  Render render;
  render.SetCameraPlane(-10.f, df::kFar);

  // 设置光源，默认位于 1000.f, 1000.f, 1000.f
  std::vector<Light> lights;
  lights.emplace_back();

  // 设置模型，默认位于原点
  std::vector<Component> components;
  for (int i = 1; i < argc; ++i) {
    components.emplace_back(argv[i]);
    components.back().SetScale(40.f);
    components.back().AddTranslation({0.f, 0.f, -100.f});
  }

  // 渲染
  render.Rendering(lights, components);

  // 输出到图片
  TGAImage image(df::kWidth, df::kHeight, TGAImage::RGB);
  const auto &frame = render.GetFrame();
  for (int x = 0; x < df::kWidth; ++x) {
    for (int y = 0; y < df::kHeight; ++y) {
      image.set(x, y, frame[y][x]);
    }
  }
  image.flip_vertically();
  image.write_tga_file("output.tga");

  return 0;
}
