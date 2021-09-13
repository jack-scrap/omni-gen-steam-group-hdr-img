#version 400

in vec3 pos;

uniform ivec2 res;

float ratio = res.x / res.y;

void main() {
	gl_Position = vec4(vec3(pos.x / ratio, pos.z, 0.0), 1.0);
}
