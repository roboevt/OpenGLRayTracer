#pragma once

const char* vertexBackup = R"(
#version 330 core

layout(location = 0) in vec4 position;

void main() {
	gl_Position = position;
};

)";

const char* fragmentBackup = R"(
#version 330 core

#define FLT_MAX 3.402823466e+38

layout(location = 0) out vec4 color;

in vec4 gl_FragCoord;     // current pixel location

uniform vec2 uResolution; // screen dimensions in pixels


struct Sphere {
	vec3 location;
	float radius;

	//Material:
	vec3 color;
};

uniform Sphere spheres[10];
uniform int numSpheres;

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct RayCollision {
	bool hit;
	float distance;
	vec3 point;
	vec3 normal;

	//Material:
	vec3 color;
};

// transform pixel location to screenspace location
// coordinates range from approximatly -.5 to .5, but aspect ratio means x range differs
vec2 getScreenLoc(vec2 pixel) {
	float aspectRatio = uResolution.x / uResolution.y;
	float screenX = pixel.x / uResolution.x;
	float dx = screenX - 0.5f;
	screenX = dx * aspectRatio;

	float screenY = (pixel.y / uResolution.y) - 0.5f;
	return vec2(screenX, screenY);
}

Ray getCameraRay(vec2 screenLoc) {
	vec3 lookPoint = vec3(screenLoc, 1);
	return Ray(vec3(0), normalize(lookPoint));
}

//https://www.youtube.com/watch?v=Qz0KTGYJtUk
RayCollision RaySphereIntersection(Ray ray, Sphere sphere) {
	RayCollision col;
	vec3 offsetRayOrigin = ray.origin - sphere.location;

	float a = dot(ray.direction, ray.direction);
	float b = 2 * dot(offsetRayOrigin, ray.direction);
	float c = dot(offsetRayOrigin, offsetRayOrigin) - sphere.radius * sphere.radius;

	float discriminant = b * b - 4 * a * c;

	if(discriminant >= 0) {
		float distance = (-b - sqrt(discriminant)) / (2 * a);
		if(distance >= 0) {
			col.hit = true;
			col.distance = distance;
			col.point = ray.origin + (normalize(ray.direction) * distance);
			col.normal = normalize(col.point - sphere.location);
			col.color = sphere.color;
		}
	} else {
		col.hit = false;
	}
	return col;
}

RayCollision trace(Ray ray) {
	RayCollision nearest;
	nearest.distance = FLT_MAX;

	for(int i = 0; i < numSpheres; i++) {  // TODO pass current number of spheres
		Sphere sphere = spheres[i];
		RayCollision col = RaySphereIntersection(ray, sphere);

		if(col.hit && col.distance < nearest.distance) {
			nearest = col;
		}
	}
	return nearest;
};

void main() {
	vec2 screenLoc = getScreenLoc(gl_FragCoord.xy);
	
	RayCollision col = trace(getCameraRay(screenLoc));
	if(col.hit)
		color = vec4(col.color, 1);
	else
		color = vec4(0);
};

)";