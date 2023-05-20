#version 330 core
out vec4 color;

uniform sampler2D screenTexture;
uniform vec2 uResolution; // screen dimensions in pixels
uniform uint frames;  // frames since last movement

void main() { 
    color = texture(screenTexture, gl_FragCoord.xy / uResolution) / int(frames);
}