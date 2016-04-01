#version 400
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 ViewDirection;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 Normal;

uniform mat4 WorldMatrix;
uniform mat4 WVPMatrix;
uniform vec3 CameraPosition;

void main()
{
	TexCoords = aTexCoords;

	//Tangent = aTangent;
	//Bitangent = aBitangent;
	//Normal = aNormal;

	Tangent = normalize(vec3(WorldMatrix * vec4(aTangent, 0.0)));
	//Bitangent = normalize(vec3(WorldMatrix * vec4(aBitangent, 0.0)));
	Bitangent = normalize(vec3(WorldMatrix * vec4(cross(aNormal, aTangent), 0.0)));
	Normal = normalize(vec3(WorldMatrix * vec4(aNormal, 0.0)));
	
	vec4 posV4 = vec4(aPosition, 1.0);
	vec4 worldPos = WorldMatrix * posV4;
	ViewDirection = normalize(CameraPosition - worldPos.xyz);

	gl_Position = WVPMatrix * posV4;
}
