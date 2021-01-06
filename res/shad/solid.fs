#version 400

vec3 active = vec3(255, 155, 71);

void main() {
	gl_FragColor = vec4(active / 255.0, 1.0);
}
