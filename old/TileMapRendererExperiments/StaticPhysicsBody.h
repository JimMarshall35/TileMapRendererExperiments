#pragma once
class b2Body;
class b2Fixture;
struct StaticPhysicsBody {
	b2Body* body;
	b2Fixture* fixture;
};