/** ***********************************************************************************************
* @brief        Implementation of VisualizationUserFunctions
*
* @author       Gerstmayr Johannes
* @date         2020-06-22 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#include "Utilities/ExceptionsTemplates.h"
#include "Graphics/VisualizationSystemContainer.h"  //includes everything needed
#include "Graphics/VisualizationPrimitives.h"		//EXUvis::AddBodyGraphicsData

#include "Main/MainSystemData.h"	//for backlink to main system
#include "Main/MainSystem.h"		//for backlink to main system

#include <functional> //! AUTO: needed for std::function

#include <pybind11/stl.h> 
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h> //accept numpy arrays: numpy array automatically converted to std::vector<Real,...> ==> accepts np.array([1,0,0]) and [1,0,0] as return value!

#include "Autogenerated/VisuObjectGround.h"
#include "Autogenerated/CObjectGround.h"
#include "Autogenerated/VisuObjectRigidBody.h"
#include "Autogenerated/CObjectRigidBody.h"
#include "Autogenerated/VisuObjectRigidBody2D.h"
#include "Autogenerated/CObjectRigidBody2D.h"
#include "Autogenerated/VisuObjectGenericODE2.h"
#include "Autogenerated/CObjectGenericODE2.h"

//! user function to compute graphics for ground; this function is considered to be slow, as a lot of conversion takes place!
void VisualizationObjectGround::CallUserFunction(const VisualizationSettings& visualizationSettings, VisualizationSystem* vSystem, const MainSystem& mainSystem, Index itemNumber)
{
	py::object pyBodyGraphicsData;
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		pyBodyGraphicsData = graphicsDataUserFunction.userFunction(mainSystem, itemNumber);
	}, "VisualizationObjectGround::CallUserFunction");

	CObjectGround* cObject = (CObjectGround*)vSystem->systemData->GetCObjects()[itemNumber];

	Vector3D refPos3D = cObject->GetPosition(Vector3D(0.), ConfigurationType::Visualization);
	Float3 refPos3DF;
	refPos3DF.CopyFrom(refPos3D); // ({ (float)pos3D[0], (float)pos3D[1], (float)pos3D[2] });

	BodyGraphicsData bodyGraphicsData;
	PyWriteBodyGraphicsDataList(pyBodyGraphicsData, bodyGraphicsData);
	EXUvis::AddBodyGraphicsData(bodyGraphicsData, vSystem->graphicsData, refPos3DF, EXUmath::unitMatrix3DF, itemNumber);

}

//! user function to compute graphics for ground; this function is considered to be slow, as a lot of conversion takes place!
void VisualizationObjectRigidBody::CallUserFunction(const VisualizationSettings& visualizationSettings, VisualizationSystem* vSystem, const MainSystem& mainSystem, Index itemNumber)
{
	py::object pyBodyGraphicsData;
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		pyBodyGraphicsData = graphicsDataUserFunction.userFunction(mainSystem, itemNumber);
	}, "VisualizationObjectRigidBody::CallUserFunction");

	CObjectRigidBody* cObject = (CObjectRigidBody*)vSystem->systemData->GetCObjects()[itemNumber];

	Vector3D refPos3D = cObject->GetPosition(Vector3D(0.), ConfigurationType::Visualization);
	Float3 refPos3DF;
	refPos3DF.CopyFrom(refPos3D);
	Matrix3DF A; A.CopyFrom(cObject->GetRotationMatrix(Vector3D(0.), ConfigurationType::Visualization));

	BodyGraphicsData bodyGraphicsData;
	PyWriteBodyGraphicsDataList(pyBodyGraphicsData, bodyGraphicsData);
	EXUvis::AddBodyGraphicsData(bodyGraphicsData, vSystem->graphicsData, refPos3DF, A, itemNumber);

}

//! user function to compute graphics for ground; this function is considered to be slow, as a lot of conversion takes place!
void VisualizationObjectRigidBody2D::CallUserFunction(const VisualizationSettings& visualizationSettings, VisualizationSystem* vSystem, const MainSystem& mainSystem, Index itemNumber)
{
	py::object pyBodyGraphicsData;
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		pyBodyGraphicsData = graphicsDataUserFunction.userFunction(mainSystem, itemNumber);
	}, "VisualizationObjectRigidBody2D::CallUserFunction");

	CObjectRigidBody2D* cObject = (CObjectRigidBody2D*)vSystem->systemData->GetCObjects()[itemNumber];

	Vector3D refPos3D = cObject->GetPosition(Vector3D(0.), ConfigurationType::Visualization);
	Float3 refPos3DF;
	refPos3DF.CopyFrom(refPos3D);
	Matrix3DF A; A.CopyFrom(cObject->GetRotationMatrix(Vector3D(0.), ConfigurationType::Visualization));

	BodyGraphicsData bodyGraphicsData;
	PyWriteBodyGraphicsDataList(pyBodyGraphicsData, bodyGraphicsData);
	EXUvis::AddBodyGraphicsData(bodyGraphicsData, vSystem->graphicsData, refPos3DF, A, itemNumber);

}

//! user function to compute graphics for ground; this function is considered to be slow, as a lot of conversion takes place!
void VisualizationObjectGenericODE2::CallUserFunction(const VisualizationSettings& visualizationSettings, VisualizationSystem* vSystem, const MainSystem& mainSystem, Index itemNumber)
{
	py::object pyBodyGraphicsData;
	UserFunctionExceptionHandling([&] //lambda function to add consistent try{..} catch(...) block
	{
		pyBodyGraphicsData = graphicsDataUserFunction.userFunction(mainSystem, itemNumber);
	}, "VisualizationObjectGenericODE2::CallUserFunction");

	//CObjectGenericODE2* cObject = (CObjectGenericODE2*)vSystem->systemData->GetCObjects()[itemNumber];

	Float3 refPos3DF = Float3(0);

	BodyGraphicsData bodyGraphicsData;
	PyWriteBodyGraphicsDataList(pyBodyGraphicsData, bodyGraphicsData);
	EXUvis::AddBodyGraphicsData(bodyGraphicsData, vSystem->graphicsData, refPos3DF, EXUmath::unitMatrix3DF, itemNumber);

}

