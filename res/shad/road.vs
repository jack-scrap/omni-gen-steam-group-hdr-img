#version 400

in vec3 pos;

out vec3 _pos;

void main() {
	gl_Position = vec4(pos, 1.0);

	_pos = pos;
}
