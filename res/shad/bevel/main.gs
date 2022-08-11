#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 16) out;

out vec3 _pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec2 boundX;
uniform vec2 boundZ;

const float pad = 0.16;

void main() {
	// front, back
	for	(int z = 0; z < 2; z++) {
		for	(int y = 0; y < 2; y++) {
			for	(int x = 0; x < 2; x++) {
				vec3 pos = vec3(
					boundX[x],
					(bool(y) ? 1 : -1) * (2 * pad),
					boundZ[z]
				);

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
				vec3 pos = vec3(
					boundX[x],
					(bool(y) ? 1 : -1) * (2 * pad),
					boundZ[z]
				);

				gl_Position = proj * view * model * vec4(gl_in[0].gl_Position.xyz + pos, 1.0);

				_pos = vec3(model * vec4(pos, 1.0));

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
