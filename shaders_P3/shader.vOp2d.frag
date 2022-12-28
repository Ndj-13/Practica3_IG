#version 330 core

out vec4 outColor;

in vec3 vcolor;
in vec3 vpos;
in vec3 vnormal;
in vec2 texCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
//debemos pedir identificador pa decirle en q puerto mirar cada textura

uniform mat4 viewL;
uniform vec3 posicionL;
uniform vec3 intensidadL;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
vec3 Ke;


//Propiedades de la luz
vec3 Ia = vec3 (0.3);
//le damos el valor de intensidad de luz a Id e Is:
vec3 Id = intensidadL.xyz;
vec3 Is = intensidadL.xyz;
vec3 lpos = vec3 (viewL * vec4(posicionL, 1.0)).xyz;

//El segundo foco no lo controlamos por teclado:
vec3 Id2 = vec3(0.8);
vec3 Is2 = vec3(0.8);
vec3 lpos2 = vec3(0.0, 0.0, 0.0); //segundo foco (luz focal)

vec3 D = vec3 (0.0, 0.0, -1.0);
float alpha = cos(radians(20.0));
float e = 3.0;
float fdir = 0.0;

vec3 pp;

float n;

vec3 shade();

void main()
{
	Ka = texture(colorTex, texCoord).rgb;
	Kd = texture(colorTex, texCoord).rgb;
	Ke = texture(emiTex, texCoord).rgb;
	Ks = vec3 (1.0);

	N = vnormal;
	pp = vpos;

	n = 50;
	
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	vec3 c = vec3(0.0);
	
	//Luz focal
	float dotFocal = dot (normalize (pp-lpos2), normalize(D));
	if (alpha < dotFocal)
	{
		fdir = pow ((dotFocal - alpha)/(1.0 - alpha), e);
	}

	c = Ia * Ka;

	//difusa
	vec3 L = normalize (lpos - pp);
	c += Id * Kd * dot(N, L);

	//difusa segundo foco
	vec3 L2 = normalize (lpos2 - pp);
	c += Id2 * fdir * Kd * dot(N, L2);

	//Especular
	vec3 V = normalize (-pp);
	vec3 R = normalize (reflect (-L,N));
	c += Is * Ks * pow(max(dot(R, V), 0), n);

	//especular segundo foco
	vec3 R2 = normalize (reflect (-L2, N));
	c += Is2 * fdir *Ks * pow(max(dot(R2, V), 0), n);

	

	c += Ke;

	c = clamp(c, 0, 1);
	
	return c;
}
