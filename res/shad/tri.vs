#version 400

in vec2 pos;

void main() {
	gl_Position = vec4(vec3(pos, 0.0), 1.0);
}
