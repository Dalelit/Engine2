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

		bool recordMemoryAllocations = true;

		//mpPvd = PxCreatePvd(*mpFoundation);
		//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10); // to do: think about it. #define PVD_HOST "127.0.0.1"
		//mpPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		//mpPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mpFoundation, PxTolerancesScale(), recordMemoryAllocations, mpPvd);
		mpPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mpFoundation, PxTolerancesScale(), recordMemoryAllocations, nullptr);
		E2_ASSERT(mpPhysics, "PxCreatePhysics failed!");

		//mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(scale));
		//if (!mCooking) fatalError("PxCreateCooking failed!");

		//if (!PxInitExtensions(*mPhysics, mPvd)) fatalError("PxInitExtensions failed!");

		PxSceneDesc sceneDesc(mpPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		mpCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mpCpuDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		mpScene = mpPhysics->createScene(sceneDesc);

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
