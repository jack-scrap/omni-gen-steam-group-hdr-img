#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4
	model,
	view,
	proj;

uniform unsigned int sz;

out vec3 _pos;

float
	pad = 0.16,
	thick = -(pad * 2);

float[2] idx = float[2](
	2 + (pad * 2),
	4 + (pad * 2)
);

vec3[2] bevel(vec3[3] strip, float fac) {
	vec3
		u = normalize(strip[2] - strip[0]),
		v = normalize(strip[1] - strip[0]);

	return vec3[2](
		strip[0] + (u * fac),
		strip[0] + (v * fac)
	);
}

void main() {
	// generate quad
	vec3[2 * 2] quad;
	int i = 0;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			quad[i] = vec3(
				(bool(x) ? 1 : -1) * (idx[0] / 2),
				0.0,
				-(y * idx[1] * sz)
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
	vec3 extrude = vec3(0.0, thick, 0.0);

	// face
	unsigned int idcFace[(2 * 2) + 2] = unsigned int[(2 * 2) + 2](
		0,
		1,
		2,
		4,
		3,
		5
	);

	for (int y = 0; y < 2; y++) {
		for (int i = 0; i < (2 * 2) + 2; i++) {
			unsigned int idx = idcFace[i];

			gl_Position = proj * view * model * vec4(
				beveled[idx] + (y * extrude),
				1.0
			);
			_pos = gl_Position.xyz;

			EmitVertex();
		}

		EndPrimitive();
	}

	// strip
	for (int i = 0; i < roof - 1; i++) {
		for (int p = 0; p < 2; p++) {
			for (int y = 0; y < 2; y++) {
				unsigned int idx = idcStrip[(i + p) % roof];

				gl_Position = proj * view * model * vec4(
					beveled[idx] + (y * extrude),
					1.0
				);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
