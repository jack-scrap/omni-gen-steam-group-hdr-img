#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 4) out;

uniform mat4
	model,
	view,
	proj;

float pad = 0.16;

void main() {
	for	(int i = 0; i < 2; i++) {
		for	(int b = 0; b < 2; b++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + vec3(0.0, 0.0, b * pad * 2), 1.0);

			EmitVertex();
		}
	}
}  
