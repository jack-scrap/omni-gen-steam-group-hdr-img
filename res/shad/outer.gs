#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 32) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform unsigned int sz;

out vec3 _pos;

void main() {
	vec3[2 * 2] vtc = vec3[2 * 2](
		vec3(0.0, 0.0, 0.0),
		vec3(10.0, 0.0, 0.0),
		vec3(0.0, 0.0, 10.0),
		vec3(10.0, 0.0, 10.0)
	);

	for (int i = 0; i < 2 * 2; i++) {
		gl_Position = proj * view * model * vec4(vtc[i], 1.0);
		_pos = gl_Position.xyz;

		EmitVertex();
	}

	EndPrimitive();
}
