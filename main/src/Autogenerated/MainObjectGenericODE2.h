/** ***********************************************************************************************
* @class        MainObjectGenericODE2Parameters
* @brief        Parameter class for MainObjectGenericODE2
*
* @author       Gerstmayr Johannes
* @date         2019-07-01 (generated)
* @date         2024-02-03  15:37:34 (last modified)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#ifndef MAINOBJECTGENERICODE2PARAMETERS__H
#define MAINOBJECTGENERICODE2PARAMETERS__H

#include <ostream>

#include "Utilities/ReleaseAssert.h"
#include "Utilities/BasicDefinitions.h"
#include "System/ItemIndices.h"

#include <pybind11/pybind11.h>      //! AUTO: include pybind for dictionary access
#include <pybind11/stl.h>           //! AUTO: needed for stl-casts; otherwise py::cast with std::vector<Real> crashes!!!
namespace py = pybind11;            //! AUTO: "py" used throughout in code
#include <pybind11/functional.h> //! AUTO: for function handling ... otherwise gives a python error (no compilation error in C++ !)
#include "Autogenerated/CObjectGenericODE2.h"

#include "Autogenerated/VisuObjectGenericODE2.h"

//! AUTO: Parameters for class MainObjectGenericODE2Parameters
class MainObjectGenericODE2Parameters // AUTO: 
{
public: // AUTO: 
};


/** ***********************************************************************************************
* @class        MainObjectGenericODE2
* @brief        A system of \f$n\f$ second order ordinary differential equations (\hac{ODE2}), having a mass matrix, damping/gyroscopic matrix, stiffness matrix and generalized forces. It can combine generic nodes, or node points. User functions can be used to compute mass matrix and generalized forces depending on given coordinates. NOTE that all matrices, vectors, etc. must have the same dimensions \f$n\f$ or \f$(n \times n)\f$, or they must be empty \f$(0 \times 0)\f$, except for the mass matrix which always needs to have dimensions \f$(n \times n)\f$.
*
* @author       Gerstmayr Johannes
* @date         2019-07-01 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#include <ostream>

#include "Utilities/ReleaseAssert.h"
#include "Utilities/BasicDefinitions.h"
#include "System/ItemIndices.h"

//! AUTO: MainObjectGenericODE2
class MainObjectGenericODE2: public MainObjectBody // AUTO: 
{
protected: // AUTO: 
    CObjectGenericODE2* cObjectGenericODE2; //pointer to computational object (initialized in object factory) AUTO:
    VisualizationObjectGenericODE2* visualizationObjectGenericODE2; //pointer to computational object (initialized in object factory) AUTO:

public: // AUTO: 
    //! AUTO: default constructor with parameter initialization
    MainObjectGenericODE2()
    {
        name = "";
    };

    // AUTO: access functions
    //! AUTO: Get pointer to computational class
    CObjectGenericODE2* GetCObjectGenericODE2() { return cObjectGenericODE2; }
    //! AUTO: Get const pointer to computational class
    const CObjectGenericODE2* GetCObjectGenericODE2() const { return cObjectGenericODE2; }
    //! AUTO: Set pointer to computational class (do this only in object factory!!!)
    void SetCObjectGenericODE2(CObjectGenericODE2* pCObjectGenericODE2) { cObjectGenericODE2 = pCObjectGenericODE2; }

    //! AUTO: Get pointer to visualization class
    VisualizationObjectGenericODE2* GetVisualizationObjectGenericODE2() { return visualizationObjectGenericODE2; }
    //! AUTO: Get const pointer to visualization class
    const VisualizationObjectGenericODE2* GetVisualizationObjectGenericODE2() const { return visualizationObjectGenericODE2; }
    //! AUTO: Set pointer to visualization class (do this only in object factory!!!)
    void SetVisualizationObjectGenericODE2(VisualizationObjectGenericODE2* pVisualizationObjectGenericODE2) { visualizationObjectGenericODE2 = pVisualizationObjectGenericODE2; }

    //! AUTO: Get const pointer to computational base class object
    virtual CObject* GetCObject() const { return cObjectGenericODE2; }
    //! AUTO: Set pointer to computational base class object (do this only in object factory; type is NOT CHECKED!!!)
    virtual void SetCObject(CObject* pCObject) { cObjectGenericODE2 = (CObjectGenericODE2*)pCObject; }

    //! AUTO: Get const pointer to visualization base class object
    virtual VisualizationObject* GetVisualizationObject() const { return visualizationObjectGenericODE2; }
    //! AUTO: Set pointer to visualization base class object (do this only in object factory; type is NOT CHECKED!!!)
    virtual void SetVisualizationObject(VisualizationObject* pVisualizationObject) { visualizationObjectGenericODE2 = (VisualizationObjectGenericODE2*)pVisualizationObject; }

    //! AUTO:  Get type name of object; could also be realized via a string -> type conversion?
    virtual const char* GetTypeName() const override
    {
        return "GenericODE2";
    }

    //! AUTO:  provide requested nodeType for objects; used for automatic checks in CheckSystemIntegrity()
    virtual Node::Type GetRequestedNodeType() const override
    {
        return Node::_None;
    }

    //! AUTO:  Check consistency prior to CSystem::Assemble(); needs to find all possible violations such that Assemble() would fail
    virtual bool CheckPreAssembleConsistency(const MainSystem& mainSystem, STDstring& errorString) const override;


    //! AUTO:  dictionary write access
    virtual void SetWithDictionary(const py::dict& d) override
    {
        cObjectGenericODE2->GetParameters().nodeNumbers = EPyUtils::GetArrayNodeIndexSafely(d["nodeNumbers"]); /* AUTO:  read out dictionary and cast to C++ type*/
        EPyUtils::SetPyMatrixContainerSafely(d, "massMatrix", cObjectGenericODE2->GetParameters().massMatrix); /*! AUTO:  safely cast to C++ type*/
        EPyUtils::SetPyMatrixContainerSafely(d, "stiffnessMatrix", cObjectGenericODE2->GetParameters().stiffnessMatrix); /*! AUTO:  safely cast to C++ type*/
        EPyUtils::SetPyMatrixContainerSafely(d, "dampingMatrix", cObjectGenericODE2->GetParameters().dampingMatrix); /*! AUTO:  safely cast to C++ type*/
        EPyUtils::SetNumpyVectorSafely(d, "forceVector", cObjectGenericODE2->GetParameters().forceVector); /*! AUTO:  safely cast to C++ type*/
        if (EPyUtils::DictItemExists(d, "forceUserFunction")) { cObjectGenericODE2->GetParameters().forceUserFunction = d["forceUserFunction"]; /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "massMatrixUserFunction")) { cObjectGenericODE2->GetParameters().massMatrixUserFunction = d["massMatrixUserFunction"]; /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "jacobianUserFunction")) { cObjectGenericODE2->GetParameters().jacobianUserFunction = d["jacobianUserFunction"]; /* AUTO:  read out dictionary and cast to C++ type*/} 
        EPyUtils::SetStringSafely(d, "name", name); /*! AUTO:  safely cast to C++ type*/
        if (EPyUtils::DictItemExists(d, "Vshow")) { visualizationObjectGenericODE2->GetShow() = py::cast<bool>(d["Vshow"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "Vcolor")) { visualizationObjectGenericODE2->GetColor() = py::cast<std::vector<float>>(d["Vcolor"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "VtriangleMesh")) { EPyUtils::SetNumpyMatrixISafely(d, "VtriangleMesh", visualizationObjectGenericODE2->GetTriangleMesh()); /*! AUTO:  safely cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "VshowNodes")) { visualizationObjectGenericODE2->GetShowNodes() = py::cast<bool>(d["VshowNodes"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "VgraphicsDataUserFunction")) { visualizationObjectGenericODE2->GetGraphicsDataUserFunction() = d["VgraphicsDataUserFunction"]; /* AUTO:  read out dictionary and cast to C++ type*/} 
        GetCObject()->ParametersHaveChanged();
    }

    //! AUTO:  dictionary read access
    virtual py::dict GetDictionary(bool addGraphicsData=false) const override
    {
        auto d = py::dict();
        d["objectType"] = (std::string)GetTypeName();
        d["nodeNumbers"] = EPyUtils::GetArrayNodeIndex(cObjectGenericODE2->GetParameters().nodeNumbers); //! AUTO: cast variables into python (not needed for standard types) 
        d["massMatrix"] = (PyMatrixContainer)cObjectGenericODE2->GetParameters().massMatrix; //! AUTO: cast variables into python (not needed for standard types) 
        d["stiffnessMatrix"] = (PyMatrixContainer)cObjectGenericODE2->GetParameters().stiffnessMatrix; //! AUTO: cast variables into python (not needed for standard types) 
        d["dampingMatrix"] = (PyMatrixContainer)cObjectGenericODE2->GetParameters().dampingMatrix; //! AUTO: cast variables into python (not needed for standard types) 
        d["forceVector"] = EPyUtils::Vector2NumPy(cObjectGenericODE2->GetParameters().forceVector); //! AUTO: cast variables into python (not needed for standard types) 
        d["forceUserFunction"] = (py::object)cObjectGenericODE2->GetParameters().forceUserFunction; //! AUTO: cast variables into python (not needed for standard types) 
        d["massMatrixUserFunction"] = (py::object)cObjectGenericODE2->GetParameters().massMatrixUserFunction; //! AUTO: cast variables into python (not needed for standard types) 
        d["jacobianUserFunction"] = (py::object)cObjectGenericODE2->GetParameters().jacobianUserFunction; //! AUTO: cast variables into python (not needed for standard types) 
        d["coordinateIndexPerNode"] = (std::vector<Index>)cObjectGenericODE2->GetParameters().coordinateIndexPerNode; //! AUTO: cast variables into python (not needed for standard types) 
        d["tempCoordinates"] = EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates()); //! AUTO: cast variables into python (not needed for standard types) 
        d["tempCoordinates_t"] = EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates_t()); //! AUTO: cast variables into python (not needed for standard types) 
        d["tempCoordinates_tt"] = EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates_tt()); //! AUTO: cast variables into python (not needed for standard types) 
        d["name"] = (std::string)name; //! AUTO: cast variables into python (not needed for standard types) 
        d["Vshow"] = (bool)visualizationObjectGenericODE2->GetShow(); //! AUTO: cast variables into python (not needed for standard types) 
        d["Vcolor"] = (std::vector<float>)visualizationObjectGenericODE2->GetColor(); //! AUTO: cast variables into python (not needed for standard types) 
        d["VtriangleMesh"] = EPyUtils::MatrixI2NumPy(visualizationObjectGenericODE2->GetTriangleMesh()); //! AUTO: cast variables into python (not needed for standard types) 
        d["VshowNodes"] = (bool)visualizationObjectGenericODE2->GetShowNodes(); //! AUTO: cast variables into python (not needed for standard types) 
        d["VgraphicsDataUserFunction"] = (py::object)visualizationObjectGenericODE2->GetGraphicsDataUserFunction(); //! AUTO: cast variables into python (not needed for standard types) 
        return d; 
    }

    //! AUTO:  parameter read access
    virtual py::object GetParameter(const STDstring& parameterName) const override 
    {
        if (parameterName.compare("name") == 0) { return py::cast((std::string)name);} //! AUTO: get parameter
        else if (parameterName.compare("nodeNumbers") == 0) { return py::cast(EPyUtils::GetArrayNodeIndex(cObjectGenericODE2->GetParameters().nodeNumbers));} //! AUTO: get parameter
        else if (parameterName.compare("massMatrix") == 0) { return py::cast((PyMatrixContainer)cObjectGenericODE2->GetParameters().massMatrix);} //! AUTO: get parameter
        else if (parameterName.compare("stiffnessMatrix") == 0) { return py::cast((PyMatrixContainer)cObjectGenericODE2->GetParameters().stiffnessMatrix);} //! AUTO: get parameter
        else if (parameterName.compare("dampingMatrix") == 0) { return py::cast((PyMatrixContainer)cObjectGenericODE2->GetParameters().dampingMatrix);} //! AUTO: get parameter
        else if (parameterName.compare("forceVector") == 0) { return EPyUtils::Vector2NumPy(cObjectGenericODE2->GetParameters().forceVector);} //! AUTO: get parameter
        else if (parameterName.compare("forceUserFunction") == 0) { return cObjectGenericODE2->GetParameters().forceUserFunction.GetPythonDictionary();;} //! AUTO: get parameter
        else if (parameterName.compare("massMatrixUserFunction") == 0) { return cObjectGenericODE2->GetParameters().massMatrixUserFunction.GetPythonDictionary();;} //! AUTO: get parameter
        else if (parameterName.compare("jacobianUserFunction") == 0) { return cObjectGenericODE2->GetParameters().jacobianUserFunction.GetPythonDictionary();;} //! AUTO: get parameter
        else if (parameterName.compare("coordinateIndexPerNode") == 0) { return py::cast((std::vector<Index>)cObjectGenericODE2->GetParameters().coordinateIndexPerNode);} //! AUTO: get parameter
        else if (parameterName.compare("tempCoordinates") == 0) { return EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates());} //! AUTO: get parameter
        else if (parameterName.compare("tempCoordinates_t") == 0) { return EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates_t());} //! AUTO: get parameter
        else if (parameterName.compare("tempCoordinates_tt") == 0) { return EPyUtils::Vector2NumPy(cObjectGenericODE2->GetTempCoordinates_tt());} //! AUTO: get parameter
        else if (parameterName.compare("Vshow") == 0) { return py::cast((bool)visualizationObjectGenericODE2->GetShow());} //! AUTO: get parameter
        else if (parameterName.compare("Vcolor") == 0) { return py::cast((std::vector<float>)visualizationObjectGenericODE2->GetColor());} //! AUTO: get parameter
        else if (parameterName.compare("VtriangleMesh") == 0) { return EPyUtils::MatrixI2NumPy(visualizationObjectGenericODE2->GetTriangleMesh());} //! AUTO: get parameter
        else if (parameterName.compare("VshowNodes") == 0) { return py::cast((bool)visualizationObjectGenericODE2->GetShowNodes());} //! AUTO: get parameter
        else if (parameterName.compare("VgraphicsDataUserFunction") == 0) { return visualizationObjectGenericODE2->GetGraphicsDataUserFunction().GetPythonDictionary();;} //! AUTO: get parameter
        else  {PyError(STDstring("ObjectGenericODE2::GetParameter(...): illegal parameter name ")+parameterName+" cannot be read");} // AUTO: add warning for user
        return py::object();
    }


    //! AUTO:  parameter write access
    virtual void SetParameter(const STDstring& parameterName, const py::object& value) override 
    {
        if (parameterName.compare("name") == 0) { EPyUtils::SetStringSafely(value, name); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("nodeNumbers") == 0) { cObjectGenericODE2->GetParameters().nodeNumbers = EPyUtils::GetArrayNodeIndexSafely(value); /* AUTO:  read out dictionary, check if correct index used and store (converted) Index to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("massMatrix") == 0) { EPyUtils::SetPyMatrixContainerSafely(value, cObjectGenericODE2->GetParameters().massMatrix); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("stiffnessMatrix") == 0) { EPyUtils::SetPyMatrixContainerSafely(value, cObjectGenericODE2->GetParameters().stiffnessMatrix); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("dampingMatrix") == 0) { EPyUtils::SetPyMatrixContainerSafely(value, cObjectGenericODE2->GetParameters().dampingMatrix); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("forceVector") == 0) { EPyUtils::SetNumpyVectorSafely(value, cObjectGenericODE2->GetParameters().forceVector); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("forceUserFunction") == 0) { cObjectGenericODE2->GetParameters().forceUserFunction = value; } //! AUTO: get parameter
        else if (parameterName.compare("massMatrixUserFunction") == 0) { cObjectGenericODE2->GetParameters().massMatrixUserFunction = value; } //! AUTO: get parameter
        else if (parameterName.compare("jacobianUserFunction") == 0) { cObjectGenericODE2->GetParameters().jacobianUserFunction = value; } //! AUTO: get parameter
        else if (parameterName.compare("Vshow") == 0) { visualizationObjectGenericODE2->GetShow() = py::cast<bool>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("Vcolor") == 0) { visualizationObjectGenericODE2->GetColor() = py::cast<std::vector<float>>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("VtriangleMesh") == 0) { EPyUtils::SetNumpyMatrixISafely(value, visualizationObjectGenericODE2->GetTriangleMesh()); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("VshowNodes") == 0) { visualizationObjectGenericODE2->GetShowNodes() = py::cast<bool>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("VgraphicsDataUserFunction") == 0) { visualizationObjectGenericODE2->GetGraphicsDataUserFunction() = value; } //! AUTO: get parameter
        else  {PyError(STDstring("ObjectGenericODE2::SetParameter(...): illegal parameter name ")+parameterName+" cannot be modified");} // AUTO: add warning for user
        GetCObject()->ParametersHaveChanged();
    }

};



#endif //#ifdef include once...
