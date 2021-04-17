#version 400

in vec3 _pos;

uniform unsigned int t;
uniform bool active;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	if (active) {
		if (mod(_pos.x + _pos.y + (float(t) / 100.0), 2.0) > 1.0) {
			gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
		} else {
			discard;
		}
	} else {
		discard;
	}
}
