#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 8) out;

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
		bordered(bordered(idx[0])) - (margin * 2),
		0.0,
		bordered(sz * bordered(idx[2])) - (margin * 2)
	);

	vec3[2 * 2] vtc;
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = vec3((bool(x) ? 1 : -1) * (outer[0] / 2), 0.0, z * -outer[2]);

			i++;
		}
	}

	// draw
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 2 * 2; i++) {
			vec3 vtx = vtc[i] + vec3(0.0, b * ht, 0.0);

			gl_Position = proj * view * model * vec4(vtx, 1.0);
			_pos = gl_Position.xyz;

			EmitVertex();
		}

		EndPrimitive();
	}
}
