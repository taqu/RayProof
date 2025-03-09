#include "World.h"
#include "Object.h"

namespace lray
{
World::World()
{
}

World::~World()
{
	clear();
}

void World::clear()
{
	for(u32 i=0; i<objects_.size(); ++i){
		delete objects_[i];
	}
	for(u32 i=0; i<bottomAccelerations_.size(); ++i){
		delete bottomAccelerations_[i];
	}
	objects_.clear();
	bounds_.clear();
	bottomAccelerations_.clear();
}

void World::add(Object* object)
{
	objects_.push_back(object);
}

void World::build()
{
	for(u32 i=0; i<bottomAccelerations_.size(); ++i){
		delete bottomAccelerations_[i];
	}

	bounds_.resize(objects_.size());
	for(u32 i=0; i<objects_.size(); ++i){
		bounds_[i] = objects_[i]->getAABB();
	}
    if(0 < objects_.size()) {
        topAcceleration_.build(objects_.size(), &bounds_[0]);
    }
	bottomAccelerations_.resize(objects_.size());
	for(u32 i=0; i<objects_.size(); ++i){
		bottomAccelerations_[i] = new BinQBVH;
		bottomAccelerations_[i]->build(objects_[i]);
	}
}

HitRecord World::intersect(const Ray& ray, f32 tmin, f32 tmax)
{
	auto func = [this](const Ray& ray, u32 id, f32 tmin, f32 tmax){
		return bottomAccelerations_[id]->intersect(ray, tmin, tmax);
	};
	return topAcceleration_.intersect(ray, tmin, tmax, func);
}
} // namespace lray

