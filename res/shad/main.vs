#version 400

in vec3 pos;
in vec2 st;
out vec3 _pos;

uniform vec3 loc;

uniform mat4
	model,
	view,
	proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);

	_pos = vec3((proj * view * model) * vec4(pos + vec3(st, 0.0), 1.0));
}
