#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform unsigned int sz;

out vec3 _pos;

float pad = 0.16;
float stroke = pad * 2;

float thick = -(0.2 * 2);

float[2] idx = float[2](
	2,
	4
);

vec3 norm(vec3[3] tri) {
	vec3
		u = tri[1] - tri[0],
		v = tri[2] - tri[0],

		orth = cross(u, v);

	return normalize(orth);
}

vec3[2] bevel(vec3[3] strip, float fac) {
	vec3
		u = strip[2] - strip[0],
		v = strip[1] - strip[0];

	return vec3[2](
		strip[0] + (normalize(u) * fac),
		strip[0] + (normalize(v) * fac)
	);
}

void main() {
	// generate quad
	vec3[2 * 2] quad;
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			quad[i] = vec3(
				(bool(x) ? 1 : -1) * ((idx[0] / 2) + stroke),
				0.0,
				-(y * ((idx[1] * sz) + stroke))
			);

			i++;
		}
	}

	// bevel
	int q = 0;
	i = 0;
	vec3[(2 * 2) + 2] beveled;
	for	(int y = 0; y < 2; y++) {
		for	(int x = 0; x < 2; x++) {
			if (bool(y)) {
				vec3[3] strip;
				for (int s = 0; s < 3; s++) {
					strip[s] = quad[(q + (s * (bool(x) ? -1 : 1))) % (2 * 2)];
				}
				vec3[2] corner = bevel(strip, 0.16 * 2);
				for	(int b = 0; b < 2; b++) {
					beveled[i] = corner[b];

					i++;
				}
			} else {
				beveled[i] = quad[q];

				i++;
			}

			q++;
		}
	}

	// index
	const unsigned int roof = (2 * 2) + 1 + 1;
	unsigned int[roof] idcStrip = unsigned int[roof](
		0,
		2,
		3,
		5,
		4,
		1
	);

	/* draw */
	// strip
	for (int i = 0; i < roof - 1; i++) {
		vec3[3] tri = vec3[3](
			beveled[i],
			beveled[i + 1],
			beveled[i + 1] + vec3(0.0, thick, 0.0)
		);

		for (int p = 0; p < 2; p++) {
			for (int y = 0; y < 2; y++) {
				unsigned int idx = idcStrip[(i + p) % roof];

				gl_Position = proj * view * model * vec4(
					beveled[idx] + (y * vec3(0.0, thick, 0.0)) + (norm(tri)),
					1.0
				);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
