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
	2 + stroke,
	4 + stroke
);

float
	fac = 0.1 * 2,
	thick = -0.2 * 2;

unsigned int sz = 2;

void main() {
	for (int y = 0; y < 2; y++) {
		for (int z = 0; z < 2; z++) {
			for (int x = 0; x < 2; x++) {
				gl_Position = proj * view * model * vec4(
					gl_in[0].gl_Position.xyz + vec3(
						(bool(x) ? 1 : -1) * (idx.x / 2),
						y * thick,
						z * sz * -idx.y
					),
					1.0
				);
				if (!bool(y)) {
					_pos = gl_Position.xyz;
				}
				_st = vec2(x, z);

				EmitVertex();   
			}
		}

		EndPrimitive();
	}
}  
