#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

out vec3 _pos;

uniform mat4
	proj,
	view,
	model;

vec2 sz = vec2(
	2,
	-4
);

float thick = 0.2 * 2;

void main() {
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + vec3(
						bool(x) ? 1 : -1,
						y * thick,
						z * sz.y
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
					bool(x) ? 1 : -1,
					y * thick,
					sz.y
				),
				1.0
			);
			_pos = gl_Position.xyz;

			EmitVertex();
		}
	}
}
