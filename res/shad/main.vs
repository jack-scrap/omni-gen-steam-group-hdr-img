#version 400

in vec3 pos;

uniform vec3 loc;

uniform mat4
	model,
	view,
	proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);
}
