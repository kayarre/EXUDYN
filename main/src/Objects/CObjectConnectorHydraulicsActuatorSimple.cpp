/** ***********************************************************************************************
* @brief        Implementation of CObjectConnectorHydraulicActuatorSimple
*
* @author       Gerstmayr Johannes
* @date         2018-05-06 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#include "Utilities/ExceptionsTemplates.h"
#include "Main/CSystemData.h"
#include "Autogenerated/CObjectConnectorHydraulicActuatorSimple.h"

//special sqrt function, used for pressure-build-up equations
Real SignedSqrt(Real x)
{
	return EXUstd::SignReal(x) * sqrt(fabs(x));
}


Index CObjectConnectorHydraulicActuatorSimple::GetODE1Size() const
{
	return GetCNode(0)->GetNumberOfODE1Coordinates();
}

//compute the properties which are needed for computation of LHS and needed for OutputVariables
void CObjectConnectorHydraulicActuatorSimple::ComputeConnectorProperties(const MarkerDataStructure& markerData, Index itemIndex,
	Vector3D& relPos, Vector3D& relVel, Real& linearVelocity, Real& force, Vector3D& forceDirection) const
{
	relPos = (markerData.GetMarkerData(1).position - markerData.GetMarkerData(0).position);

	//actuatorLength measures the distance between the markers (bushings)
	Real actuatorLength = relPos.GetL2Norm();
	Real actuatorLengthInv;

	if (actuatorLength != 0.) { actuatorLengthInv = 1. / actuatorLength; }
	else { actuatorLengthInv = 1.; SysError("CObjectConnectorHydraulicActuatorSimple::ComputeODE2LHS: actuatorLength = 0"); }

	//unit direction and relative velocity of actuator
	forceDirection = actuatorLengthInv * relPos;
	relVel = (markerData.GetMarkerData(1).velocity - markerData.GetMarkerData(0).velocity);

	linearVelocity = forceDirection * relVel; //this quantity is difference from relVel.GetL2Norm()

	//stiffness term; this is the term without the jacobian [delta l_vec]; compare Shabana MultibodyDynamics1998, page 119:
	force = 0;
	if (parameters.activeConnector)
	{
		Vector2D p;
		p[0] = ((CNodeODE1*)GetCNode(0))->GetCurrentCoordinate(0);
		p[1] = ((CNodeODE1*)GetCNode(0))->GetCurrentCoordinate(1);
		force += -p[0] * parameters.chamberCrossSection0 + p[1] * parameters.chamberCrossSection1;
		//pout << "force=" << force << ", p=" << p << "\n";

		//damping term  + force:
		force += parameters.actuatorDamping * linearVelocity; // +parameters.force;

	}
}

//! Computational function: compute left-hand-side (LHS) of second order ordinary differential equations (ODE) to "ode2Lhs"
//  MODEL: f
void CObjectConnectorHydraulicActuatorSimple::ComputeODE2LHS(Vector& ode2Lhs, const MarkerDataStructure& markerData, Index objectNumber) const
{
	//relative position, spring length and inverse spring length
	CHECKandTHROW(markerData.GetMarkerData(1).velocityAvailable && markerData.GetMarkerData(0).velocityAvailable,
		"CObjectConnectorHydraulicActuatorSimple::ComputeODE2LHS: marker do not provide velocityLevel information");

	//link separate vectors to result (ode2Lhs) vector
	ode2Lhs.SetNumberOfItems(markerData.GetMarkerData(0).positionJacobian.NumberOfColumns() + markerData.GetMarkerData(1).positionJacobian.NumberOfColumns());
	ode2Lhs.SetAll(0.); //this is the default; used if !activeConnector

	if (parameters.activeConnector)
	{
		Real force, linearVelocity;
		Vector3D relPos, relVel, forceDirection;
		ComputeConnectorProperties(markerData, objectNumber, relPos, relVel, linearVelocity, force, forceDirection);
		Vector3D fVec = force * forceDirection;

		//now link ode2Lhs Vector to partial result using the two jacobians
		if (markerData.GetMarkerData(1).positionJacobian.NumberOfColumns()) //special case: COGround has (0,0) Jacobian
		{
			LinkedDataVector ldv1(ode2Lhs, markerData.GetMarkerData(0).positionJacobian.NumberOfColumns(), markerData.GetMarkerData(1).positionJacobian.NumberOfColumns());

			//ldv1 = (1.)*(markerData.GetMarkerData(1).positionJacobian.GetTransposed()*f); //slow version		
			EXUmath::MultMatrixTransposedVector(markerData.GetMarkerData(1).positionJacobian, fVec, ldv1);
		}

		if (markerData.GetMarkerData(0).positionJacobian.NumberOfColumns()) //special case: COGround has (0,0) Jacobian
		{
			LinkedDataVector ldv0(ode2Lhs, 0, markerData.GetMarkerData(0).positionJacobian.NumberOfColumns());

			//ldv0 = (-1.)*(jacobian0.GetTransposed()*f); //SLOW version
			EXUmath::MultMatrixTransposedVector(markerData.GetMarkerData(0).positionJacobian, fVec, ldv0);
			ldv0 *= -1.;
		}
	}
}

//! Computational function: compute right-hand-side (RHS) of first order ordinary differential equations (ODE) to 'ode1Rhs'
void CObjectConnectorHydraulicActuatorSimple::ComputeODE1RHS(Vector& ode1Rhs, const MarkerDataStructure& markerData, Index objectNumber) const
{
	ode1Rhs.SetNumberOfItems(GetODE1Size());
	ode1Rhs.SetAll(0.);//not needed?

	Vector3D relPos = (markerData.GetMarkerData(1).position - markerData.GetMarkerData(0).position);
	Real actuatorLength = relPos.GetL2Norm();
	Real actuatorLengthInv;

	if (actuatorLength != 0.) { actuatorLengthInv = 1. / actuatorLength; }
	else { actuatorLengthInv = 1.; SysError("CObjectConnectorHydraulicActuatorSimple::ComputeODE2LHS: actuatorLength = 0"); }

	//unit direction and relative velocity of actuator
	//Vector3D forceDirection = actuatorLengthInv * relPos;

	Vector3D relVel = (markerData.GetMarkerData(1).velocity - markerData.GetMarkerData(0).velocity);

	Real stroke_t = actuatorLengthInv * (relPos * relVel); //stroke velocity; >0 means extension
	Real stroke = actuatorLength - parameters.offsetLength;

	Real V0act = parameters.hoseVolume0;
	Real V1act = parameters.hoseVolume1;
    Real effectiveBulkModulus0 = parameters.oilBulkModulus;
    Real effectiveBulkModulus1 = parameters.oilBulkModulus;

	if (parameters.useChamberVolumeChange)
	{
		V0act += parameters.chamberCrossSection0 * stroke;
		V1act += parameters.chamberCrossSection1 * (parameters.strokeLength - stroke);

        //if volume is zero, this could only be if hoseVolume is also zero and this will raise an exception appr. 20 lines below
        if (V0act != 0 && V1act != 0) 
        {
            Real valCylBulk0 = 0;
            Real valCylBulk1 = 0;
            Real valHoseBulk0 = 0;
            Real valHoseBulk1 = 0;

            if (parameters.cylinderBulkModulus != 0)
            {
                valCylBulk0 = (V0act - parameters.hoseVolume0) / (V0act * parameters.cylinderBulkModulus);
                valCylBulk1 = (V1act - parameters.hoseVolume1) / (V1act * parameters.cylinderBulkModulus);
            }
            if (parameters.hoseBulkModulus != 0)
            {
                valHoseBulk0 = (parameters.hoseVolume0) / (V0act * parameters.hoseBulkModulus);
                valHoseBulk1 = (parameters.hoseVolume1) / (V1act * parameters.hoseBulkModulus);
            }

            effectiveBulkModulus0 = 1. / (1. / parameters.oilBulkModulus + valCylBulk0 + valHoseBulk0);
            effectiveBulkModulus1 = 1. / (1. / parameters.oilBulkModulus + valCylBulk1 + valHoseBulk1);
        }
    }

	CHECKandTHROW(V0act != 0 && V1act != 0, "CObjectConnectorHydraulicActuatorSimple::ComputeODE1RHS: chamber volume vanished; further computation not possible!");

	LinkedDataVector p = GetCNode(0)->GetCurrentCoordinateVector(); //reference values are zero!
	//pout << "ComputeODE1RHS: p=" << p << "\n";

	//valve opening decides whether flow from pump (system pressure) or into tank
	if (parameters.valveOpening0 >= 0) //pump, system pressure
	{
		ode1Rhs[0] = effectiveBulkModulus0 / V0act * (-parameters.chamberCrossSection0 * stroke_t +
			parameters.valveOpening0 * parameters.nominalFlow*SignedSqrt(parameters.systemPressure-p[0]));
	}
	else //tank
	{
		ode1Rhs[0] = effectiveBulkModulus0 / V0act * (-parameters.chamberCrossSection0 * stroke_t +
			parameters.valveOpening0 * parameters.nominalFlow*SignedSqrt(p[0] - parameters.tankPressure));
	}
    
    //valve opening decides whether flow from pump (system pressure) or into tank
	if (parameters.valveOpening1 >= 0) //pump, system pressure
	{
		ode1Rhs[1] = effectiveBulkModulus1 / V1act * ( parameters.chamberCrossSection1 * stroke_t +
			parameters.valveOpening1 * parameters.nominalFlow*SignedSqrt(parameters.systemPressure - p[1]));
	}
	else //tank
	{
		ode1Rhs[1] = effectiveBulkModulus1 / V1act * ( parameters.chamberCrossSection1 * stroke_t +
			parameters.valveOpening1 * parameters.nominalFlow*SignedSqrt(p[1] - parameters.tankPressure));
	}

}




//! return the available jacobian dependencies and the jacobians which are available as a function; if jacobian dependencies exist but are not available as a function, it is computed numerically; can be combined with 2^i enum flags
JacobianType::Type CObjectConnectorHydraulicActuatorSimple::GetAvailableJacobians() const
{
	return (JacobianType::Type)(JacobianType::ODE2_ODE2 + JacobianType::ODE2_ODE2_t +
		JacobianType::ODE1_ODE1 + JacobianType::ODE2_ODE1 + JacobianType::ODE1_ODE2 + JacobianType::ODE1_ODE2_t);
}






//! provide according output variable in "value"
void CObjectConnectorHydraulicActuatorSimple::GetOutputVariableConnector(OutputVariableType variableType, const MarkerDataStructure& markerData, Index itemIndex, Vector& value) const
{
	Real force, linearVelocity;
	Vector3D relPos, relVel, forceDirection;
	ComputeConnectorProperties(markerData, itemIndex, relPos, relVel, linearVelocity, force, forceDirection);

	switch (variableType)
	{
	case OutputVariableType::Distance: value.SetVector({ relPos.GetL2Norm() }); break;
	case OutputVariableType::Displacement: value.CopyFrom(relPos); break;
	case OutputVariableType::Velocity: value.CopyFrom(relVel); break;
	case OutputVariableType::VelocityLocal: value.SetVector({ linearVelocity }); break;
	case OutputVariableType::Force: value.CopyFrom(force*forceDirection); break;
	default:
		SysError("CObjectConnectorHydraulicActuatorSimple::GetOutputVariable failed"); //error should not occur, because types are checked!
	}
}

