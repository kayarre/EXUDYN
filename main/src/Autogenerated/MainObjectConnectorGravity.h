/** ***********************************************************************************************
* @class        MainObjectConnectorGravityParameters
* @brief        Parameter class for MainObjectConnectorGravity
*
* @author       Gerstmayr Johannes
* @date         2019-07-01 (generated)
* @date         2024-02-03  15:35:22 (last modified)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */

#ifndef MAINOBJECTCONNECTORGRAVITYPARAMETERS__H
#define MAINOBJECTCONNECTORGRAVITYPARAMETERS__H

#include <ostream>

#include "Utilities/ReleaseAssert.h"
#include "Utilities/BasicDefinitions.h"
#include "System/ItemIndices.h"

#include <pybind11/pybind11.h>      //! AUTO: include pybind for dictionary access
#include <pybind11/stl.h>           //! AUTO: needed for stl-casts; otherwise py::cast with std::vector<Real> crashes!!!
namespace py = pybind11;            //! AUTO: "py" used throughout in code
#include "Autogenerated/CObjectConnectorGravity.h"

#include "Autogenerated/VisuObjectConnectorGravity.h"

//! AUTO: Parameters for class MainObjectConnectorGravityParameters
class MainObjectConnectorGravityParameters // AUTO: 
{
public: // AUTO: 
};


/** ***********************************************************************************************
* @class        MainObjectConnectorGravity
* @brief        A connector for additing forces due to gravitational fields beween two bodies, which can be used for aerospace and small-scale astronomical problems; DO NOT USE this connector for adding gravitational forces (loads), which should be using LoadMassProportional, which is acting global and always in the same direction.
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

//! AUTO: MainObjectConnectorGravity
class MainObjectConnectorGravity: public MainObjectConnector // AUTO: 
{
protected: // AUTO: 
    CObjectConnectorGravity* cObjectConnectorGravity; //pointer to computational object (initialized in object factory) AUTO:
    VisualizationObjectConnectorGravity* visualizationObjectConnectorGravity; //pointer to computational object (initialized in object factory) AUTO:

public: // AUTO: 
    //! AUTO: default constructor with parameter initialization
    MainObjectConnectorGravity()
    {
        name = "";
    };

    // AUTO: access functions
    //! AUTO: Get pointer to computational class
    CObjectConnectorGravity* GetCObjectConnectorGravity() { return cObjectConnectorGravity; }
    //! AUTO: Get const pointer to computational class
    const CObjectConnectorGravity* GetCObjectConnectorGravity() const { return cObjectConnectorGravity; }
    //! AUTO: Set pointer to computational class (do this only in object factory!!!)
    void SetCObjectConnectorGravity(CObjectConnectorGravity* pCObjectConnectorGravity) { cObjectConnectorGravity = pCObjectConnectorGravity; }

    //! AUTO: Get pointer to visualization class
    VisualizationObjectConnectorGravity* GetVisualizationObjectConnectorGravity() { return visualizationObjectConnectorGravity; }
    //! AUTO: Get const pointer to visualization class
    const VisualizationObjectConnectorGravity* GetVisualizationObjectConnectorGravity() const { return visualizationObjectConnectorGravity; }
    //! AUTO: Set pointer to visualization class (do this only in object factory!!!)
    void SetVisualizationObjectConnectorGravity(VisualizationObjectConnectorGravity* pVisualizationObjectConnectorGravity) { visualizationObjectConnectorGravity = pVisualizationObjectConnectorGravity; }

    //! AUTO: Get const pointer to computational base class object
    virtual CObject* GetCObject() const { return cObjectConnectorGravity; }
    //! AUTO: Set pointer to computational base class object (do this only in object factory; type is NOT CHECKED!!!)
    virtual void SetCObject(CObject* pCObject) { cObjectConnectorGravity = (CObjectConnectorGravity*)pCObject; }

    //! AUTO: Get const pointer to visualization base class object
    virtual VisualizationObject* GetVisualizationObject() const { return visualizationObjectConnectorGravity; }
    //! AUTO: Set pointer to visualization base class object (do this only in object factory; type is NOT CHECKED!!!)
    virtual void SetVisualizationObject(VisualizationObject* pVisualizationObject) { visualizationObjectConnectorGravity = (VisualizationObjectConnectorGravity*)pVisualizationObject; }

    //! AUTO:  Get type name of node (without keyword 'Object'...!); could also be realized via a string -> type conversion?
    virtual const char* GetTypeName() const override
    {
        return "ConnectorGravity";
    }


    //! AUTO:  dictionary write access
    virtual void SetWithDictionary(const py::dict& d) override
    {
        cObjectConnectorGravity->GetParameters().markerNumbers = EPyUtils::GetArrayMarkerIndexSafely(d["markerNumbers"]); /* AUTO:  read out dictionary and cast to C++ type*/
        cObjectConnectorGravity->GetParameters().gravitationalConstant = py::cast<Real>(d["gravitationalConstant"]); /* AUTO:  read out dictionary and cast to C++ type*/
        cObjectConnectorGravity->GetParameters().mass0 = py::cast<Real>(d["mass0"]); /* AUTO:  read out dictionary and cast to C++ type*/
        cObjectConnectorGravity->GetParameters().mass1 = py::cast<Real>(d["mass1"]); /* AUTO:  read out dictionary and cast to C++ type*/
        if (EPyUtils::DictItemExists(d, "minDistanceRegularization")) { cObjectConnectorGravity->GetParameters().minDistanceRegularization = py::cast<Real>(d["minDistanceRegularization"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "activeConnector")) { cObjectConnectorGravity->GetParameters().activeConnector = py::cast<bool>(d["activeConnector"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        EPyUtils::SetStringSafely(d, "name", name); /*! AUTO:  safely cast to C++ type*/
        if (EPyUtils::DictItemExists(d, "Vshow")) { visualizationObjectConnectorGravity->GetShow() = py::cast<bool>(d["Vshow"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "VdrawSize")) { visualizationObjectConnectorGravity->GetDrawSize() = py::cast<float>(d["VdrawSize"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        if (EPyUtils::DictItemExists(d, "Vcolor")) { visualizationObjectConnectorGravity->GetColor() = py::cast<std::vector<float>>(d["Vcolor"]); /* AUTO:  read out dictionary and cast to C++ type*/} 
        GetCObject()->ParametersHaveChanged();
    }

    //! AUTO:  dictionary read access
    virtual py::dict GetDictionary(bool addGraphicsData=false) const override
    {
        auto d = py::dict();
        d["objectType"] = (std::string)GetTypeName();
        d["markerNumbers"] = EPyUtils::GetArrayMarkerIndex(cObjectConnectorGravity->GetParameters().markerNumbers); //! AUTO: cast variables into python (not needed for standard types) 
        d["gravitationalConstant"] = (Real)cObjectConnectorGravity->GetParameters().gravitationalConstant; //! AUTO: cast variables into python (not needed for standard types) 
        d["mass0"] = (Real)cObjectConnectorGravity->GetParameters().mass0; //! AUTO: cast variables into python (not needed for standard types) 
        d["mass1"] = (Real)cObjectConnectorGravity->GetParameters().mass1; //! AUTO: cast variables into python (not needed for standard types) 
        d["minDistanceRegularization"] = (Real)cObjectConnectorGravity->GetParameters().minDistanceRegularization; //! AUTO: cast variables into python (not needed for standard types) 
        d["activeConnector"] = (bool)cObjectConnectorGravity->GetParameters().activeConnector; //! AUTO: cast variables into python (not needed for standard types) 
        d["name"] = (std::string)name; //! AUTO: cast variables into python (not needed for standard types) 
        d["Vshow"] = (bool)visualizationObjectConnectorGravity->GetShow(); //! AUTO: cast variables into python (not needed for standard types) 
        d["VdrawSize"] = (float)visualizationObjectConnectorGravity->GetDrawSize(); //! AUTO: cast variables into python (not needed for standard types) 
        d["Vcolor"] = (std::vector<float>)visualizationObjectConnectorGravity->GetColor(); //! AUTO: cast variables into python (not needed for standard types) 
        return d; 
    }

    //! AUTO:  parameter read access
    virtual py::object GetParameter(const STDstring& parameterName) const override 
    {
        if (parameterName.compare("name") == 0) { return py::cast((std::string)name);} //! AUTO: get parameter
        else if (parameterName.compare("markerNumbers") == 0) { return py::cast(EPyUtils::GetArrayMarkerIndex(cObjectConnectorGravity->GetParameters().markerNumbers));} //! AUTO: get parameter
        else if (parameterName.compare("gravitationalConstant") == 0) { return py::cast((Real)cObjectConnectorGravity->GetParameters().gravitationalConstant);} //! AUTO: get parameter
        else if (parameterName.compare("mass0") == 0) { return py::cast((Real)cObjectConnectorGravity->GetParameters().mass0);} //! AUTO: get parameter
        else if (parameterName.compare("mass1") == 0) { return py::cast((Real)cObjectConnectorGravity->GetParameters().mass1);} //! AUTO: get parameter
        else if (parameterName.compare("minDistanceRegularization") == 0) { return py::cast((Real)cObjectConnectorGravity->GetParameters().minDistanceRegularization);} //! AUTO: get parameter
        else if (parameterName.compare("activeConnector") == 0) { return py::cast((bool)cObjectConnectorGravity->GetParameters().activeConnector);} //! AUTO: get parameter
        else if (parameterName.compare("Vshow") == 0) { return py::cast((bool)visualizationObjectConnectorGravity->GetShow());} //! AUTO: get parameter
        else if (parameterName.compare("VdrawSize") == 0) { return py::cast((float)visualizationObjectConnectorGravity->GetDrawSize());} //! AUTO: get parameter
        else if (parameterName.compare("Vcolor") == 0) { return py::cast((std::vector<float>)visualizationObjectConnectorGravity->GetColor());} //! AUTO: get parameter
        else  {PyError(STDstring("ObjectConnectorGravity::GetParameter(...): illegal parameter name ")+parameterName+" cannot be read");} // AUTO: add warning for user
        return py::object();
    }


    //! AUTO:  parameter write access
    virtual void SetParameter(const STDstring& parameterName, const py::object& value) override 
    {
        if (parameterName.compare("name") == 0) { EPyUtils::SetStringSafely(value, name); /*! AUTO:  safely cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("markerNumbers") == 0) { cObjectConnectorGravity->GetParameters().markerNumbers = EPyUtils::GetArrayMarkerIndexSafely(value); /* AUTO:  read out dictionary, check if correct index used and store (converted) Index to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("gravitationalConstant") == 0) { cObjectConnectorGravity->GetParameters().gravitationalConstant = py::cast<Real>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("mass0") == 0) { cObjectConnectorGravity->GetParameters().mass0 = py::cast<Real>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("mass1") == 0) { cObjectConnectorGravity->GetParameters().mass1 = py::cast<Real>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("minDistanceRegularization") == 0) { cObjectConnectorGravity->GetParameters().minDistanceRegularization = py::cast<Real>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("activeConnector") == 0) { cObjectConnectorGravity->GetParameters().activeConnector = py::cast<bool>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("Vshow") == 0) { visualizationObjectConnectorGravity->GetShow() = py::cast<bool>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("VdrawSize") == 0) { visualizationObjectConnectorGravity->GetDrawSize() = py::cast<float>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else if (parameterName.compare("Vcolor") == 0) { visualizationObjectConnectorGravity->GetColor() = py::cast<std::vector<float>>(value); /* AUTO:  read out dictionary and cast to C++ type*/; } //! AUTO: get parameter
        else  {PyError(STDstring("ObjectConnectorGravity::SetParameter(...): illegal parameter name ")+parameterName+" cannot be modified");} // AUTO: add warning for user
        GetCObject()->ParametersHaveChanged();
    }

};



#endif //#ifdef include once...
