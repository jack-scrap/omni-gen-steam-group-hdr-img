#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 12) out;

out vec3 _pos;
out float _mag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

const float pad = 0.16;
const float stroke = pad * 2;

float wd = 16.0;

vec2 norm(vec2 vec) {
	return normalize(vec2(-vec.y, vec.x));
}

void main() {
	vec2 orth = norm(gl_in[1].gl_Position.xz - gl_in[0].gl_Position.xy);

	for	(int z = 0; z < 2; z++) {
		for	(int i = 0; i < 2; i++) {
			for	(int b = 0; b < 2; b++) {
				gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + ((b + ((bool(z) ? 1 : -1) * (wd / 2))) * vec3(orth.x, 0.0, orth.y) * stroke), 1.0);
				_pos = vec3(model * vec4(gl_in[i].gl_Position.xyz + ((b + ((bool(z) ? 1 : -1) * (wd / 2))) * vec3(orth.x, 0.0, orth.y) * stroke), 1.0));

				EmitVertex();
			}
		}

		_mag = length(_pos.xz);

		EndPrimitive();
	}
}
