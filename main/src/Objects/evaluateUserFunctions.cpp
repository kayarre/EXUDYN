/** ***********************************************************************************************
* @brief        implementation of all CObject EvaluateUserFunction... functions, 
*				to reduce dependencies on pybind and MainSystem.h
*
* @author       Gerstmayr Johannes
* @date         2021-01-04 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#include "Utilities/ExceptionsTemplates.h"
#include <pybind11/stl.h> 
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h> //accept numpy arrays: numpy array automatically converted to std::vector<Real,...> ==> accepts np.array([1,0,0]) and [1,0,0] as return value!
#include "Pymodules/PybindUtilities.h" //for CObjectGenericODE2
#include "Pymodules/PyMatrixContainer.h" //for CObjectGenericODE2

#include "Main/MainSystem.h"

#include "Autogenerated/CObjectConnectorCartesianSpringDamper.h"
#include "Autogenerated/CObjectConnectorCoordinate.h"
#include "Autogenerated/CObjectConnectorCoordinateVector.h"
#include "Autogenerated/CObjectConnectorCoordinateSpringDamper.h"
#include "Autogenerated/CObjectConnectorCoordinateSpringDamperExt.h"
#include "Autogenerated/CObjectConnectorSpringDamper.h"
#include "Autogenerated/CObjectConnectorRigidBodySpringDamper.h"
#include "Autogenerated/CObjectConnectorLinearSpringDamper.h"
#include "Autogenerated/CObjectConnectorTorsionalSpringDamper.h"

#include "Autogenerated/CObjectANCFCable2D.h"
#include "Autogenerated/CObjectFFRF.h"
#include "Autogenerated/CObjectFFRFreducedOrder.h"
#include "Autogenerated/CObjectGenericODE2.h"
#include "Autogenerated/CObjectGenericODE1.h"
#include "Autogenerated/CObjectKinematicTree.h"
#include "Autogenerated/CObjectJointGeneric.h"

#include "Autogenerated/CSensorUserFunction.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCartesianSpringDamper::EvaluateUserFunctionForce(Vector3D& force, const MainSystemBase& mainSystem, Real t, Index itemIndex, Vector3D& vPos, Vector3D& vVel) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = parameters.springForceUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, vPos, vVel,
			parameters.stiffness, parameters.damping, parameters.offset);
	}, "ObjectConnectorCartesianSpringDamper::springForceUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinate::EvaluateUserFunctionOffset(Real& offset, const MainSystemBase& mainSystem, Real t, Index itemIndex) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		offset = parameters.offsetUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, parameters.offset);
	}, "ObjectConnectorCoordinate::offsetUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinate::EvaluateUserFunctionOffset_t(Real& offset, const MainSystemBase& mainSystem, Real t, Index itemIndex) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		offset = parameters.offsetUserFunction_t.userFunction((const MainSystem&)mainSystem,  t, itemIndex, parameters.offset);
	}, "ObjectConnectorCoordinate::offsetUserFunction_t");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinateVector::EvaluateUserFunctionConstraint(Vector& algebraicEquations, const MainSystemBase& mainSystem, Real t, Index itemIndex, 
	const ResizableVector& qMarker0, const ResizableVector& qMarker1, const ResizableVector& qMarker0_t, const ResizableVector& qMarker1_t, bool velocityLevel) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		algebraicEquations.CopyFrom(parameters.constraintUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, 
			(StdVector)(qMarker0.Append(qMarker1)), (StdVector)(qMarker0_t.Append(qMarker1_t)), velocityLevel));
	}, "ObjectConnectorCoordinateVector::constraintUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinateVector::EvaluateUserFunctionJacobian(EXUmath::MatrixContainer& jacobian, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	const ResizableVector& qMarker0, const ResizableVector& qMarker1, const ResizableVector& qMarker0_t, const ResizableVector& qMarker1_t, bool velocityLevel) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		PyMatrixContainer PMC(parameters.jacobianUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			(StdVector)(qMarker0.Append(qMarker1)), (StdVector)(qMarker0_t.Append(qMarker1_t)), velocityLevel));

		CHECKandTHROW(PMC.UseDenseMatrix(), "ObjectConnectorCoordinateVector::EvaluateUserFunctionJacobian: jacobian currently only accepts dense matrices");
		jacobian.SetUseDenseMatrix();
		jacobian.GetInternalDenseMatrix() = PMC.GetInternalDenseMatrix(); 

	}, "ObjectConnectorCoordinateVector::jacobianUserFunction");
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorRigidBodySpringDamper::EvaluateUserFunctionForce(Vector6D& fLocVec6D, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Vector6D& uLoc6D, Vector6D& vLoc6D) const
{
	Vector3D u, rot, v, omega;
	for (Index i = 0; i < 3; i++)
	{
		u[i] = uLoc6D[i];
		rot[i] = uLoc6D[i + 3];
		v[i] = vLoc6D[i];
		omega[i] = vLoc6D[i + 3];
	}

	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		fLocVec6D = parameters.springForceTorqueUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			u, rot, v, omega, EXUmath::Matrix6DToStdArray66(parameters.stiffness), EXUmath::Matrix6DToStdArray66(parameters.damping),
			EXUmath::Matrix3DToStdArray33(parameters.rotationMarker0), EXUmath::Matrix3DToStdArray33(parameters.rotationMarker1),
			parameters.offset);

	}, "ObjectConnectorRigidBodySpringDamper::springForceTorqueUserFunction");
}

//! call to post Newton step user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorRigidBodySpringDamper::EvaluateUserFunctionPostNewtonStep(Vector& returnValue, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Vector& dataCoordinates, Vector6D& uLoc6D, Vector6D& vLoc6D) const
{
	Vector3D u, rot, v, omega;
	for (Index i = 0; i < 3; i++)
	{
		u[i] = uLoc6D[i];
		rot[i] = uLoc6D[i + 3];
		v[i] = vLoc6D[i];
		omega[i] = vLoc6D[i + 3];
	}

	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		returnValue = parameters.postNewtonStepUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, (std::vector<Real>)dataCoordinates,
			u, rot, v, omega, EXUmath::Matrix6DToStdArray66(parameters.stiffness), EXUmath::Matrix6DToStdArray66(parameters.damping),
			EXUmath::Matrix3DToStdArray33(parameters.rotationMarker0), EXUmath::Matrix3DToStdArray33(parameters.rotationMarker1),
			parameters.offset);

	}, "ObjectConnectorRigidBodySpringDamper::postNewtonStepUserFunction");

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorLinearSpringDamper::EvaluateUserFunctionForce(Real& force, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Real displacement, Real velocity) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = parameters.springForceUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			displacement, velocity, parameters.stiffness, parameters.damping, parameters.offset);

	}, "CObjectConnectorLinearSpringDamper::springForceUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorTorsionalSpringDamper::EvaluateUserFunctionForce(Real& torque, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Real angle, Real omega) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		torque = parameters.springTorqueUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			angle, omega, parameters.stiffness, parameters.damping, parameters.offset);

	}, "CObjectConnectorTorsionalSpringDamper::springTorqueUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinateSpringDamper::EvaluateUserFunctionForce(Real& force, const MainSystemBase& mainSystem, Real t, Index itemIndex,
    Real relPos, Real relVel) const
{
    UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
    {
        //user function args:(deltaL, deltaL_t, Real stiffness, Real damping, Real offset, Real dryFriction, Real dryFrictionProportionalZone)
        force = parameters.springForceUserFunction.userFunction((const MainSystem&)cSystemData->GetMainSystemBacklink(), t, itemIndex,
            relPos, relVel, parameters.stiffness, parameters.damping, parameters.offset); // , parameters.dryFriction, parameters.dryFrictionProportionalZone);
    }, "ObjectConnectorCoordinateSpringDamper::springForceUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorCoordinateSpringDamperExt::EvaluateUserFunctionForce(Real& force, const MainSystemBase& mainSystem, Real t, Index itemIndex,
    Real relPos, Real relVel) const
{
    UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
    {
        //user function args:(deltaL, deltaL_t, Real stiffness, Real damping, Real offset, Real dryFriction, Real dryFrictionProportionalZone)
        force = parameters.springForceUserFunction.userFunction((const MainSystem&)cSystemData->GetMainSystemBacklink(), t, itemIndex,
            relPos, relVel, parameters.stiffness, parameters.damping, parameters.offset, parameters.velocityOffset,
            parameters.fDynamicFriction, parameters.fStaticFrictionOffset, parameters.exponentialDecayStatic,
            parameters.fViscousFriction, parameters.frictionProportionalZone);
    }, "ObjectConnectorCoordinateSpringDamperExt::springForceUserFunction");
    //def UFforce(mbs, t, itemNumber, u, v, k, d, offset, vOffset, muDynamic, myStaticOffset, muExpVel, muViscous, muRegVel) :
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectJointGeneric::EvaluateUserFunctionOffset(Vector6D& offset, const MainSystemBase& mainSystem, Real t, Index itemIndex) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//user function args:(Real t, Real load)
		offset = (Vector6D)(parameters.offsetUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, parameters.offsetUserFunctionParameters));
	}, "ObjectJointGeneric::offsetUserFunction");

}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectJointGeneric::EvaluateUserFunctionOffset_t(Vector6D& offset, const MainSystemBase& mainSystem, Real t, Index itemIndex) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//user function args:(Real t, Real load)
		offset = (Vector6D)(parameters.offsetUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex, parameters.offsetUserFunctionParameters));
	}, "ObjectJointGeneric::offsetUserFunction  (called from ComputeJacobianAE)");

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectConnectorSpringDamper::EvaluateUserFunctionForce(Real& force, const MainSystemBase& mainSystem, Real t, Index itemIndex, Real deltaL, Real deltaL_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//user function args:(deltaL, deltaL_t, Real stiffness, Real damping, Real springForce)
		force = parameters.springForceUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			deltaL, deltaL_t, parameters.stiffness, parameters.damping, parameters.force);
	}, "ObjectConnectorSpringDamper::springForceUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectGenericODE2::EvaluateUserFunctionForce(Vector& force, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = (Vector)(parameters.forceUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t));
	}, "ObjectGenericODE2::forceUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectGenericODE2::EvaluateUserFunctionMassMatrix(EXUmath::MatrixContainer& massMatrix, const MainSystemBase& mainSystem, Real t, Index objectNumber,
	const StdVector& coordinates, const StdVector& coordinates_t, const ArrayIndex& ltg) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//EPyUtils::NumPy2Matrix(parameters.massMatrixUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t), massMatrix);
		massMatrix.CopyOrAddTriplets(PyMatrixContainer(parameters.massMatrixUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t)), ltg);
		//pout << "Mass=" << massMatrix.GetEXUdenseMatrix() << "\n";
	}, "ObjectGenericODE2::massMatrixUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectGenericODE2::EvaluateUserFunctionJacobian(EXUmath::MatrixContainer& jacobianODE2, const MainSystemBase& mainSystem, Real t, Index objectNumber, 
	const StdVector& coordinates, const StdVector& coordinates_t, Real factorODE2, Real factorODE2_t, const ArrayIndex& ltg) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//factorODE2 / factorODE2_t must be added in user function, ltg-transformation done by CopyOrAddTriplets
		jacobianODE2.CopyOrAddTriplets(PyMatrixContainer(parameters.jacobianUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, 
			coordinates, coordinates_t, factorODE2, factorODE2_t)), ltg);
	}, "ObjectGenericODE2::jacobianUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectGenericODE1::EvaluateUserFunctionRHS(Vector& force, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = (Vector)(parameters.rhsUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates));
	}, "ObjectGenericODE1::rhsUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectKinematicTree::EvaluateUserFunctionForce(Vector& force, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = (Vector)(parameters.forceUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t));
	}, "ObjectKinematicTree::forceUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectFFRF::EvaluateUserFunctionForce(Vector& force, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = (Vector)(parameters.forceUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t));
	}, "ObjectFFRF::forceUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectFFRF::EvaluateUserFunctionMassMatrix(Matrix& massMatrix, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		EPyUtils::NumPy2Matrix(parameters.massMatrixUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t), massMatrix);
	}, "ObjectFFRF::massMatrixUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectFFRFreducedOrder::EvaluateUserFunctionForce(Vector& force, const MainSystemBase& mainSystem, Real t, Index objectNumber, const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		force = (Vector)(parameters.forceUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t));
	}, "ObjectFFRFreducedOrder::forceUserFunction");
}

//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectFFRFreducedOrder::EvaluateUserFunctionMassMatrix(Matrix& massMatrix, const MainSystemBase& mainSystem, Real t, Index objectNumber, 
	const StdVector& coordinates, const StdVector& coordinates_t) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		EPyUtils::NumPy2Matrix(parameters.massMatrixUserFunction.userFunction((const MainSystem&)mainSystem, t, objectNumber, coordinates, coordinates_t), massMatrix);
	}, "ObjectFFRFreducedOrder::massMatrixUserFunction");
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CObjectANCFCable2D::EvaluateUserFunctionBendingMoment(Real& torque, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Real axialPositionNormalized, Real curvature, Real curvature_t, Real curvatureRef, Real physicsBendingStiffness, Real physicsBendingDamping,
	Real axialStrain, Real axialStrain_t, Real axialStrainRef) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
		{
			torque = parameters.bendingMomentUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			axialPositionNormalized, curvature, curvature_t, curvatureRef, physicsBendingStiffness, physicsBendingDamping,
			axialStrain, axialStrain_t, axialStrainRef); 
		}, "CObjectANCFCable2D::bendingMomentUserFunction");
}

void CObjectANCFCable2D::EvaluateUserFunctionAxialForce(Real& force, const MainSystemBase& mainSystem, Real t, Index itemIndex,
	Real axialPositionNormalized, Real axialStrain, Real axialStrain_t, Real axialStrainRef, Real physicsAxialStiffness, Real physicsAxialDamping,
	Real curvature, Real curvature_t, Real curvatureRef) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
		{
			force = parameters.axialForceUserFunction.userFunction((const MainSystem&)mainSystem, t, itemIndex,
			axialPositionNormalized, axialStrain, axialStrain_t, axialStrainRef, physicsAxialStiffness, physicsAxialDamping,
			curvature, curvature_t, curvatureRef);
		}, "CObjectANCFCable2D::axialForceUserFunction");
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! call to user function implemented in separate file to avoid including pybind and MainSystem.h at too many places
void CSensorUserFunction::EvaluateUserFunction(Vector& sensorValues, const MainSystemBase& mainSystem, Real t, ConfigurationType configuration) const
{
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		//user function args:(deltaL, deltaL_t, Real stiffness, Real damping, Real springForce)
		sensorValues = parameters.sensorUserFunction.userFunction((const MainSystem&)mainSystem, t, parameters.sensorNumbers, parameters.factors, configuration);
	}, "SensorUserFunction::sensorUserFunction");
}

