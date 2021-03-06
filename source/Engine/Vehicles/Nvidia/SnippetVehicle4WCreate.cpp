// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2017 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "Engine/Pch.h"

#include "SnippetVehicleCreate.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleSceneQuery.h"


//#pragma optimize ("", off)

namespace snippetvehicle
{

	using namespace physx;

	namespace fourwheel
	{
		void setupWheelsSimulationData
		(const YAML::Node& properties, const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
			const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
			const PxVec3& chassisCMOffset, const PxF32 chassisMass,
			PxVehicleWheelsSimData* wheelsSimData)
		{
			//Set up the wheels.
			PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
			{
				//Set up the wheel data structures with mass, moi, radius, width.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					wheels[i].mMass = wheelMass;
					wheels[i].mMOI = wheelMOI;
					wheels[i].mRadius = wheelRadius;
					wheels[i].mWidth = wheelWidth;
				}

				//Enable the handbrake for the rear wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = properties["wheels"]["maxHandBrakeTorque"].as<float>();
				wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = properties["wheels"]["maxHandBrakeTorque"].as<float>();
				//Enable steering for the front wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * properties["wheels"]["maxSteer"].as<float>();
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * properties["wheels"]["maxSteer"].as<float>();
			}

			//Set up the tires.
			PxVehicleTireData tires[PX_MAX_NB_WHEELS];
			{
				//Set up the tires.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					tires[i].mType = TIRE_TYPE_NORMAL;
				}
			}

			//Set up the suspensions
			PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
			{
				//Compute the mass supported by each suspension spring.
				PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
				PxVehicleComputeSprungMasses
				(numWheels, wheelCenterActorOffsets,
					chassisCMOffset, chassisMass, 1, suspSprungMasses);

				//Set the suspension data.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					suspensions[i].mMaxCompression = properties["suspension"]["maxCompression"].as<float>();
					suspensions[i].mMaxDroop = properties["suspension"]["maxDroop"].as<float>();
					suspensions[i].mSpringStrength = properties["suspension"]["springStrength"].as<float>();
					suspensions[i].mSpringDamperRate = properties["suspension"]["springDamperRate"].as<float>();
					suspensions[i].mSprungMass = suspSprungMasses[i];
				}

				//Set the camber angles.
				const PxF32 camberAngleAtRest = properties["suspension"]["camberAngleAtRest"].as<float>();
				const PxF32 camberAngleAtMaxDroop = properties["suspension"]["camberAngleAtMaxDroop"].as<float>();
				const PxF32 camberAngleAtMaxCompression = properties["suspension"]["camberAngleAtMaxCompression"].as<float>();
				for (PxU32 i = 0; i < numWheels; i += 2)
				{
					suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
					suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
					suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
					suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
					suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
					suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
				}
			}

			//Set up the wheel geometry.
			PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
			PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
			{
				//Set the geometry data.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					//Vertical suspension travel.
					suspTravelDirections[i] = PxVec3(0, -1, 0);

					//Wheel center offset is offset from rigid body center of mass.
					wheelCentreCMOffsets[i] =
						wheelCenterActorOffsets[i] - chassisCMOffset;

					//Suspension force application point 0.3 metres below 
					//rigid body center of mass.
					suspForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

					//Tire force application point 0.3 metres below 
					//rigid body center of mass.
					tireForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
				}
			}

			//Set up the filter data of the raycast that will be issued by each suspension.
			PxFilterData qryFilterData;
			setupNonDrivableSurface(qryFilterData);

			//Set the wheel, tire and suspension data.
			//Set the geometry data.
			//Set the query filter data
			for (PxU32 i = 0; i < numWheels; i++)
			{
				wheelsSimData->setWheelData(i, wheels[i]);
				wheelsSimData->setTireData(i, tires[i]);
				wheelsSimData->setSuspensionData(i, suspensions[i]);
				wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
				wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
				wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
				wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
				wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
				wheelsSimData->setWheelShapeMapping(i, PxI32(i));
			}

			//Add a front and rear anti-roll bar
			PxVehicleAntiRollBarData barFront;
			barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
			barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
			barFront.mStiffness = properties["rollbars"]["frontStiffness"].as<float>();
			wheelsSimData->addAntiRollBarData(barFront);
			PxVehicleAntiRollBarData barRear;
			barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
			barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
			barRear.mStiffness = properties["rollbars"]["rearStiffness"].as<float>();
			wheelsSimData->addAntiRollBarData(barRear);
		}

	} //namespace fourwheel

	PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxCooking* cooking)
	{
		const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
		const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
		const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
		const PxU32 numWheels = vehicle4WDesc.numWheels;

		const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
			//Assume all wheels are identical for simplicity.
			PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Set the meshes and materials for the non-driven wheels
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims, *physics, *cooking);
			PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			//Rigid body data.
			PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

			veh4WActor = createVehicleActor
			(rigidBodyData,
				wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
				chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
				*physics);
		}

		//Set up the sim data for the wheels.
		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
		{
			//Compute the wheel center offsets from the origin.
			PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			for (int wheel = 0; wheel < 4; ++wheel)
			{
				//wheelCenterActorOffsets[wheel] = PxVec3(vehicle4WDesc.wheelOffsets[wheel][3][0], vehicle4WDesc.wheelOffsets[wheel][3][1], vehicle4WDesc.wheelOffsets[wheel][3][2]);
				wheelCenterActorOffsets[wheel] = PxVec3(-vehicle4WDesc.wheelOffsets[wheel][3][0], vehicle4WDesc.wheelOffsets[wheel][3][1], vehicle4WDesc.wheelOffsets[wheel][3][2]);
			}

			//Set up the simulation data for all wheels.
			fourwheel::setupWheelsSimulationData
			(vehicle4WDesc.yamlProperties, vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
				numWheels, wheelCenterActorOffsets,
				vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
				wheelsSimData);
		}

		//Set up the sim data for the vehicle drive model.
		PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			PxVehicleDifferential4WData diff;
			diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			//Engine
			PxVehicleEngineData engine;
			engine.mPeakTorque = vehicle4WDesc.yamlProperties["engine"]["peakTorque"].as<float>();
			engine.mMaxOmega = vehicle4WDesc.yamlProperties["engine"]["maxOmega"].as<float>();
			driveSimData.setEngineData(engine);

			//Gears
			PxVehicleGearsData gears;
			gears.mSwitchTime = vehicle4WDesc.yamlProperties["gears"]["switchTime"].as<float>();
			driveSimData.setGearsData(gears);

			//Clutch
			PxVehicleClutchData clutch;
			clutch.mStrength = vehicle4WDesc.yamlProperties["clutch"]["strength"].as<float>();
			driveSimData.setClutchData(clutch);

			//Ackermann steer accuracy
			PxVehicleAckermannGeometryData ackermann;
			ackermann.mAccuracy = 1.0f;
			ackermann.mAxleSeparation =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
			ackermann.mFrontWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
			ackermann.mRearWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
			driveSimData.setAckermannGeometryData(ackermann);
		}

		//Create a vehicle from the wheels and drive sim data.
		PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
		vehDrive4W->setup(physics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

		//Configure the userdata
		configureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		return vehDrive4W;
	}

} //namespace snippetvehicle
