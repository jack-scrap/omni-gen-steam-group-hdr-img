#version 400

in vec3 pos;

uvec2 res = uvec2(1376, 576);

float ratio = res.x / res.y;

void main() {
	gl_Position = vec4(vec3(pos.x / ratio, pos.z, 0.0), 1.0);
}
