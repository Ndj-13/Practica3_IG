#version 330 core

layout(location = 0) in vec3 inPos;	
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent; 

uniform mat4 modelViewProj;
uniform mat4 modelView;
uniform mat4 normal;

out vec3 vcolor;
out vec3 vpos;
out vec3 vnormal; //vector normal --> azul: eje perpendicular a la textura
out vec2 texCoord;

out vec3 vTangent; //vector tangente --> rojo: derecha de la textura
out vec3 vbiTangent; //vector bitangente --> verde: parte de arriba

void main()
{
	vcolor = inColor;
	texCoord = inTexCoord;
	vnormal = normalize((normal * vec4(inNormal, 0.0)).xyz);
	vpos = (modelView * vec4(inPos, 1.0)).xyz;

	//BUMP MAPPING

	//Transformar V en espacio tangente
	vTangent = normalize((modelView * vec4(inTangent, 0.0)).xyz);

	//Calcular la bitangente B = (N x T) * T.w
	vec3 vbiTangent = normalize(cross (vnormal, vTangent));
	
	gl_Position =  modelViewProj * vec4 (inPos,1.0);
}
