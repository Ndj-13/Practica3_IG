#version 330 core

out vec4 outColor;

in vec3 vcolor1;
in vec3 vpos1;
in vec3 vnormal1;

in vec3 vcolor2;
in vec3 vpos2;
in vec3 vnormal2;

in vec3 vcolor3;
in vec3 vpos3;
in vec3 vnormal3;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
vec3 Ke;


//Propiedades de la luz
vec3 Ia = vec3 (0.3);
//le damos el valor de intensidad de luz a Id e Is:
vec3 Id = vec3(0.8);
vec3 Is = vec3(0.8);

vec3 pf = vec3(0.0, 0.0, 0.0);
vec3 pf2 = vec3(0.0, 0.0, -5.5); //segundo foco

vec3 D = vec3 (0.0, 0.0, -1.0);
float alpha = cos(radians(20.0));
float e = 3.0;
float fdir = 0.0;
float fdir2 = 0.0;
float fdir3 = 0.0;

vec3 pp;
vec3 pp2;
vec3 pp3;

float n;

//Atenuacion
float fdist = 0;
float d0 = 1.0;
//dmin = d0 = 1
float fwin = 0;
float dmax = 50.0;

vec3 shade();

void main()
{
	Ka = vcolor1;
	Kd = vcolor2;
	Ke = vcolor3;
	Ks = vec3 (1.0);

	N1 = vnormal1;
	N2 = vnormal2;
	N3 = vnormal3;

	pp = vpos1;
	pp2 = vpos2;
	pp3 = vpos3;

	n = 50;
	
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	vec3 c = vec3(0.0);
	
	//Luz focal
	float dotFocal = dot (normalize (pp-pf2), normalize(D));
	float dotFocal2 = dot (normalize (pp2-pf2), normalize(D));
	float dotFocal3 = dot (normalize (pp3-pf2), normalize(D));
	if (alpha < dotFocal)
	{
		fdir = pow ((dotFocal - alpha)/(1.0 - alpha), e);
	}
	if (alpha < dotFocal2)
	{
		fdir2 = pow ((dotFocal2 - alpha)/(1.0 - alpha), e);
	}
	if (alpha < dotFocal3)
	{
		fdir3 = pow ((dotFocal3 - alpha)/(1.0 - alpha), e);
	}

	c = Ia * Ka;

	//difusa
	vec3 L = normalize (pf - pp);
	vec3 L2 = normalize (pf - pp2); //segundo cubo
	vec3 L3 = normalize (pf - pp3); //tercer cubo
	c += Id * Kd * dot(N1, L);
	c += Id * Kd * dot(N2, L2);
	c += Id * Kd * dot(N3, L3);

	//difusa segundo foco
	vec3 L4 = normalize (pf2 - pp);
	vec3 L5 = normalize (pf2 - pp2);
	vec3 L6 = normalize (pf2 - pp3);
	c += Id * fdir * Kd * dot(N1, L4);
	c += Id * fdir2 * Kd * dot(N2, L5);
	c += Id * fdir3 * Kd * dot(N3, L6);

	//Especular
	vec3 V = normalize (-pp);
	vec3 V2 = normalize (-pp2);
	vec3 V3 = normalize (-pp3);
	vec3 R = normalize (reflect (-L,N1));
	vec3 R2 = normalize (reflect (-L2,N2));
	vec3 R3 = normalize (reflect (-L3,N3));
	c += Is * Ks * pow(max(dot(R, V), 0), n);
	c += Is * Ks * pow(max(dot(R2, V2), 0), n);
	c += Is * Ks * pow(max(dot(R3, V3), 0), n);

	//especular segundo foco
	vec3 R4 = normalize (reflect (-L4, N1));
	vec3 R5 = normalize (reflect (-L5, N2));
	vec3 R6 = normalize (reflect (-L6, N3));
	c += Is * fdir *Ks * pow(max(dot(R4, V), 0), n);
	c += Is * fdir2 *Ks * pow(max(dot(R5, V2), 0), n);
	c += Is * fdir3 *Ks * pow(max(dot(R6, V3), 0), n);

	c = clamp(c, 0, 1);
	
	return c;
}
