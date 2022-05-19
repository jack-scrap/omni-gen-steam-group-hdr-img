#version 400

in vec3 _pos;

uniform bool active;

const float ambi = 0.6;

vec3 sun = vec3(1.0, 1.0, -1.0);

vec3[2] col = vec3[2](
	vec3(38, 38, 38),
	vec3(255, 142, 65)
);

void main() {
	vec3 normFace = normalize(cross(dFdx(_pos), dFdy(_pos)));

	float ins = max(dot(normFace, normalize(sun)), 0.0);

	gl_FragColor = vec4((ambi + (ins * (1.0 - ambi))) * (col[int(active)] / 255.0), 1.0);
}
