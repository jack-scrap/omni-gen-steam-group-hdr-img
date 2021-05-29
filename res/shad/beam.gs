#version 400

layout (lines) in;

layout (triangle_strip, max_vertices = 24) out;

out vec3 _pos;

uniform mat4
	model,
	view,
	proj;

uniform unsigned int axis;
uniform float ln;

bvec2 dir = bvec2(true, true);
float thick = 0.16;

vec3 norm(vec3[3] tri) {
	vec3
		u = tri[1] - tri[0],
		v = tri[2] - tri[0],

		orth = cross(u, v),

		_ = normalize(orth);

	return _;
}

void main() {
	// extrude (length)
	vec3 end = vec3(0.0);
	end[axis] = ln;

	vec3 rect[2 * 2];
	for (int i = 0; i < 2; i++) {
		for (int b = 0; b < 2; b++) {
			rect[(b * 2) + i] = gl_in[i].gl_Position.xyz + (b * end);
		}
	}

	// normal
	vec3 face = norm(vec3[3](rect[0], rect[1], rect[2]));

	// extrude (thickness)
	vec3 vtc[2 * 2 * 2];
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 2 * 2; i++) {
			vtc[(b * 2 * 2) + i] = rect[i] + ((bool(b) ? 1 : -1) * int(dir[b]) * face * thick);
		}
	}

	/* draw */
	// Y
	for (int b = 0; b < 2; b++) {
		for (int i = 0; i < 2 * 2; i++) {
			gl_Position = proj * view * model * vec4(
				vtc[(b * 2 * 2) + i],
				1.0
			);
			_pos = gl_Position.xyz;

			EmitVertex();
		}

		EndPrimitive();
	}

	// Z
	for (int b = 0; b < 2; b++) {
		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				gl_Position = proj * view * model * vec4(
					vtc[b + (y * 2 * 2) + (x * 2)],
					1.0
				);
				_pos = gl_Position.xyz;

				EmitVertex();
			}
		}

		EndPrimitive();
	}
}
