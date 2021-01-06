#version 400

in vec3 _pos;
in vec2 _st;

uniform sampler2D tex;

void main() {
	gl_FragColor = texture(tex, _st);
}
