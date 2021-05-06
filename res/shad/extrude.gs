#version 400

layout (triangles) in;

layout (triangle_strip, max_vertices = 12) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

float fac = 0.16 * 2;

void main() {
	// draw
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 3; i++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + vec3(0.0, b * fac, 0.0), 1.0);

			_pos = gl_Position.xyz;
			EmitVertex();   
		}
		EndPrimitive();
	}  
}  
