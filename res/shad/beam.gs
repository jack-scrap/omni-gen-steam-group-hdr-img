#version 400

out vec3 _pos;

layout (lines) in;

layout (triangle_strip, max_vertices = 4) out;

uniform mat4
	model,
	view,
	proj;

void main() {
	for	(int i = 0; i < 2; i++) {
		// length
		for	(int b = 0; b < 2; b++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + vec3(b, 0.0, 0.0), 1.0);
			_pos = gl_Position.xyz;

			EmitVertex();
		}
	}
}  
