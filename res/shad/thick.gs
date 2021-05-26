#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 4) out;

uniform mat4
	model,
	view,
	proj;

vec2 norm(vec2 vec) {
	return normalize(vec2(-vec.y, vec.x));
}

void main() {
	vec2 vec = vec2(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x, gl_in[1].gl_Position.z - gl_in[0].gl_Position.z);
	vec2 orth = norm(vec);

	for	(int i = 0; i < 2; i++) {
		for	(int b = 0; b < 2; b++) {
			vec3 pad = vec3((bool(b) ? 1 : -1) * orth.x, 0.0, orth.y);

			gl_Position = proj * view * model * vec4(gl_in[i].gl_Position.xyz + pad, 1.0);

			EmitVertex();
		}
	}
}  
