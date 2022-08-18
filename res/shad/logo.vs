#version 400

in vec3 pos;

uniform uvec2 res;

float ratio = float(res.x) / float(res.y);

const float max = 0.26;
const float scale = 1 / 0.26;

void main() {
	gl_Position = vec4(vec3(pos.x / ratio, pos.z, 0.0) * scale, 1.0);
}
