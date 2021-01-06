#version 400

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	gl_FragColor = vec4(col[int(true)] / 255.0, 1.0);
}
