#version 400

layout (triangles) in;

layout (triangle_strip, max_vertices = 12) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

uniform unsigned int axis;

void main() {
	for (int b = 0; b < 2; b++) {
		vec3 dir = vec3(1);
		dir[axis] = bool(b) ? 1 : -1;

		for (int i = 0; i < gl_PatchVerticesIn; i++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz * dir, 1.0);

			_pos = gl_Position.xyz;
			EmitVertex();   
		}
		EndPrimitive();
	}  
}  
