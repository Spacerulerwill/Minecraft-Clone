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
	// 0.55 - 0.575 is fade to night
	else if (current_day_progress <= 0.575) {
		float fade_to_night_progress = (current_day_progress - 0.55) / 0.025;
		FragColor = mix(transition, night, fade_to_night_progress);
	}
	// 0.575 - 0.925 is night time
	else if (current_day_progress <= 0.925) { 
		float night_progress = (current_day_progress - 0.575) / 0.35;
		FragColor = night;
	} 
	// 0.925 - 0.95 is night to sunrise
	else if (current_day_progress <= 0.95) {
		float night_to_sunrise_progress = (current_day_progress - 0.925) / 0.025;
		FragColor = mix(night, transition, night_to_sunrise_progress);
	}
	// rest is sunrise
	else {
		float sunrise_progress = (current_day_progress - 0.95) / 0.05;
		FragColor = mix(day, transition, 1.0 - sunrise_progress);
	}
 }
