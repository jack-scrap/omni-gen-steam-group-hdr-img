#version 400

in vec2
	pos,
	st;

uniform vec2 loc;

out vec2 _st;

void main() {
	gl_Position = vec4(pos + loc, 0.0, 1.0);

	_st = st * vec2(1, -1);
}
