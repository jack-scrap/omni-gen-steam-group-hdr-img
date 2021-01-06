#version 400

in vec3 pos;
out vec3 _pos;
out vec3 _posNoTrans;

uniform mat4
	model,
	view,
	proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);

	_pos = vec3((proj * view * model) * vec4(pos, 1.0));
	_posNoTrans = pos;
}
