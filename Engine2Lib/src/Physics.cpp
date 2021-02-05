#include "pch.h"
#include "Common.h"
#include "Physics.h"

#pragma comment(lib,"PhysXCommon_64.lib")
#pragma comment(lib,"PhysX_64.lib")
#pragma comment(lib,"PhysXFoundation_64.lib")
#pragma comment(lib,"PhysXCooking_64.lib")
#pragma comment(lib,"PhysXExtensions_static_64.lib")

//#pragma comment(lib,"PhysXGPU_64.lib")

#define E2_PHYS_RELEASE(x) if (x) x->release(); x = nullptr;

namespace Engine2
{
	using namespace physx;

	Physics::~Physics()
	{
		// reminder: do these in reverse order they are created
		E2_PHYS_RELEASE(mpScene);
		E2_PHYS_RELEASE(mpCpuDispatcher);
		E2_PHYS_RELEASE(mpPhysics);
		E2_PHYS_RELEASE(mpFoundation);
	}

	void Physics::Initialise()
	{
		// https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Startup.html

		E2_LOG_INFO("Physics initialise...");

		mpFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mErrorCallback);
		E2_ASSERT(mpFoundation, "PxCreateFoundation failed!");


		//mpPvd = PxCreatePvd(*mpFoundation);
		//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10); // to do: think about it. #define PVD_HOST "127.0.0.1"
		//mpPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
		//bool recordMemoryAllocations = true;
		//mpPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mpFoundation, PxTolerancesScale(), recordMemoryAllocations, mpPvd);

		mpPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mpFoundation, PxTolerancesScale(), false, nullptr);
		E2_ASSERT(mpPhysics, "PxCreatePhysics failed!");

		//mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(scale));
		//if (!mCooking) fatalError("PxCreateCooking failed!");

		//if (!PxInitExtensions(*mPhysics, mPvd)) fatalError("PxInitExtensions failed!");

		mpCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		mpDefaultMaterial = mpPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	}

	void Physics::CreateScene()
	{
		E2_ASSERT(mpScene == nullptr, "Scene already exists");

		PxSceneDesc sceneDesc(mpPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = mpCpuDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		mpScene = mpPhysics->createScene(sceneDesc);
	}

	void Physics::AddEntity(EngineECS::EntityId_t entityId, const Transform& transform, const RigidBody& rigidBody, const Collider& collider)
	{
		// Shape

		PxShape* shape = nullptr;

		switch (collider.GetType())
		{
		case Collider::ColliderType::box:
			auto halfExtents = collider.HalfExtents(transform.scale);
			shape = mpPhysics->createShape(PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z), *mpDefaultMaterial);
			break;
		case Collider::ColliderType::sphere:
			shape = mpPhysics->createShape(PxSphereGeometry(collider.Radius(transform.scale)), *mpDefaultMaterial);
			break;
		default:
			E2_ASSERT(false, "Collider type not implemented");
		}

		// Body
		auto qrot = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.rotation);
		PxQuat q(qrot.m128_f32[0], qrot.m128_f32[1], qrot.m128_f32[2], qrot.m128_f32[3]);
		PxVec3 p(transform.position.m128_f32[0], transform.position.m128_f32[1], transform.position.m128_f32[2]);
		PxRigidActor* body = nullptr;
		if (rigidBody.IsKinematic())
		{
			body = mpPhysics->createRigidStatic(PxTransform(p, q));
		}
		else
		{
			body = mpPhysics->createRigidDynamic(PxTransform(p, q));
			//PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		}
		E2_ASSERT(body, "createRigidDynamic failed");
		body->attachShape(*shape);
		body->userData = reinterpret_cast<void*>(entityId);


		// Add to scene
		mpScene->addActor(*body);

		// cleanup
		shape->release();
	}

	void Physics::StepSimulation(float dt)
	{
		mpScene->simulate(dt);
		mpScene->fetchResults(true);
	}

	void Physics::UpdateTransforms(EngineECS::Coordinator& coordinator)
	{
		auto actorCount = mpScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		
		if (actorCount)
		{
			std::vector<PxRigidActor*> actors(actorCount);
			mpScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(actors.data()), actorCount);

			for (UINT32 i = 0; i < actorCount; ++i)
			{
				EngineECS::EntityId_t eid = reinterpret_cast<EngineECS::EntityId_t>(actors[i]->userData);

				auto shapesCount = actors[i]->getNbShapes();

				PxShape* shapes[1];

				if (shapesCount)
				{
					E2_ASSERT(shapesCount == 1, "Expecting only 1 shape for actor for now");

					actors[i]->getShapes(shapes, shapesCount);

					//for (UINT32 j = 0; j < shapesCount; ++j)
					{
						const PxTransform shapePose(PxShapeExt::getGlobalPose(*shapes[0], *actors[i]));
						Transform* pt = coordinator.GetComponent<Transform>(eid);
						TransformMatrix* ptm = coordinator.GetComponent<TransformMatrix>(eid);

						auto shapePos = shapePose.p;
						auto shapeQRot = shapePose.q;
						DirectX::XMVECTOR p = { shapePos.x, shapePos.y, shapePos.z, 1.0f };
						DirectX::XMVECTOR q = { shapeQRot.x, shapeQRot.y, shapeQRot.z, shapeQRot.w };

						ptm->Set(p, q, pt->scale);
					}
				}
			}

		}
	}

	void Physics::ClearScene()
	{
		mpScene->release();
		mpScene = nullptr;
	}

	void Physics::ErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		const char* errorCode = NULL;

		switch (code)
		{
		case PxErrorCode::eNO_ERROR:
			errorCode = "no error";
			break;
		case PxErrorCode::eINVALID_PARAMETER:
			errorCode = "invalid parameter";
			break;
		case PxErrorCode::eINVALID_OPERATION:
			errorCode = "invalid operation";
			break;
		case PxErrorCode::eOUT_OF_MEMORY:
			errorCode = "out of memory";
			break;
		case PxErrorCode::eDEBUG_INFO:
			errorCode = "info";
			break;
		case PxErrorCode::eDEBUG_WARNING:
			errorCode = "warning";
			break;
		case PxErrorCode::ePERF_WARNING:
			errorCode = "performance warning";
			break;
		case PxErrorCode::eABORT:
			errorCode = "abort";
			break;
		case PxErrorCode::eINTERNAL_ERROR:
			errorCode = "internal error";
			break;
		case PxErrorCode::eMASK_ALL:
			errorCode = "unknown error";
			break;
		}

		PX_ASSERT(errorCode);
		if (errorCode)
		{
			//char buffer[1024];
			//sprintf(buffer, "%s (%d) : %s : %s\n", file, line, errorCode, message);
			std::stringstream buffer;
			buffer << file << " (" << line << ") : " << errorCode << " : " << message;

			//physx::shdfnd::printString(buffer);
			E2_LOG_ERROR(buffer.str());

			// in debug builds halt execution for abort codes
			PX_ASSERT(e != PxErrorCode::eABORT);

			// in release builds we also want to halt execution 
			// and make sure that the error message is flushed  
			while (code == PxErrorCode::eABORT)
			{
				//physx::shdfnd::printString(buffer);
				E2_LOG_ERROR(buffer);
				//physx::shdfnd::Thread::sleep(1000);
				Sleep(1000);
			}
		}
	}
}
