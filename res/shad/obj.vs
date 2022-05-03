#version 400

in vec3 pos;
in vec2 st;
out vec3 _pos;
out vec3 _obj;
out vec2 _st;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);

	_pos = gl_Position.xyz;
	_obj = pos;
	_st = vec2(st.x, 1.0 + (st.y * -1));
}
