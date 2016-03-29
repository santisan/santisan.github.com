#version 400
in vec2 TexCoords;
in mat3 TangentToWorldMatrix;
in vec3 ViewDirection;
in vec3 WorldNormal;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;

layout(location = 0) out vec4 oFragColor;

void main()
{
	vec3 L = normalize(vec3(1.0, -1.0, 1.0));

	vec3 diffuseColor = texture(DiffuseMap, TexCoords).rgb;
	vec3 N = texture(NormalMap, TexCoords).rgb;
	N = normalize(N * 2.0 - 1.0);
	N = normalize(TangentToWorldMatrix * N);
	
	float specularPower = texture(SpecularMap, TexCoords).r * 64.0;
	vec3 R = reflect(L, N);
	float specular = pow(dot(R, normalize(ViewDirection)), specularPower);

	vec3 ambient = vec3(0.16, 0.16, 0.16);

	vec3 color = (dot(N, -L) + specular) * diffuseColor + ambient;

	oFragColor = vec4(color, 1.0);

	oFragColor = vec4(N, 1.0);
	//oFragColor = vec4(normalize(WorldNormal), 1.0);
}
