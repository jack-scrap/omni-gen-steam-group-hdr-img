#version 400

uniform mat4 model;

in vec3 pos;

void main() {
	gl_Position = model * vec4(pos, 1.0);
}
