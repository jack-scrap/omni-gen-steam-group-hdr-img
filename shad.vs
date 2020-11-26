#version 400

in vec2 pos;

uniform vec2 loc;

void main() {
  gl_Position = vec4(pos + (loc * 0.1), 0.0, 1.0);
}
