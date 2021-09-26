#version 400

in vec3 pos;
in vec2 st;
out vec3 _pos;
out vec3 _obj;
out vec2 _st;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);

	_pos = vec3(proj * view * model * vec4(pos, 1.0));
	_obj = pos;
	_st = vec2(0.0, 1.0) + (st * vec2(1, -1));
}
