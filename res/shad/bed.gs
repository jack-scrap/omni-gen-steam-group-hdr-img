#version 400

layout (points) in;

layout (triangle_strip, max_vertices = 36) out;

out vec3 _pos;

uniform mat4
	proj,
	view,
	model;

float
	pad = 0.2,
	stroke = 0.32,

	outer = pad + stroke,

	wd = 2.0,
	ht = 4.0;

void main() {
	float vtc[6 * (2 * 3)] = float[6 * (2 * 3)](
		-ht, -pad, -wd,
		-ht, pad, -wd,

		// corner
		ht - pad, -pad, -wd,
		ht - pad, pad, -wd,

		ht, -pad, -wd + pad,
		ht, pad, -wd + pad,

		ht, -pad, wd - pad,
		ht, pad, wd - pad,

		ht - pad, -pad, wd,
		ht - pad, pad, wd,

		-ht, -pad, wd,
		-ht, pad, wd
	);

	// draw
	for (int i = 0; i < 6 - 1; i++) {
		int start = i * 2;

		for (int v = 0; v < 2 * 2; v++) {
			int idx = (start + v) * 3;

			gl_Position = proj * view * model * (gl_in[0].gl_Position + vec4(
				vec3(
					vtc[idx],
					vtc[idx + 1],
					vtc[idx + 2]
				),
				0.0
			));
			_pos = gl_Position.xyz;
			EmitVertex();   
		}
		EndPrimitive();
	}

	/* quad */
	float vtcQuad[(2 * 2) * 3];
	int i0 = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vtcQuad[i0] = bool(x) ? (ht - pad) : -ht;
			vtcQuad[i0 + 1] = 0.0;
			vtcQuad[i0 + 2] = (bool(z) ? 1 : -1) * wd;

			i0 += 3;
		}
	}

	// beveled
	float vtcEdge[(2 * 2) * 3];
	int i1 = 0;
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vtcEdge[i1] = bool(x) ? ht : (ht - pad);
			vtcEdge[i1 + 1] = 0.0;
			vtcEdge[i1 + 2] = ((bool(z) ? 1 : -1) * wd) + ((bool(x) ? (bool(z) ? -1 : 1) : 0) * pad);

			i1 += 3;
		}
	}

	for (int y = 0; y < 2; y++) {
		for (int i = 0; i < vtcQuad.length(); i += 3) {
			gl_Position = proj * view * model * (gl_in[0].gl_Position + vec4(
				vec3(
					vtcQuad[i],
					vtcQuad[i + 1] + ((bool(y) ? 1 : -1) * pad),
					vtcQuad[i + 2]
				),
				0.0
			));
			_pos = gl_Position.xyz;
			EmitVertex();   
		}
		EndPrimitive();

		for (int i = 0; i < vtcEdge.length(); i += 3) {
			gl_Position = proj * view * model * (gl_in[0].gl_Position + vec4(
			vec3(
				vtcEdge[i],
				vtcEdge[i + 1] + ((bool(y) ? 1 : -1) * pad),
				vtcEdge[i + 2]
			),
			0.0
			));
			_pos = gl_Position.xyz;
			EmitVertex();   
		}
		EndPrimitive();
	}
}
