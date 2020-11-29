#version 400

in vec3 pos;

uniform vec3 loc;

void main() {
  gl_Position = vec4(pos, 1.0);
}
