#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 4) out;

out vec3 _obj;
out float _mag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

const float pad = 0.16;

vec2 norm(vec2 vec) {
	return normalize(vec2(-vec.y, vec.x));
}

float hyp(vec2 tri) {
	float total = pow(tri.x, 2) + pow(tri.y, 2);

	return sqrt(total);
}

void main() {
	vec2 orth = norm(gl_in[1].gl_Position.xz - gl_in[0].gl_Position.xy);

	for	(int i = 0; i < 2; i++) {
		for	(int b = 0; b < 2; b++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + ((bool(b) ? 1 : -1) * vec3(orth.x, 0.0, orth.y) * pad), 1.0);
			_obj = gl_in[i].gl_Position.xyz + ((bool(b) ? 1 : -1) * vec3(orth.x, 0.0, orth.y) * pad);
			_mag = hyp(gl_Position.xz);

			EmitVertex();
		}
	}
}
