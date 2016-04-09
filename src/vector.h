#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>
#include "main.h"

class quat4;
class vec2;
class vec3;
class ivec3;
class ivec2;

class quat4 {
	public:
		// Properties
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};

			struct {
				float r;
				float g;
				float b;
				float a;
			};

			struct {
				float v[4];
			};
	    };

		// Constructs
		quat4(void);
		quat4(float ix, float iy, float iz, float iw);
		quat4(float angle, vec3 axis);

		// Operators
		float operator[](int i){return v[i];}
		quat4 operator*(quat4 in);
};

class vec2 {
	public:
		// Properties
		union {
			struct {
				float x;
				float y;
			};

			struct {
				float s;
				float t;
			};

			struct {
				float start;
				float end;
			};

			struct {
				float v[2];
			};
	    };

		// Constructs
		vec2(void);
		vec2(float i);
		vec2(float x, float y);

		// Operators
		float operator[](int i){return v[i];}
		vec2 operator+(vec2 in);
		void operator+=(vec2 in);
		vec2 operator-(vec2 in);
		void operator-=(vec2 in);
		vec2 operator-(void);
		vec2 operator*(vec2 in);
		void operator*=(vec2 in);
		vec2 operator/(vec2 in);
		void operator/=(vec2 in);
		bool operator==(vec2 in);
		bool operator!=(vec2 in);
};	

class vec3 {
	public:
		// Properties
		union {
			struct {
				float x;
				float y;
				float z;
			};

			struct {
				float r;
				float g;
				float b;
			};

			struct {
				float v[3];
			};
	    };

		// Constructs
		vec3(void);
		vec3(float i);
		vec3(float x, float y, float z);
		vec3(ivec3 in);

		// Operators
		float operator[](int i){return v[i];}

		vec3 operator+(vec3 in);
		void operator+=(vec3 in);
		
		vec3 operator-(vec3 in);
		vec3 operator-(float in);

		void operator-=(vec3 in);
		vec3 operator-(void);
		
		vec3 operator*(vec3 in);
		vec3 operator*(float in);
		vec3 operator*(quat4 in);
		void operator*=(vec3 in);
		
		vec3 operator/(vec3 in);
		void operator/=(vec3 in);
		void operator/=(float in);
		
		bool operator==(vec3 in);
		bool operator!=(vec3 in);

		// Methods
		vec3 cross(vec3 in);
		static vec3 vec3::cross(vec3 v1, vec3 v2);
		static vec3 vec3::cross(quat4 v1, vec3 v2);

		float dot(vec3 in);
		static float dot(vec3 v1, vec3 v2);
		float length();
		static float length(vec3 in);
		static vec3 vec3::max(vec3 in, float m);
		static vec3 vec3::min(vec3 in, float m);
		void normalise();
		static vec3 vec3::pow(vec3 in, float p);
		static vec3 vec3::reflect(vec3 in, vec3 normal);
};

class ivec3 {
	public:
		// Properties
		union {
			struct {
				int x;
				int y;
				int z;
			};

			struct {
				int r;
				int g;
				int b;
			};

			struct {
				int v[3];
			};
	    };

		// Constructs
		ivec3(void);
		ivec3(int i);
		ivec3(int x, int y, int z);

		// Operators
		int operator[](int i){return v[i];}

		ivec3 operator+(ivec3 in);
		void operator+=(ivec3 in);

		ivec3 operator-(ivec3 in);
		void operator-=(ivec3 in);
		ivec3 operator-(void);
		
		ivec3 operator*(ivec3 in);
		ivec3 operator*(float in);
		void operator*=(ivec3 in);
		
		ivec3 operator/(ivec3 in);
		void operator/=(ivec3 in);

		bool operator==(ivec3 in);
		bool operator!=(ivec3 in);
};

class ivec2 {
	public:
		// Properties
		union {
			struct {
				int x;
				int y;
			};

			struct {
				int start;
				int end;
			};

			struct {
				int v[2];
			};
	    };

		// Constructs
		ivec2(void);
		ivec2(int i);
		ivec2(int x, int y);

		// Operators
		int operator[](int i){return v[i];}
		ivec2 operator+(ivec2 in);
		void operator+=(ivec2 in);
		ivec2 operator-(ivec2 in);
		void operator-=(ivec2 in);
		ivec2 operator-(void);
		ivec2 operator*(ivec2 in);
		void operator*=(ivec2 in);
		ivec2 operator/(ivec2 in);
		void operator/=(ivec2 in);
		bool operator==(ivec2 in);
		bool operator!=(ivec2 in);
};	

struct hit_t {
	vec3 point;
	vec2 uv;
};

#endif
