#version 400

in vec2 pos;

uniform vec2 res;

float ratio = res.x / res.y;

void main() {
	gl_Position = vec4(vec2(pos.x / ratio, pos.y), 0.0, 1.0);
}
