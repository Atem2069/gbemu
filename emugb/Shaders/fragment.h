R"(
#version 460 core


layout(location=0)in vec2 texcoord;
layout(location=0)out vec4 fragColor;

layout(binding=0)uniform sampler2D texSampler;

void main()
{
	fragColor = texture2D(texSampler,texcoord);
}


)"