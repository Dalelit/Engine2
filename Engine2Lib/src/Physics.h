#pragma once
// nVidia PhysX setup included...
// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/BuildingWithPhysX.html
// One file will fail with a missing include... remove the .h
// Then build the checked and release version of PhysX SDK in the vs solution.
//
// In this solution...
// In Engine2Lib and Engine2App
//   VC++ directories->include directories    ;$(SolutionDir)Engine2Lib\src\submodules\PhysX\physx\include;$(SolutionDir)Engine2Lib\src\submodules\PhysX\pxshared\include
//   VC++ directories->library directories    ;$(SolutionDir)Engine2Lib\src\submodules\PhysX\physx\bin\win.x86_64.vc142.mt\[debug or release]
// In Engine2App
//   Debugging->environment                   PATH=%PATH%;$(SolutionDir)Engine2Lib\src\submodules\PhysX\physx\bin\win.x86_64.vc142.mt\[debug or release]
//   Linker->additional lib dir               physx\bin\win.x86_64.vc142.mt\[checked or release]
// Create an ErrorCallback class inheriting physx::PxErrorCallback. Use code in PhysExtensions ExtDefaultErrorCallback.cpp
// Ensure C runtime settings are the same... use Runtime Library /MT
// Fix one of the inlcudes in PhysX... remove the .h
//
// To do: move this to build in the proper place, not a subdirectory of src.
// To do: have debug use the 'checked' physx build

#include <PxPhysicsAPI.h>
#include "RigidBody.h"
#include "Collider.h"
#include "Entity.h"

namespace Engine2
{
	class Physics
	{
	public:

		~Physics();
		void Initialise();

		void CreateScene();
		void AddEntity(EngineECS::EntityId_t entityId, const Transform& transform, const RigidBody& rigidBody, const Collider& collider);
		void StepSimulation(float dt);
		void UpdateTransforms(EngineECS::Coordinator& coordinator);
		void ClearScene();

	protected:
		physx::PxFoundation* mpFoundation = nullptr;
		//physx::PxPvd* mpPvd = nullptr;
		physx::PxPhysics* mpPhysics = nullptr;
		physx::PxDefaultCpuDispatcher* mpCpuDispatcher = nullptr;
		physx::PxScene* mpScene = nullptr;
		physx::PxMaterial* mpDefaultMaterial = nullptr;


		physx::PxDefaultAllocator mDefaultAllocatorCallback;

		class ErrorCallback : public physx::PxErrorCallback
		{
		public:
			void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
		} mErrorCallback;


	};
}
