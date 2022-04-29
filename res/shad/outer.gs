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

void main() {
	vec3[2 * 2] vtc;
	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vtc[i] = vec3(x * -idx[0], 0.0, (bool(z) ? 1 : -1) * (idx[2] / 2.0));

			i++;
		}
	}

	for (int i = 0; i < 2 * 2; i++) {
		gl_Position = proj * view * model * vec4(vtc[i], 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();
	}

	EndPrimitive();
}
