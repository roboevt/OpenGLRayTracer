#version 330 core

layout(location = 0) out vec4 color;

in vec4 gl_FragCoord;     // current pixel location

uniform vec2 uResolution; // screen dimensions in pixels

// transform pixel location to screenspace location
vec2 getScreenLoc(vec2 pixel) {
	float aspectRatio = uResolution.x / uResolution.y;
	float screenX = pixel.x / uResolution.x;
	float dx = screenX - 0.5f;
	screenX = 0.5f + (dx * aspectRatio);

	float screenY = pixel.y / uResolution.y;
	return vec2(screenX, screenY);
}

bool checkRect(vec2 check, vec2 min, vec2 max) {
	return (check.x > min.x && check.x < max.x && check.y > min.y && check.y < max.y);
}

void main() {
	vec2 screenLoc = getScreenLoc(gl_FragCoord.xy);
	
	vec2 loc = vec2(0.5f, 0.5f);
	
	vec2 dir = loc - screenLoc;
	float distance = sqrt(dot(dir,dir));
	 
	if(distance < 0.25) {
		color = vec4(screenLoc.xy,0,1);
	} else { 
		color = vec4(0); 
	}
};