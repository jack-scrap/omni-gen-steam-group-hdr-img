#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 20) out;

out vec3 _pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec2 bound[2];

const float pad = 0.16;

const float fac = 0.32;

void main() {
	// front, back
	for	(int z = 0; z < 2; z++) {
		for	(int y = 0; y < 2; y++) {
			for	(int x = 0; x < 2; x++) {
				vec3 pos = vec3(bound[0][x] - ((bool(x) ? 1 : -1) * fac), (bool(y) ? 1 : -1) * (2 * pad), bound[1][z]);

				gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + pos, 1.0);

				_pos = vec3(model * vec4(pos, 1.0));

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	// left, right
	for	(int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				vec3 pos = vec3(bound[0][x], (bool(y) ? 1 : -1) * (2 * pad), bound[1][z] - ((bool(z) ? 1 : -1) * fac));

				gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + pos, 1.0);

				_pos = vec3(model * vec4(pos, 1.0));

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	// corners
	vec3 corner = vec3(bound[0][0], 0.0, bound[0][0]);

	for (int y = 0; y < 2; y++) {
		for (int c = 0; c < 2; c++) {
			vec3 shear = vec3(0.0, (bool(y) ? 1 : -1) * pad * 2, 0.0);
			shear[bool(c) ? 0 : 2] = fac;

			vec3 pos = corner + shear;

			gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + pos, 1.0);

			_pos = vec3(model * vec4(pos, 1.0));

			EmitVertex();
		}
	}

	EndPrimitive();
}
