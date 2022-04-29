#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform unsigned int sz;

out vec3 _pos;

const float pad = 0.16;
const float margin = 0.16;
const float stroke = pad * 2;

const float ht = -(0.2 * 2);

const float[3] idx = float[3](
	2,
	2,
	4
);

float thick = 0.2;

float item(float bound) {
	float _ = bound + (margin * 2);

	return _;
}

float padded(float bound) {
	float _ = bound + (pad * 2);

	return _;
}

float bordered(float bound) {
	return item(bound) + padded(stroke * 2);
}

void main() {
	float[3] outer = float[3](
		bordered(bordered(idx[0])),
		0.0,
		bordered(bordered(idx[2]))
	);

	vec3[4] strip = vec3[4](
		vec3(-(outer[0] / 2), 0.0, 0.0),
		vec3(-(outer[0] / 2), 0.0, -outer[2]),
		vec3((outer[0] / 2), 0.0, -outer[2]),
		vec3((outer[0] / 2), 0.0, 0.0)
	);

	for (int i = 0; i < 4 - 1; i++) {
		for (int l = 0; l < 2; l++) {
			for (int b = 0; b < 2; b++) {
				vec3 vtx = strip[i + l] + vec3(0.0, b * -((thick * 2) + thick), 0.0);

				gl_Position = proj * view * model * vec4(vtx, 1.0);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
