#version 400 core
#define MAX_ITERATION 1000
#define PI 3.14159265
layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
layout(location = 0) out vec4 diffuseColor;
uniform dvec4 TLBR;
uniform dvec2 screendims;
//uniform vec3 palette[MAX_ITERATION];
void main()
{
	double x0 = TLBR[0] + (gl_FragCoord.x / screendims.x)*abs(TLBR[2] - TLBR[0]);
    double y0 = TLBR[1]+ (gl_FragCoord.y/screendims.y)*abs(TLBR[3]-TLBR[1]);
    int iteration = 0;
	double x2 = 0;
	double y2 = 0;
	double x = 0;
	double y = 0;
	while (x2 + y2 <= 4 && iteration < MAX_ITERATION) {
		y = 2 * x * y + y0;
		x = x2 - y2 + x0;
		x2 = x * x;
		y2 = y * y;
		iteration++;
	}
	diffuseColor = vec4(
		255.0 * sin(((2 * PI)*(float(iteration) / float(MAX_ITERATION))) / 0.5),
		255.0 * sin(((PI)*(float(iteration) / float(MAX_ITERATION) + 0.66)) / 0.6),
		255.0 * sin(((2 * PI)*(float(iteration) / float(MAX_ITERATION) + 0.33)) / 0.8),
		0
	);
	//diffuseColor = vec4(palette[iteration],0);
	//diffuseColor = mix(vec4(1.0, 1.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), float(iteration) / float(MAX_ITERATION));//vec4(palette[iteration],0);
}