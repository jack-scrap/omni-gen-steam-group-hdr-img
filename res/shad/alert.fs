#version 400

in vec3 _model;

uniform unsigned int t;
uniform bool active;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	if (active) {
		if (mod(_model.x + _model.z + _model.y + (float(t) / 50.0), 2.0) > 1.0) {
			gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
		} else {
			discard;
		}
	} else {
		discard;
	}
}
