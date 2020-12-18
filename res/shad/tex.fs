#version 400

in vec3 _pos;
in vec2 _st;
out vec4 col;

uniform sampler2D tex;

void main() {
	col = texture(tex, _st);
}
