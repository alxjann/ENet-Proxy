#pragma once
#include "../utils/Math.hpp"
#include "../utils/Reader.hpp"
#include <cstdint>
#include <cstddef>
#include <deque>
#include <iostream>

#pragma pack (push, 1)
struct WorldObject {
	std::uint16_t id;
	CL_Vec2F pos;
	std::uint8_t count;
	std::uint8_t flags;
	std::uint32_t offset;

	WorldObject() : id(0), pos({}), count(0), flags(0), offset(0) {}
	~WorldObject() = default;

	bool operator==(const WorldObject& other) const {
		return id == other.id && pos == other.pos && count == other.count
			&& flags == other.flags && offset == other.offset;
	}
	
	auto Serialize(void* buffer, std::size_t& position) -> void {
		MemoryReader reader(buffer);
		reader.Skip(position);

		id = reader.Read<std::uint16_t>();
		pos = reader.Read<CL_Vec2F>();
		count = reader.Read<std::uint8_t>();
		flags = reader.Read<std::uint8_t>();
		offset = reader.Read<std::uint32_t>();

		position = reader.GetPosition();
	}
};

class WorldObjectMap {
public:
	std::uint32_t objectCount;
	std::uint32_t lastOffset;
	std::vector<WorldObject> objects;
	//std::unordered_map<uint32_t, WorldObject> objects2;

	WorldObjectMap() : objectCount(0), lastOffset(0), objects() {}
	~WorldObjectMap() {
		objects.clear();
	}

	auto Serialize(void* buffer, std::size_t& position) -> void {
		MemoryReader reader(buffer);
		reader.Skip(position);
		reader.Skip(12);

		objectCount = reader.Read<std::uint32_t>();
		lastOffset = reader.Read<std::uint32_t>();

		objects.reserve(objectCount);

		position = reader.GetPosition();

		for (std::uint32_t i = 0; i < objectCount; i++) {
			WorldObject object{};
			object.Serialize(buffer, position);
			objects.push_back(object);
		}
	}
};
#pragma pack(pop)