#version 400

in vec3 _pos;

uniform unsigned int t;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	if (mod(_pos.y - (float(t) / 1000.0), 0.2) > 0.1) {
		gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
	}
}
