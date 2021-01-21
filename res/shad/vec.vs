#version 400

in vec2 pos;
out vec2 _pos;
out vec2 _posNoTrans;

uniform mat4
	model,
	view,
	proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);

	_pos = vec2((proj * view * model) * vec4(pos, 0.0, 1.0));
	_posNoTrans = pos;
}
