#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 100) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

vec2 sz = vec2(
	2,
	-4
);

float thick = 0.2 * 2;

float pad = 0.16 * 2;

void main() {
	for (int b = 0; b < 2; b++) {
		for (int z = 0; z < 2; z++) {
			gl_Position = proj * view * model * vec4(
				gl_in[0].gl_Position.xyz + vec3(
					(sz.x / 2) + (b * pad),
					0.0,
					(z * sz.y) - (int(bool(b) && bool(z)) * pad)
				),
				1.0
			);
			_pos = gl_Position.xyz;

			EmitVertex();
		}
	}

	EndPrimitive();
}
