#pragma once
class b2Body;
struct StaticPhysicsBody {
	b2Body* body;
	b2Fixture* fixture;
};