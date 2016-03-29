#version 400
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
//layout(location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out mat3 TangentToWorldMatrix;
out vec3 ViewDirection;
out vec3 WorldNormal;

uniform mat4 WorldMatrix;
uniform mat4 WVPMatrix;
uniform vec3 CameraPosition;

void main()
{
	TexCoords = aTexCoords;
	
	vec3 tangent = normalize(vec3(WorldMatrix * vec4(aTangent, 0.0)));
	vec3 normal = normalize(vec3(WorldMatrix * vec4(aNormal, 0.0)));
	// Ensure TBN vectors are orthogonal using Gram-Schmidt
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(tangent, normal);
	TangentToWorldMatrix = mat3(tangent, bitangent, normal);
	
	vec4 posV4 = vec4(aPosition, 1.0);
	vec4 worldPos = WorldMatrix * posV4;
	ViewDirection = normalize(CameraPosition - worldPos.xyz);

	WorldNormal = normal;

	gl_Position = WVPMatrix * posV4;
}
