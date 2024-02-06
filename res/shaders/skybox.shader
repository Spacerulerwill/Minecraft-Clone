#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 

#shader fragment

#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube day_skybox;
uniform samplerCube night_skybox;
uniform samplerCube transition_skybox;
uniform float current_day_progress;

void main()
{    
	vec4 day = texture(day_skybox, TexCoords);
    vec4 night = texture(night_skybox, TexCoords);
	vec4 transition = texture(transition_skybox, TexCoords);
	
	// 0 - 0.5 is day time
	if (current_day_progress <= 0.5) {
		FragColor = day;
	}
	// 0.5 - 0.55 is sunset
	else if (current_day_progress <= 0.55) {
		float sunset_progress = (current_day_progress - 0.5) / 0.05;
		FragColor = mix(day, transition, sunset_progress);
	} 
	// 0.55 - 0.59 is sunet to black
	else if (current_day_progress <= 0.59) {
		float fade_to_black_progress = (current_day_progress - 0.55) / 0.04;
		FragColor = transition * (1.0 - fade_to_black_progress);
	}
	// 0.59 - 0.6 is black to night sky
	else if (current_day_progress <= 0.6) {
		float black_to_night_progress = (current_day_progress - 0.59) / 0.01;
		FragColor = night * black_to_night_progress;
	}
	// 0.6- 0.9 is night time
	else if (current_day_progress <= 0.9) { 
		float night_progress = (current_day_progress - 0.6) / 0.3;
		FragColor = night;
	} 
	// 0.9 - 0.91 is night to black
	else if (current_day_progress <= 0.91) {
		float night_to_black_progress = (current_day_progress - 0.90) / 0.01;
		FragColor = night * (1.0 - night_to_black_progress); 
	}
	// 0.91 - 0.95 is black to sunrise
	else if (current_day_progress <= 0.95) {
		float black_to_sunrise_progress = (current_day_progress - 0.91) / 0.04;	
		FragColor = transition * black_to_sunrise_progress; 
	}
	// rest is sunrise
	else {
		float sunrise_progress = (current_day_progress - 0.95) / 0.05;
		FragColor = mix(day, transition, 1.0 - sunrise_progress);
	}
 }
