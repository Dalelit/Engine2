#pragma once
#include <iostream>
#include <bitset>
#include <cassert>
#include <set>

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
	coord.GetComponents<Transform>();         // Get all of a single component, can us in a for (auto c : xxx) loop, or use the iterator which can also reference the entity id.
*/
//
// To Do:
// - safety as there is no checking
// - emplace creation
// - performance testing and tuning
// - testing and test cases


namespace EngineECS
{
	using EntityId_t = uint32_t;

	using ComponentId_t = uint32_t;
	constexpr ComponentId_t MAXCOMPONENTS = 32;

	using ComponentIndex_t = EntityId_t; // index for components is the same as the entity as that's the max there will be

	using Signature = std::bitset<MAXCOMPONENTS>;


	/*
	* 
	* Storage interface
	* 
	*/
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

		virtual void DestroyComponent(EntityId_t id) = 0;

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

	/*
	*
	* Templated Storage class using the storage interface.
	* Where the components are stored.
	* Assumes an entity can only have a component once.
	* 
	* To do - version for mulitples of the same component for an entity
	*
	*/
	template <typename T>
	class ComponentStorage : public Storage
	{
	public:
		ComponentStorage(ComponentId_t id, ComponentIndex_t capacity)
		{
			this->id = id;
			this->name = typeid(T).name();
			this->capacity = capacity;

			data = malloc(sizeof(T) * capacity);
			entityToIndexMap = new ComponentId_t[capacity];
			indexToEntityMap = new EntityId_t[capacity];
			Clear();
		}

		~ComponentStorage()
		{
			T* ptr = (T*)data;
			for (ComponentIndex_t i = 0; i < count; i++)
			{
				ptr->~T();
				ptr++;
			}
			free(data);
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
			return (T*)data + entityToIndexMap[id];
		}

		void DestroyComponent(EntityId_t id)
		{
			uint32_t index = entityToIndexMap[id];
			T* pDeleted = ((T*)data) + index;
			pDeleted->~T(); // destructor called

			count--; // 1 less component stored;

			// if there are still components, and the one deleted wasn't the last one, swap the end to fill the gap
			if (count > 0 && index != count)
			{
				T* pLast = ((T*)data) + count;               // get the last entry
				memcpy(pDeleted, pLast, sizeof(T));          // copy the last entry into the gap
				EntityId_t idLast = indexToEntityMap[count]; // get the entity id of last entry
				indexToEntityMap[index] = idLast;            // move id now in new location
				entityToIndexMap[idLast] = index;            // last id linked to new index
			}

			next--; // next entry in the store is previous as the last entry has moved
		}

		inline T& operator[](ComponentIndex_t indx) { return *((T*)data + indx); }
		inline const T& operator[](ComponentIndex_t indx) const { return *((T*)data + indx); }

		void Clear()
		{
			memset(data, 0, sizeof(T) * capacity);
			count = 0;
			next = (T*)data;
			memset(entityToIndexMap, 0, sizeof(ComponentIndex_t) * capacity);
			memset(indexToEntityMap, 0, sizeof(EntityId_t) * capacity);
		}

		void print(std::ostream& out) const override
		{
			out << id << " " << name << ", " << count << "/" << capacity;
		}

		class Iterator
		{
		public:
			Iterator(T* component, EntityId_t* entities) : currentComponent(component), currentEntity(entities) {}

			T& operator*() const { return *currentComponent; }
			T* operator->() const { return currentComponent; }
			bool operator==(const Iterator& other) const { return currentComponent == other.currentComponent; }
			bool operator!=(const Iterator& other) const { return currentComponent != other.currentComponent; }

			// Get the entity id for the current component the iterator is referencing
			EntityId_t EntityId() { return *currentEntity; }

			Iterator& operator++()
			{
				currentComponent++;
				currentEntity++;
				return *this;
			}

		private:
			T* currentComponent;
			EntityId_t* currentEntity;
		};

		Iterator begin() { return Iterator((T*)data, indexToEntityMap); }
		Iterator end()   { return Iterator((T*)data + count, indexToEntityMap + count); }
		const Iterator begin() const { return Iterator((T*)data, indexToEntityMap); }
		const Iterator end() const   { return Iterator((T*)data + count, indexToEntityMap + count); }

	private:
		void* data = nullptr;
		T* next = nullptr;
	};

	/*
	*
	* Coordinate is the main class used to create/destroy/access entities and their components
	*
	*/
	class Coordinator
	{
	public:
		Coordinator(EntityId_t maxEntities = 1000) : maxEntities(maxEntities)
		{
			entitySignatures = new Signature[maxEntities];

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
			EntityId_t id;

			if (availableIds.size() > 0)
			{
				auto iter = availableIds.begin();
				id = *iter;
				availableIds.erase(iter);
			}
			else
			{
				id = entityCounter++;
			}

			entitySignatures[id].reset();

			return id;
		}

		void DestroyEntity(EntityId_t id)
		{
			Signature& s = entitySignatures[id];
			ComponentId_t componentId = 0;
			ComponentId_t count = (ComponentId_t)s.count(); // get the number of components to destroy
			
			while (count > 0) // while there are still components to destroy
			{
				if (HasComponent(id, componentId)) // if it has the component
				{
					componentStores[componentId]->DestroyComponent(id);
					count--; // count it's been destroyed
				}
				componentId++;
			}

			entitySignatures[id].reset();
			availableIds.insert(id);
		}

		EntityId_t GetEntityCount() const { return entityCounter - (EntityId_t)availableIds.size(); }
		EntityId_t GetMaxEntities() const { return maxEntities; }
		ComponentId_t GetComponentCount() const { return componentIdCounter; }

		bool TestEntity(Signature signature, EntityId_t id) { return (entitySignatures[id] & signature) == signature; }

		template <typename T>
		T* AddComponent(EntityId_t id)
		{
			assert(!HasComponent<T>(id) && "Entity already has component");

			entitySignatures[id].set(GetComponentId<T>());
			return GetComponentStorage<T>()->Create(id);
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
		void DestroyComponent(EntityId_t id)
		{
			if (HasComponent<T>(id))
			{
				GetComponentStorage<T>()->DestroyComponent(id);
				entitySignatures[id].reset(GetComponentId<T>());
			}
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
		EntityId_t maxEntities;
		EntityId_t entityCounter = 0;
		std::set<EntityId_t> availableIds; // using something sorted. To Do: something more efficient?
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
				pStore = new ComponentStorage<T>(id, maxEntities);
				componentStores[id] = pStore;
				componentNames[id] = typeid(T).name();
			}

			return (ComponentStorage<T>*)pStore;
		}
	};

	/*
	*
	* View base/interface
	*
	*/
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

	/*
	*
	* Templated View class.
	* Iterates over the shortest list of components, checking the entity signature.
	*
	*/
	template <typename... T>
	class View : public ViewBase
	{
	public:
		View(Coordinator& coord) : ViewBase(coord)
		{
			SetSignature<T...>();

			ComponentId_t componentIds[] = { pCoord->GetComponentId<T>()... };
			ComponentIndex_t componentCount = coord.GetMaxEntities();

			// find the shortest component list to base the view on
			for (int i = 0; i < sizeof...(T); i++)
			{
				Storage* store = coord.GetComponentStore(componentIds[i]);
				if (store == nullptr) // scenario where a component has not created it's store (i.e. when component has not been used)
				{
					componentStore = nullptr;
					componentCount = 0;
					return;
				}
				else if (componentCount > store->Count())
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
			if (componentStore)
			{
				Iterator iter(pCoord, signature, componentStore->EntitiesBegin(), componentStore->EntitiesEnd());
				iter.CheckForNext();
				return iter;
			}
			else
			{
				return Iterator(pCoord, signature, nullptr, nullptr);
			}
		}

		const Iterator end() const
		{
			if (componentStore)
			{
				return Iterator(pCoord, signature, componentStore->EntitiesEnd(), componentStore->EntitiesEnd());
			}
			else
			{
				return Iterator(pCoord, signature, nullptr, nullptr);
			}
		}

		void Print(std::ostream& out)
		{
			out << "View " << signature << ", Entities to test " << componentStore->Count();
		}

	protected:
		Storage* componentStore = nullptr;
	};

	/*
	*
	* Entity View.
	* Iterates over the entities checking the signature.
	* Not as efficient of the 'View' class unless you know you're going to look at all entities.
	*
	*/
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
