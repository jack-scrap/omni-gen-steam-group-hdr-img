#version 400

in vec3 _posNoTrans;

uniform unsigned int t;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	if (mod(_posNoTrans.y - (float(t) / 1000.0), 1.0) > 0.5) {
		gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
	}
}
