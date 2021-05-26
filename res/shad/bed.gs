#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 8) out;

out vec3 _pos;
out vec2 _st;

uniform mat4
	model,
	view,
	proj;

float
	pad = 0.16,
	stroke = pad * 2;

vec2 idx = vec2(
	2,
	4
);

float
	fac = 0.1 * 2,
	thick = -0.2 * 2;

uniform unsigned int sz;

void main() {
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			gl_Position = proj * view * model * vec4(
				gl_in[0].gl_Position.xyz + vec3(x, 0.0, z),
				1.0
			);

			EmitVertex();
		}
	}
}  
