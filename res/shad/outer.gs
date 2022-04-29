#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform unsigned int sz;

out vec3 _pos;

const float[3] idx = float[3](
	2,
	2,
	4
);

float thick = 0.2;

void main() {
	vec3[2 * 2] quad;
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			quad[i] = vec3((bool(x) ? 1 : -1) * (idx[0] / 2), 0.0, z * -idx[2]);

			i++;
		}
	}

	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 2 * 2; i++) {
			gl_Position = proj * view * model * vec4(quad[i] + (b * vec3(0.0, thick, 0.0)), 1.0);
			_pos = gl_Position.xyz;

			EmitVertex();
		}

		EndPrimitive();
	}
}
