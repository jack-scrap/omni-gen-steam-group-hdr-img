#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 4) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

float fac = 0.16 * 2;

void main() {
	for (int i = 0; i < 2; i++) {
		for (int z = 0; z < 2; z++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + vec3(0.0, 0.0, z * fac), 1.0);

			_pos = gl_Position.xyz;
			EmitVertex();   
		}
	}
}  
