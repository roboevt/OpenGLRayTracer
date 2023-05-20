#version 330 core

#define FLT_MAX 3.402823466e+38
#define MAX_BOUNCES 5

out vec4 color;

uniform sampler2D previousFrame;
in vec4 gl_FragCoord;     // current pixel location
uniform vec2 uResolution; // screen dimensions in pixels
uniform uint frames;      // frames since last camera movement
uniform mat4 mvp;
uniform int samples;

struct Sphere {
	vec3 location;
	float radius;

	//Material:
	vec3 diffuseColor;
	vec3 emissionColor;
	float emissionStrength;
	float reflectivity;
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
	vec3 diffuseColor;
	vec3 emissionColor;
	float emissionStrength;
	float reflectivity;
};

uint NextRandom(inout uint state) {
	state = state * 747796405u + 2891336453u;
	uint result = ((state >> ((state >> 28) + 4u)) ^ state) * 277803737u;
	result = (result >> 22) ^ result;
	return result;
}

float rand(inout uint state) {  //alThor
	return NextRandom(state) / 4294967295.0; // 2^32 - 1
}

// Random value in normal distribution (with mean=0 and sd=1)
float RandomValueNormalDistribution(inout uint state) {
	// Thanks to https://stackoverflow.com/a/6178290
	float theta = 2 * 3.1415926 * rand(state);
	float rho = sqrt(-2 * log(rand(state)));
	return rho * cos(theta);
}

vec3 randDirection(inout uint state) {
	// Thanks to https://math.stackexchange.com/a/1585996
	float x = RandomValueNormalDistribution(state);
	float y = RandomValueNormalDistribution(state);
	float z = RandomValueNormalDistribution(state);
	return normalize(vec3(x, y, z));
}

vec3 SkyColorHorizon = vec3(1,1,1);
vec3 SkyColorZenith = vec3(0.0788092, 0.36480793, 0.7264151);
vec3 GroundColor = vec3(.35,.3,.35);
float SunFocus = .5;
float SunIntensity = 0;
vec3 SunLoc = vec3(100,100,100);

vec3 GetEnvironmentLight(Ray ray){
	vec3 dir = (vec4(ray.direction,1) * mvp).xyz;
	float skyGradientT = pow(smoothstep(0, 0.4, dir.y), 0.35);
	float groundToSkyT = smoothstep(-0.01, 0, dir.y);
	vec3 skyGradient = mix(SkyColorHorizon, SkyColorZenith, skyGradientT);
	float sun = pow(max(0, dot(dir, SunLoc)), SunFocus) * SunIntensity;
	// Combine ground, sky, and sun
	vec3 composite = mix(GroundColor, skyGradient, groundToSkyT) + sun * float(groundToSkyT>=1);
	return composite;
}

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
			col.diffuseColor = sphere.diffuseColor;
			col.emissionColor = sphere.emissionColor;
			col.emissionStrength = sphere.emissionStrength;
			col.reflectivity = sphere.reflectivity;
		} else {
			col.hit = false;
		}
	} else {
		col.hit = false;
	}
	return col;
}

RayCollision RayWorldIntersection(Ray ray) {
	RayCollision nearest;
	nearest.hit = false;
	nearest.distance = FLT_MAX;

	for(int i = 0; i < numSpheres; i++) {
		Sphere sphere = spheres[i];
		RayCollision col = RaySphereIntersection(ray, sphere);

		if(col.hit && col.distance < nearest.distance) {
			nearest = col;
		}
	}
	return nearest;
};

vec3 trace(Ray ray, inout uint state) {
	vec3 incomingLight = vec3(0);
	vec3 rayColor = vec3(1);

	for(int i = 0; i < MAX_BOUNCES; i++) {
		RayCollision col = RayWorldIntersection(ray);
		if(col.hit) {
			ray.origin = col.point;
			vec3 diffuseDirection = normalize(col.normal + randDirection(state));
			vec3 specularDirection = reflect(ray.direction, col.normal);
			ray.direction = mix(diffuseDirection, specularDirection, col.reflectivity);

			vec3 emittedLight = col.emissionColor * col.emissionStrength;
			incomingLight += emittedLight * rayColor;
			rayColor *= col.diffuseColor;
		} else {
			incomingLight += GetEnvironmentLight(ray) * rayColor;  // background
			break;
		}
	}
	return incomingLight;
}

void main() {
	vec2 screenLoc = getScreenLoc(gl_FragCoord.xy);

	Ray cameraRay = getCameraRay(screenLoc);
	vec3 newColor = vec3(0);
	for(int i = 0; i < samples; i++) {
		uint state = ((frames +uint(i) + 1u) * 3459054u) * uint(gl_FragCoord.x + gl_FragCoord.y * uResolution.x);
		newColor += trace(cameraRay, state);
	}
	newColor /= samples;

	vec3 previousColor = texture(previousFrame, gl_FragCoord.xy / uResolution).xyz;
	color = vec4(previousColor + newColor, 1);
};