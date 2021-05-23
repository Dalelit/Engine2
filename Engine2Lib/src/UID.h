#pragma once

// Really basic id generator.
// To do: work out something that is at least thread safe.

namespace Engine2
{
	class UID
	{
	public:
		UID() = default;
		UID(uint64_t value) : id(value) {}

		bool operator==(const UID other) const { return id == other.id; }

		bool Empty() const { return id == 0; }

	protected:
		uint64_t id = 0;
	};

	class UIDGenerator
	{
	public:
		UIDGenerator();

		// Note: not thread safe
		UID Next();

	protected:
		uint32_t start;
		uint32_t count = 0;
	};
}