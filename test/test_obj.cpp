#include "catch_amalgamated.hpp"
#include <vector>
#include <random>
#include "Random.h"
#include "cppobj.h"

TEST_CASE("Load Obj" "[Obj]")
{
	using namespace lray;
	cppobj::Parser parser;
	parser.parse("./CornellBox/CornellBox-Original.obj");
	parser.parse("./living_room/living_room.obj");
	for(u32 i=0; i<parser.getNumFaces(); ++i){
		const cppobj::Face& face = parser.getFace(i);
		printf("[%d] %d %d %d\n",face.p0_, face.p1_, face.p2_);
		const cppobj::Vertex& v0 = parser.getVertex(face.p0_);
		const cppobj::Vertex& v1 = parser.getVertex(face.p1_);
		const cppobj::Vertex& v2 = parser.getVertex(face.p2_);
		printf("    p(%f %f %f) t(%f %f) n(%f %f %f)\n",
			v0.position_.x_, v0.position_.y_, v0.position_.z_,
			v0.texcoord_.x_, v0.texcoord_.y_,
			v0.normal_.x_, v0.normal_.y_, v0.normal_.z_);
		printf("    p(%f %f %f) t(%f %f) n(%f %f %f)\n",
			v1.position_.x_, v1.position_.y_, v1.position_.z_,
			v1.texcoord_.x_, v1.texcoord_.y_,
			v1.normal_.x_, v1.normal_.y_, v1.normal_.z_);
		printf("    p(%f %f %f) t(%f %f) n(%f %f %f)\n",
			v2.position_.x_, v2.position_.y_, v2.position_.z_,
			v2.texcoord_.x_, v2.texcoord_.y_,
			v2.normal_.x_, v2.normal_.y_, v2.normal_.z_);
	}
}
