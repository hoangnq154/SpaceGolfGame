#include "utilities.h"
#include "Components/PositionComponent.h"
#include "Components/CircleRadiusComponent.h"
#include "Components/VelocityComponent.h"
#include <math.h>

namespace Utilities {

	// Returns true/false wether or not the first entity is colliding with second one
	// This method is radius-safe
	bool isColliding(entityx::Entity & first, entityx::Entity & second)
	{
		Vector2f diff = first.component<PositionComponent>()->pos - second.component<PositionComponent>()->pos;
		float r = getRadius(first) + getRadius(second);
		return diff.lengthSq() < r * r;
	}

	// Null-safe method of gething radius of an object
	float getRadius(entityx::Entity & entity)
	{
		if (entity.component<CircleRadiusComponent>()) {
			return entity.component<CircleRadiusComponent>()->r;
		}
		else {
			return 0;
		}
	}

	// Null-safe method of geting velocity of an object
	Vector2f& getVelocity(entityx::Entity & entity)
	{
		if (entity.component<VelocityComponent>()) {
			return entity.component<VelocityComponent>()->v;
		}
		else {
			return Vector2f();
		}
	}

	// Returns where moving object should be at the moment of impact at nonmoving object
	// This is done by solving the following equations
	//
	// Circle equation for nonMoving entity:
	// (x - px)^2 + (y - py)^2 = r^2
	// Parametric line equation for moving entity:
	// x = bx + vx*t
	// y = by + vy*t
	//
	// If we solve for t, we will get exact position of moving object at the moment of impact
	//
	// Please note:
	// px, py - non moving object position
	// bx, by - moving object position
	// vx, vy - moving object velocity
	// r - combined radiuses of moving and non-moving object (r1 + r2)
	Vector2f getPointOfImpact(entityx::Entity &nonMoving, entityx::Entity &moving) {
		Vector2f& p = nonMoving.component<PositionComponent>()->pos;
		Vector2f& v = moving.component<VelocityComponent>()->v;
		Vector2f& b = moving.component<PositionComponent>()->pos;
		Vector2f d = b - p;
		float r = getRadius(nonMoving) + getRadius(moving);
		float qa = v.lengthSq();
		float qb = 2 * d.dot(v);
		float qc = d.lengthSq() - r * r;
		float t1, t2;
		solveQuadratic(qa, qb, qc, t1, t2);

		// We get 2 candidates after solving quadratic equation. One is entry
		// point, the other one is exit point. We are interested in entry point.
		Vector2f cand1 = b + v * t1;
		Vector2f cand2 = b + v * t2;

		// The entry point is the closest point to moving object's position
		// before the impact.
		Vector2f before = b - v;
		return ((before - cand1).lengthSq() < (before - cand2).lengthSq()) ? cand1 : cand2;
	}

	// Returns random number between from and to
	float random(float from, float to)
	{
		return (float)rand() / (float)RAND_MAX * (to - from) + from;
	}

	// Returns a vector with random components
	Vector2f randomVector(float from, float to)
	{
		return Vector2f(random(from, to), random(from, to));
	}

	// Returns abgr value from integer values (0 - 255)
	uint32_t abgr(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		return (uint32_t)a << 24 |
			(uint32_t)b << 16 |
			(uint32_t)g << 8 |
			(uint32_t)r;
	}

	// Returns abgr value from float values (0.0 - 1.0)
	uint32_t abgr(float r, float g, float b, float a)
	{
		return abgr((unsigned char)cap(r * 256.0f, 0.0f, 255.999f),
			(unsigned char)cap(g * 256.0f, 0.0f, 255.999f),
			(unsigned char)cap(b * 256.0f, 0.0f, 255.999f),
			(unsigned char)cap(a * 256.0f, 0.0f, 255.999f));
	}

	// Returns 2 solutions of quadratic equation based on a, b, c parameters
	// http://tutorial.math.lamar.edu/Classes/Alg/SolveQuadraticEqnSummary_files/eq0002P.gif
	void solveQuadratic(float a, float b, float c, float &s1, float &s2) {
		float underRoot = b * b - 4.0f * a * c;
		assert(underRoot >= 0.0f);
		float root = sqrt(underRoot);
		float a2 = 2.0f * a;
		s1 = (-b + root) / a2;
		s2 = (-b - root) / a2;
	}

	// Cap a number between from and to values
	template <typename T>
	T cap(T value, T from, T to) {
		if (value > to) {
			return to;
		}
		if (value < from) {
			return from;
		}
		return value;
	}
	// template int cap<int>(int value, int from, int to);
}