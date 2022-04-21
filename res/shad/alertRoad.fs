#version 400

in float _mag;

uniform unsigned int t;
uniform bool active;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 142, 65)
);

void main() {
	if (active) {
		if (mod(_mag + (float(t) / 1000), 2.0) > 1.0) {
			gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
		} else {
			discard;
		}
	} else {
		discard;
	}
}
