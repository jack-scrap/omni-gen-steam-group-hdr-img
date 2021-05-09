#version 400

layout (triangles) in;

layout (triangle_strip, max_vertices = 30) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

uniform unsigned int axis;
uniform float fac;

void main() {
	vec3 trans = vec3(0.0);
	trans[axis] = fac;

	// mirror
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 3; i++) {
			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + trans, 1.0);
			_pos = gl_Position.xyz;

			EmitVertex();	 
		}

		EndPrimitive();
	}

	// fill
	for (int i = 0; i < 3; i++) {
		int a = i;
		int b = i + 1 < 3 ? i + 1 : 0;

		gl_Position = proj * view * model * vec4(gl_in[a].gl_Position.xyz, 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();	 

		gl_Position = proj * view * model * vec4(gl_in[b].gl_Position.xyz, 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();	 

		gl_Position = proj * view * model * vec4(gl_in[a].gl_Position.xyz + trans, 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();	 

		gl_Position = proj * view * model * vec4(gl_in[b].gl_Position.xyz + trans, 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();	 
	}

	EndPrimitive();
}
