#version 400

in vec3 _pos;
in vec2 _st;

uniform sampler2D tex;

const float ambi = 0.6;

vec3 sun = vec3(1.0, 1.0, 1.0);

void main() {
	vec3 normFace = normalize(cross(dFdx(_pos), dFdy(_pos)));

	float ins = ambi + (max(dot(normFace, normalize(sun)), 0.0) * (1.0 - ambi));

	gl_FragColor = vec4(ins * texture(tex, _st).rgb, 1.0);
}
