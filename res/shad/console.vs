#version 400

in vec2 pos;
in vec2 st;

out vec2 _st;

void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
	_st = vec2(st.x, 1.0 + (st.y * -1));
}
