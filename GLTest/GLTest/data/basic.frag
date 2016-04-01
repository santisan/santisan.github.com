#version 400
in vec2 TexCoords;
in vec3 ViewDirection;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 Normal;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform float Time;

layout(location = 0) out vec4 oFragColor;

void main()
{
	vec3 L = -normalize(vec3(cos(Time), -1.0, sin(Time)));

	mat3 tangentToWorldMatrix = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));
	vec3 N = texture(NormalMap, TexCoords).rgb;
	N = normalize(N * 2.0 - 1.0);
	N = normalize(tangentToWorldMatrix * N);
	//vec3 N = normalize(Normal);

	vec3 diffuseColor = texture(DiffuseMap, TexCoords).rgb;
	
	float specularPower = texture(SpecularMap, TexCoords).r;
	float shininess = 4.0;
	vec3 H = normalize(ViewDirection + L);
	
	float specular = pow(max(dot(N, H), 0.0), shininess) * specularPower;

	vec3 ambient = vec3(0.16, 0.16, 0.16);

	vec3 color = (vec3(max(dot(N, L), 0.0)) + ambient) * diffuseColor + specular;

	oFragColor = vec4(color, 1.0);

	//oFragColor = vec4(cross(Normal, Tangent), 1.0);
	//oFragColor = vec4(N, 1.0);
}
