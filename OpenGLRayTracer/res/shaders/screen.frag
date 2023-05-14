#version 330 core
out vec4 color;

uniform sampler2D screenTexture;
//uniform vec2 uResolution; // screen dimensions in pixels

void main() { 
    //vec2 screenLoc = gl_FragCoord.xy / uResolution;
    color = vec4(1,0,0,1);
    //color = texture(screenTexture, screenLoc);
    color = texture(screenTexture, gl_FragCoord.xy / vec2(1920,1080));
}