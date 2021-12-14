R"(
#version 460 core


layout(location=0)in vec2 texcoord;
layout(location=0)out vec4 fragColor;

layout(binding=0)uniform usampler2D texSampler;


void main()
{
	vec4 dmgCols[4]=
	{
		vec4(127.0/255.0,134.0/255.0,15.0/255.0,1),
		vec4(87.0/255.0,124.0/255.0,68.0/255.0,1),
		vec4(54.0/255.0,93.0/255.0,72.0/255.0,1),
		vec4(42.0/255.0,69.0/255.0,59.0/255.0,1)
	};
	vec4 pocketCols[4]=
	{
		vec4(196.0/255.0,207.0/255.0,161.0/255.0,1),
		vec4(139.0/255.0,149.0/255.0,109.0/255.0,1),
		vec4(77.0/255.0,83.0/255.0,60.0/255.0,1),
		vec4(31.0/255.0,31.0/255.0,31.0/255.0,1)
	};
	uint idx = texture(texSampler,texcoord).x;
	fragColor = dmgCols[idx];
}


)"