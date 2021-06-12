#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

float
	edge = 0.1,
	pad = 0.16,

	side = 1 - (2 * edge);

void main() {
	// front, back
	for	(int t = 0; t < 2; t++) {
		for	(int y = 0; y < 2; y++) {
			for	(int x = 0; x < 2; x++) {
				gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + vec3(
					(bool(x) ? 1 : -1) * side,
					(bool(y) ? 1 : -1) * (2 * pad),
					bool(t) ? 1 : -1
				), 1.0);
				_pos = vec4(gl_in[0].gl_Position.xyz + vec3(
					(bool(x) ? 1 : -1) * side,
					(bool(y) ? 1 : -1) * (2 * pad),
					bool(t) ? 1 : -1
				), 1.0).xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	// left, right
	for	(int t = 0; t < 2; t++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + vec3(
					bool(t) ? 1 : -1,
					(bool(y) ? 1 : -1) * (2 * pad),
					(bool(z) ? 1 : -1) * side
				), 1.0);
				_pos = (proj * view * model * vec4(gl_in[0].gl_Position.xyz + vec3(
					bool(t) ? 1 : -1,
					(bool(y) ? 1 : -1) * (2 * pad),
					(bool(z) ? 1 : -1) * side
				), 1.0)).xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}

	// corner
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			for (int h = 0; h < 2; h++) {
				for (int v = 0; v < 2; v++) {
					gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + vec3(
						(bool(x) ? 1 : -1) * (1 - edge),
						0.0,
						(bool(z) ? 1 : -1) * (1 - edge)
					) + vec3(
						(bool(h) ? 1 : -1) * ((bool(x) ? 1 : -1) * edge),
						(bool(v) ? 1 : -1) * (2 * pad),
						(bool(h) ? 1 : -1) * ((bool(z) ? -1 : 1) * edge)
					), 1.0);
					_pos = (proj * view * model * vec4(gl_in[0].gl_Position.xyz + vec3(
						(bool(x) ? 1 : -1) * (1 - edge),
						0.0,
						(bool(z) ? 1 : -1) * (1 - edge)
					) + vec3(
						(bool(h) ? 1 : -1) * ((bool(x) ? 1 : -1) * edge),
						(bool(v) ? 1 : -1) * (2 * pad),
						(bool(h) ? 1 : -1) * ((bool(z) ? -1 : 1) * edge)
					), 1.0)).xyz;

					EmitVertex();
				}
			}

			EndPrimitive();
		}
	}
}
