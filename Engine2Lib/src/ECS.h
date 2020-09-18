#pragma once
#include <iostream>
#include <bitset>
#include <cassert>

// Usage:
/*
	Coordinator coord;                        // Declare the cooridnator.
	EntityId_t e1 = coord.CreateEntity();     // Create Entities
	coord.AddComponent<Transform>(e1);        // Add a component to an entity
	coord.GetComponent<Transform>(e1);        // Get a component for an entity
	coord.HasComponent<Transform>(e1);        // Test is an entity has a component
	View<Transform,RigidBody> v(coord);       // Get a view of all entities with a set signature of components
											  //   More efficient as it will only check entities in the shortest component list
	EntityView<Transform,RigidBody> v(coord); // Get a view of all entities with a set signature of components (I'd expect the view to be better)
	EntityView v(coord);                      // Get a view of all entities
	for (auto t : v) { t }                    // Use the view
	coord.GetComponents<Transform>();         // Get all of a single component, can us in a for (auto c : xxx) loop
*/
//
// To Do:
// - remove entities
// - remove components
// - emplace creation


namespace EngineECS
{
	using EntityId_t = uint32_t;
	constexpr EntityId_t MAXENTITIES = 1000000;

	using ComponentId_t = uint32_t;
	constexpr ComponentId_t MAXCOMPONENTS = 32;

	using ComponentIndex_t = EntityId_t; // index for components is the same as the entity as that's the max there will be

	using Signature = std::bitset<MAXCOMPONENTS>;

	class Storage
	{
	public:
		virtual ~Storage() = default;

		inline ComponentIndex_t Count()     const { return count; }
		inline ComponentIndex_t Capacity()  const { return capacity; }
		inline const std::string& GetName() const { return name; }

		inline EntityId_t GetEntity(ComponentIndex_t index) const { return indexToEntityMap[index]; }
		inline EntityId_t* EntitiesBegin() const { return indexToEntityMap; }
		inline EntityId_t* EntitiesEnd() const { return indexToEntityMap + count; }

		friend std::ostream& operator<<(std::ostream& out, const Storage& store) { store.print(out); return out; }
		friend std::ostream& operator<<(std::ostream& out, const Storage* store) { store->print(out); return out; }
		virtual void print(std::ostream& out) const = 0;

	protected:
		ComponentId_t id;
		ComponentIndex_t capacity = 0;
		std::string name;
		ComponentId_t count = 0;
		ComponentId_t* entityToIndexMap = nullptr;
		EntityId_t* indexToEntityMap = nullptr;
	};

	template <typename T>
	class ComponentStorage : public Storage
	{
	public:
		ComponentStorage(ComponentId_t id, ComponentIndex_t capacity)
		{
			this->id = id;
			this->name = typeid(T).name();
			this->capacity = capacity;

			data = new T[capacity];
			entityToIndexMap = new ComponentId_t[capacity];
			indexToEntityMap = new EntityId_t[capacity];
			Clear();
		}

		~ComponentStorage()
		{
			for (ComponentIndex_t i = 0; i < count; i++) data[i].~T();
			delete[] data;
			delete[] entityToIndexMap;
			delete[] indexToEntityMap;
		}

		T* Create(EntityId_t id)
		{
			assert(count < capacity && "Exceeded storage capacity");

			T* result = next;
			uint32_t index = count;

			entityToIndexMap[id] = index;
			indexToEntityMap[index] = id;

			count++;
			next++;

			return new(result) T(); // default construct in the required memory location
		}

		inline T* GetComponent(EntityId_t id)
		{
			return data + entityToIndexMap[id];
		}

		inline T* begin() { return data; }
		inline T* end() { return data + count; }
		inline const T* begin() const { return data; }
		inline const T* end() const { return data + count; }

		inline T& operator[](ComponentIndex_t indx) { return data[indx]; }
		inline const T& operator[](ComponentIndex_t indx) const { return data[indx]; }

		void Clear()
		{
			memset(data, 0, sizeof(T) * capacity);
			count = 0;
			next = data;
			memset(entityToIndexMap, 0, sizeof(ComponentIndex_t) * capacity);
			memset(indexToEntityMap, 0, sizeof(EntityId_t) * capacity);
		}

		void print(std::ostream& out) const override
		{
			out << id << " " << name << ", " << count << "/" << capacity;
		}

	private:
		T* data = nullptr;
		T* next = nullptr;
	};

	class Coordinator
	{
	public:
		Coordinator()
		{
			entitySignatures = new Signature[MAXENTITIES];

			memset(componentStores, 0, sizeof(componentStores));
			memset(componentNames, 0, sizeof(componentNames));
		}

		~Coordinator()
		{
			delete[] entitySignatures;

			for (ComponentId_t i = 0; i < MAXCOMPONENTS; i++)
				if (componentStores[i]) delete componentStores[i];
		}

		EntityId_t CreateEntity()
		{
			EntityId_t id = entityCounter++;

			entitySignatures[id].reset();

			return id;
		}

		EntityId_t GetEntityCount() const { return entityCounter; }

		bool TestEntity(Signature signature, EntityId_t id) { return (entitySignatures[id] & signature) == signature; }

		template <typename T>
		T* AddComponent(EntityId_t id)
		{
			ComponentStorage<T>* store = GetComponentStorage<T>();

			entitySignatures[id].set(GetComponentId<T>());

			return store->Create(id);
		}

		template <typename T>
		inline bool HasComponent(EntityId_t id) { return HasComponent(id, GetComponentId<T>()); }
		inline bool HasComponent(EntityId_t id, ComponentId_t cid) { return entitySignatures[id].test(cid); }

		template <typename T>
		T* GetComponent(EntityId_t id)
		{
			if (HasComponent<T>(id)) return GetComponentStorage<T>()->GetComponent(id);
			else return nullptr;
		}

		template <typename T>
		ComponentId_t GetComponentId()
		{
			static ComponentId_t cid = componentIdCounter++;
			return cid;
		}

		template <typename T>
		ComponentStorage<T>& GetComponents()
		{
			return *GetComponentStorage<T>();
		}

		Storage* GetComponentStore(ComponentId_t id)
		{
			return componentStores[id];
		}

		const char* GetComponentName(ComponentId_t id) const { return componentNames[id]; }

		void PrintEntity(std::ostream& out, EntityId_t id)
		{
			Signature& s = entitySignatures[id];
			out << id << " " << s << " ";
			ComponentId_t i = 0, count = 0;
			while (count < s.count())
			{
				if (HasComponent(id, i))
				{
					out << GetComponentName(i) << ", ";
					count++;
				}
				i++;
			}
		}
		friend std::ostream& operator<<(std::ostream& out, Coordinator& coord) {
			out << "Coordinator" << std::endl;
			out << " entities: " << coord.entityCounter << std::endl;
			for (EntityId_t id = 0; id < coord.entityCounter; id++)
			{
				coord.PrintEntity(out, id);
				out << std::endl;
			}

			out << " components: " << coord.componentIdCounter << std::endl;
			for (uint32_t i = 0; i < coord.componentIdCounter; i++) out << coord.componentStores[i] << std::endl;

			return out;
		}

	private:
		EntityId_t entityCounter = 0;
		Signature* entitySignatures = nullptr;

		ComponentId_t componentIdCounter = 0;
		Storage* componentStores[MAXCOMPONENTS];
		const char* componentNames[MAXCOMPONENTS];

		template<typename T>
		ComponentStorage<T>* GetComponentStorage()
		{
			ComponentId_t id = GetComponentId<T>();

			Storage* pStore = componentStores[id];

			if (pStore == nullptr)
			{
				pStore = new ComponentStorage<T>(id, MAXENTITIES);
				componentStores[id] = pStore;
				componentNames[id] = typeid(T).name();
			}

			return (ComponentStorage<T>*)pStore;
		}
	};

	class ViewBase
	{
	public:
		ViewBase(Coordinator& coord) : pCoord(&coord) {}
		virtual ~ViewBase() = default;
		virtual void Print(std::ostream& out) = 0;

		friend std::ostream& operator<<(std::ostream& out, ViewBase& v)
		{
			v.Print(out);
			return out;
		}

	protected:
		Coordinator* pCoord = nullptr;
		Signature signature;
		bool all = false;

		template <typename... T>
		void SetSignature()
		{
			if (sizeof...(T) > 0)
			{
				// note: 0 in here as cannot expand template to have empty array
				ComponentId_t componentIds[] = { pCoord->GetComponentId<T>()..., 0 };
				for (int i = 0; i < sizeof...(T); i++)
				{
					signature.set(componentIds[i]);
				}
			}
		}
	};

	template <typename... T>
	class View : public ViewBase
	{
	public:
		View(Coordinator& coord) : ViewBase(coord)
		{
			SetSignature<T...>();

			ComponentId_t componentIds[] = { pCoord->GetComponentId<T>()... };
			ComponentIndex_t componentCount = MAXENTITIES;

			// find the shortest component list to base the view on
			for (int i = 0; i < sizeof...(T); i++)
			{
				Storage* store = coord.GetComponentStore(componentIds[i]);
				if (componentCount > store->Count())
				{
					componentStore = store;
					componentCount = store->Count();
				}
			}

			assert(componentStore != nullptr && "view should have found a shortest component store");
		}

		struct Iterator
		{
			Coordinator* pCoord;
			EntityId_t* currentEntity = nullptr;
			EntityId_t* endEntity = nullptr;
			Signature signature;

			Iterator(Coordinator* pCoord, Signature signature, EntityId_t* entitiesBegin, EntityId_t* entitiesEnd) :
				pCoord(pCoord), signature(signature), currentEntity(entitiesBegin), endEntity(entitiesEnd)
			{
			}

			EntityId_t operator*() const { return *currentEntity; }
			bool operator==(const Iterator& other) const { return currentEntity == other.currentEntity; }
			bool operator!=(const Iterator& other) const { return currentEntity != other.currentEntity; }

			Iterator& operator++()
			{
				currentEntity++;
				CheckForNext();
				return *this;
			}

			void CheckForNext()
			{
				while (currentEntity != endEntity && !pCoord->TestEntity(signature, *currentEntity))
				{
					currentEntity++;
				}
			}
		};

		const Iterator begin() const
		{
			Iterator iter(pCoord, signature, componentStore->EntitiesBegin(), componentStore->EntitiesEnd());
			iter.CheckForNext();

			return iter;
		}

		const Iterator end() const
		{
			return Iterator(pCoord, signature, componentStore->EntitiesEnd(), componentStore->EntitiesEnd());
		}

		void Print(std::ostream& out)
		{
			out << "View " << signature << ", Entities to test " << componentStore->Count();
		}

	protected:
		Storage* componentStore = nullptr;
	};

	template <typename... T>
	class EntityView : public ViewBase
	{
	public:
		EntityView(Coordinator& coord) : ViewBase(coord)
		{
			SetSignature<T...>();
		}

		struct Iterator
		{
			Coordinator* pCoord = nullptr;
			Signature signature;
			EntityId_t index;

			Iterator(Coordinator* pCoord, Signature signature, ComponentIndex_t index) :
				pCoord(pCoord), signature(signature), index(index)
			{
			}

			EntityId_t operator*() const { return index; }
			bool operator==(const Iterator& other) const { return index == other.index; }
			bool operator!=(const Iterator& other) const { return index != other.index; }

			Iterator& operator++()
			{
				index++;
				while (index < pCoord->GetEntityCount() && !pCoord->TestEntity(signature, index)) index++;

				return *this;
			}

		};
		const Iterator begin() const
		{
			ComponentIndex_t index = 0;

			// get the first one that matches the signature
			while (index < pCoord->GetEntityCount() && !pCoord->TestEntity(signature, index)) index++;

			return Iterator(pCoord, signature, index);
		}

		const Iterator end() const
		{
			return Iterator(pCoord, signature, pCoord->GetEntityCount());
		}

		void Print(std::ostream& out)
		{
			out << "EntityView " << signature << ", Entities to test " << pCoord->GetEntityCount();
		}
	};
}
