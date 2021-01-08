#version 400

in vec2
	pos,
	st;

out vec2 _st;

void main() {
	gl_Position = vec4(pos + vec2(-1, 1), 0.0, 1.0);

	_st = st;
}
