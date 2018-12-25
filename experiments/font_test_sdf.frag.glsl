#version 330

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D text;
uniform vec4 color;

void main() {
  frag_color = frag_color = texture(text, tex_coords);
  if (frag_color.r < 0.5) discard;
  frag_color = color * vec4(1.0, 1.0, 1.0, 1.0);
}

