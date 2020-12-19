#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4
	proj,
	view,
	model;

vec2 sz = vec2(
	2,
	-4
);

float
	fac = 0.1 * 2,
	thick = -0.2 * 2;

void main() {
	/* generate */
	// vertices
	vec3 quad[(2 * 2) + 2];

	int i = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vec3 sheer;
			if (bool(z)) {
				for (int b = 0; b < 2; b++) {
					sheer = vec3(
						(b * (bool(x) ? 1 : -1) * (bool(b) ? 1 : -1)) * -fac,
						0,
						(bool(b) ? 0 : 1) * fac
					);

					quad[i] = vec3(
						(bool(x) ? 1 : -1) * (sz.x / 2),
						0,
						z * sz.y
					) + sheer;

					i++;
				}
			} else {
				quad[i] = vec3(
					(bool(x) ? 1 : -1) * (sz.x / 2),
					0,
					z * sz.y
				);

				i++;
			}
		}
	}

	// indices
	unsigned short idc[2 * 2 * 2] = unsigned short[2 * 2 * 2](
		0, 1,
		2, 4,

		2, 4,
		3, 5
	);

	// draw
	// extrude
	for (int y = 0; y < 2; y++) {
		for (int q = 0; q < 2; q++) {
			for (int i = 0; i < 2 * 2; i++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + quad[idc[(q * 2 * 2) + i]] + vec3(0, y * thick, 0),
					1.0
				);
				EmitVertex();   
			}

			EndPrimitive();
		}
	}

	// fill
	for (int q = 0; q < 2; q++) {
		for (int i = 0; i < 2 * 2; i++) {
			for (int y = 0; y < 2; y++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + quad[idc[(q * 2 * 2) + i]] + vec3(0, y * thick, 0),
					1.0
				);
				EmitVertex();   
			}
		}
	}
}  
