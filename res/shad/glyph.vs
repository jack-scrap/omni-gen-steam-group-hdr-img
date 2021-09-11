#version 400

in vec3 pos;

uniform mat4 model;

uniform ivec2 res;

float ratio = res.x / res.y;

void main() {
	gl_Position = model * vec4(vec3(pos.x / ratio, pos.y, pos.z), 1.0);
}
