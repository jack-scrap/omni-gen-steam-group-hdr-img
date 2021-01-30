#version 400

in vec3 _pos;

uniform bool active;

vec3
	ambient = vec3(0.2, 0.2, 0.2),
	sun = vec3(1.0, 1.0, 1.0);

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 155, 71)
);

void main() {
	vec3 normFace = normalize(cross(dFdx(_pos), dFdy(_pos)));

	float i = max(dot(normFace, normalize(sun)), 0.0);

	gl_FragColor = vec4(i * (col[int(active)] / 255.0), 1.0);
}
