#version 400

uniform bool active;

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	if (active) {
		gl_FragColor = vec4(col[int(active)] / 255.0, 1.0);
	} else {
		discard;
	}
}
