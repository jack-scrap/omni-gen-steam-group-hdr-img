#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

void main() {
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			gl_Position = vec4(gl_in[0].gl_Position.xyz + vec3(x, y, 0.0), 1.0);

			EmitVertex();
		}
	}
}
