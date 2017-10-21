//
//  main.cpp
//  wizardess
//
//  Created by Aldo Moore on 15/02/2014.
//  Copyright (c) 2014 Aldo Moore. All rights reserved.
//

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#undef __gl_h_
#include <OpenGL/gl3.h>

#include <boost/format.hpp>

#include <png.h>
#define PNG_SIG_LEN 8

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <vector>

#define SCREEN_W 640
#define SCREEN_H 480

void read_png_data(png_struct* png_p, png_byte* data, png_size_t len) {
  void* png_v = png_get_io_ptr(png_p);
  ((std::istream*)png_v)->read((char*)data, len);
}

GLuint load_image(const std::string& path, int* w, int* h) {
  std::ifstream ifs(path, std::ios::in | std::ios::binary | std::ios::ate);
  std::streamsize ifs_size;
  if (ifs.seekg(0, std::ios::end).good())
    ifs_size = ifs.tellg();
  if (ifs.seekg(0, std::ios::beg).good())
    ifs_size -= ifs.tellg();
  
  png_byte head[PNG_SIG_LEN];
  ifs.read((char*)head, PNG_SIG_LEN);
  if (!ifs.good())
    return 0;
  if (png_sig_cmp(head, 0, PNG_SIG_LEN))
    return 0;
  
  png_struct* png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_p)
    return 0;
  
  png_info* info_p = png_create_info_struct(png_p);
  if (!info_p)
    return 0;
  
  if (setjmp(png_jmpbuf(png_p)))
    return 0;
  
  png_set_read_fn(png_p, (void*)&ifs, read_png_data);
  png_set_sig_bytes(png_p, PNG_SIG_LEN);
  png_read_info(png_p, info_p);
  
  int bit_depth, color_type;
  png_uint_32 tmp_w, tmp_h;
  png_get_IHDR(png_p, info_p, &tmp_w, &tmp_h, &bit_depth, &color_type, nullptr, nullptr, nullptr);
  png_uint_32 channels = png_get_channels(png_p, info_p);
  
  if (w) *w = tmp_w;
  if (h) *h = tmp_h;
  
  GLuint format;
  switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(png_p);
      channels = 3;
      format = GL_RGB;
      break;
      
    case PNG_COLOR_TYPE_GRAY:
      if (bit_depth < 8) {
        png_set_gray_to_rgb(png_p);
        bit_depth = 8;
      }
      format = GL_RGB;
      break;
      
    case PNG_COLOR_TYPE_RGB:
      format = GL_RGB;
      break;
      
    case PNG_COLOR_TYPE_RGB_ALPHA:
      format = GL_RGBA;
      break;
      
    default:
      return 0;
  }
  
  if (bit_depth == 16) {
    png_set_strip_16(png_p);
    bit_depth = 8;
  }
  
  png_byte** rows = new png_byte*[tmp_h];
  png_byte*  data = new png_byte [tmp_w * tmp_h * bit_depth * channels / 8];
  
  const unsigned int row_stride = tmp_w * bit_depth * channels / 8;
  for (unsigned int i = 0; i < tmp_h; ++i)
    rows[i] = (png_byte*)data + ((tmp_h - i - 1) * row_stride);
  
  png_read_image(png_p, rows);
  
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, format, tmp_w, tmp_h, 0, format, GL_UNSIGNED_BYTE, data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  ifs.close();
  delete [] data;
  delete [] (png_byte*)rows;
  png_destroy_read_struct(&png_p, &info_p, (png_info**)0);
  
  return tex;
}

void print_shader_log(GLuint s) {
  if (glIsShader(s)) {
    int log_len = 0, max_len = 0;
    glGetShaderiv(s, GL_INFO_LOG_LENGTH, &max_len);
    char* log = new char[max_len];
    
    glGetShaderInfoLog(s, max_len, &log_len, log);
    if (log_len >  0)
      printf("%s\n", log);
    
    delete [] log;
  }
}

GLuint load_shader(const GLchar* src, GLenum type) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, nullptr);
  glCompileShader(s);
  
  GLint res = GL_FALSE;
  glGetShaderiv(s, GL_COMPILE_STATUS, &res);
  if (!res) {
    print_shader_log(s);
    return 0;
  }
  
  return s;
}

GLuint create_shader(const GLchar* vs_src, const GLchar* fs_src) {
  GLuint sp = glCreateProgram();
  GLuint vs = load_shader(vs_src, GL_VERTEX_SHADER);
  GLuint fs = load_shader(fs_src, GL_FRAGMENT_SHADER);
  glAttachShader(sp, vs);
  glAttachShader(sp, fs);
  glLinkProgram(sp);
  glDeleteShader(vs);
  glDeleteShader(fs);
  return sp;
}

int main(int argc, const char* argv[]) {
  if (!glfwInit())
    throw "Failed to initalize GLFW!";
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* win = glfwCreateWindow(SCREEN_W, SCREEN_H, "", NULL, NULL);
  if (!win) {
    glfwTerminate();
    throw "Failed to create GLFW window!";
  }
  glfwMakeContextCurrent(win);
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  const GLfloat vertices[8] = {
    0.f,   0.f,
    250.f, 0.f,
    0.f,   250.f,
    250.f, 250.f
  };
  
  const GLfloat clip[4] = { 250.f, 250.f, 500.f, 500.f };
  
  int w, h;
  GLuint texture = load_image("/Users/anzu/Desktop/test_atlas.png", &w, &h);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  GLfloat left   = clip[0] / w;
  GLfloat right  = clip[2] / w;
  GLfloat top    = clip[1] / h;
  GLfloat bottom = clip[3] / h;
  
  const GLfloat texture_coord[8] = {
    bottom, right,
    top, right,
    bottom, left,
    top, left
  };
  
  const GLuint indices[6] = {
    0, 1, 2,
    2, 1, 3
  };
  
  const GLchar* vs =
    "#version 150\n"
    "in vec2 position;"
    "uniform mat4 mvp;"
    "in vec2 texture_coord;"
    "out vec2 texture_coord_vs;"
    "void main() {"
    "   gl_Position = mvp * vec4(position, 0, 1);"
    "   texture_coord_vs = vec2(texture_coord.s, 1.0 - texture_coord.t);"
    "}";
  
  const GLchar* fs =
    "#version 150\n"
    "in vec2 texture_coord_vs;"
    "out vec4 out_color;"
    "uniform sampler2D texture_sampler;"
    "void main() {"
    "   out_color = texture(texture_sampler, texture_coord_vs);"
    "}";
  
  glm::mat4 proj  = glm::ortho<GLfloat>(0.f, 640.f, 480.f, 0.f, -1.f, 1.f);
  glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(10, 10, 0));
  glm::mat4 mvp   = proj * model;
  
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices + sizeof texture_coord, NULL, GL_STATIC_DRAW);
  
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof vertices, sizeof texture_coord, texture_coord);
  
  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);
  
  GLuint shader  = create_shader(vs, fs);
  glUseProgram(shader);
  
  GLuint mvp_id = glGetUniformLocation(shader, "mvp");
  glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);
  
  model = glm::translate(glm::mat4(), glm::vec3(10, 10, 0));
  mvp   = proj * model;
  glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);
  
  GLint position_attribute = glGetAttribLocation(shader, "position");
  glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attribute);
  
  GLint texture_coord_attribute = glGetAttribLocation(shader, "texture_coord");
  glVertexAttribPointer(texture_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof vertices);
  glEnableVertexAttribArray(texture_coord_attribute);
  
  while (!glfwWindowShouldClose(win)) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  
  glfwDestroyWindow(win);
  glfwTerminate();
  return EXIT_SUCCESS;
}
