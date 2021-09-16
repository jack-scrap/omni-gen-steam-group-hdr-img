#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 12) out;

out vec3 _local;
out vec3 _mag;

uniform mat4
	model,
	view,
	proj;

float
	pad = 0.16,
	wd = 16.0;

vec2 norm(vec2 vec) {
	return normalize(vec2(-vec.y, vec.x));
}

float hyp(vec2 tri) {
	float total = pow(tri.x, 2) + pow(tri.y, 2);

	return sqrt(total);
}

void main() {
	vec2 orth = norm(gl_in[1].gl_Position.xz - gl_in[0].gl_Position.xy);

	for	(int z = 0; z < 2; z++) {
		for	(int i = 0; i < 2; i++) {
			for	(int b = 0; b < 2; b++) {
				gl_Position = proj * view * model * vec4(
					gl_in[i].gl_Position.xyz + ((b + ((bool(z) ? 1 : -1) * (wd / 2))) * vec3(orth.x, 0.0, orth.y) * pad * 2),
					1.0
				);
				_local = gl_in[i].gl_Position.xyz + (b * vec3(orth.x, 0.0, orth.y) * pad * 2);
				_mag = vec3(hyp(gl_Position.xz));

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
