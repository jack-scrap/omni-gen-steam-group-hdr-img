#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 100) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

float
	pad = 0.16,
	stroke = pad * 2;

vec2 idx = vec2(
	2 + stroke,
	4 + stroke
);

float thick = -(0.2 * 2);

uniform unsigned int sz;

void main() {
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			for (int b = 0; b < 2; b++) {
				for (int z = 0; z < 2; z++) {
					gl_Position = proj * view * model * vec4(
						gl_in[0].gl_Position.xyz + vec3(
							(bool(x) ? 1 : -1) * ((idx.x / 2) + (b * stroke)),
							y * thick,
							(z * sz * -idx.y) - (int(bool(b) && bool(z)) * stroke)
						),
						1.0
					);
					_pos = gl_Position.xyz;

					EmitVertex();
				}
			}

			EndPrimitive();
		}

		for (int b = 0; b < 2; b++) {
			for (int x = 0; x < 2; x++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + vec3(
						(bool(x) ? 1 : -1) * ((idx.x / 2) + stroke),
						y * thick,
						(sz * -idx.y) - (b * stroke)
					),
					1.0
				);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + vec3(
						(bool(x) ? 1 : -1) * ((idx.x / 2) + stroke),
						y * thick,
						z * -((sz * idx.y) + stroke)
					),
					1.0
				);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			gl_Position = proj * view * model * vec4(
				gl_in[0].gl_Position.xyz + vec3(
					(bool(x) ? 1 : -1) * ((idx.x / 2) + stroke),
					y * thick,
					(sz * -idx.y) - stroke
				),
				1.0
			);
			_pos = gl_Position.xyz;

			EmitVertex();
		}
	}

	EndPrimitive();
}
