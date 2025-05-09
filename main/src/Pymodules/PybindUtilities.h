/** ***********************************************************************************************
* @file			PybindUtilities.h
* @brief		This file contains helper functions and utilities for pybind11 integration
* @details		Details:
* 				- Helper functions for manipulating arrays, vectors, etc.
*
* @author		Gerstmayr Johannes
* @date			2019-04-24 (created)
* @copyright	This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See 'LICENSE.txt' for more details.
* @note			Bug reports, support and further information:
* 				- email: johannes.gerstmayr@uibk.ac.at
* 				- weblink: https://github.com/jgerstmayr/EXUDYN
* 				
*
************************************************************************************************ */
#ifndef PYBINDUTILITIES__H
#define PYBINDUTILITIES__H

#include "Linalg/MatrixContainer.h"	
#include "System/ItemIndices.h"	
//#include "Pymodules/PyMatrixVector.h"
#include "Utilities/ExceptionsTemplates.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
//#include <pybind11/operators.h>
#include <pybind11/numpy.h>			//interface to numpy
//#include <pybind11/cast.h>		//
#include <pybind11/functional.h>    //not sure if this is needed, but for safety as functions are converted here as well; for function handling ... otherwise gives a python error (no compilation error in C++ !)

//typedef py::array_t<Real> PyNumpyArray; //PyNumpyArray is used at some places to avoid include of pybind

namespace py = pybind11;            //! namespace 'py' used throughout in code

//! Exudyn python utilities namespace
namespace EPyUtils { 

	//! function to check if a specific item exists (but type is not checked) in the dictionary
	inline bool DictItemExists(const py::dict& d, const char* itemName)
	{
		if (d.contains(itemName)) { return true; }
		return false;
	}

	//! return true, if dictionary contains item 'itemName' with valid string
	inline bool DictItemIsValidString(const py::dict& d, const char* itemName)
	{
		if (d.contains(itemName))
		{
			py::object other = d[itemName]; //this is necessary to make isinstance work
			if (py::isinstance<py::str>(other))
			{
				return true; //yes, item is a string
			}
		}
		return false;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! check if python type is any kind of integer; np.array uses int32_t, while standard lists use int_ types
	inline bool IsPyTypeInteger(const py::object& pyObject)
	{
		return //py::isinstance<std::int64_t>(pyObject) || //may be needed by other compilers?
			//py::isinstance<std::int8_t>(pyObject) ||
			//py::isinstance<std::int16_t>(pyObject) ||
			py::isinstance<std::int32_t>(pyObject) ||
			//py::isinstance<std::int64_t>(pyObject) ||
			//py::isinstance<std::intptr_t>(pyObject) ||
			//py::isinstance<std::int_fast32_t>(pyObject) ||
			py::isinstance<py::int_>(pyObject);
	}

	//! return true if is list or numpy array
	inline bool IsPyTypeListOrArray(const py::object& pyObject)
	{
		return (py::isinstance<py::list>(pyObject) || py::isinstance<py::array>(pyObject));
	}

	//! check if py::object is list (list of lists) or numpy array
	//! if yes, return true; columns=0 means vector (list or 1D numpy array); otherwise it is a matrix
	inline bool GetPyArrayOrListDimensions(const py::object& obj, int& rows, int& columns) 
	{
		rows = 0;
		columns = 0;

		//Check if the object is a numpy array
		if (py::isinstance<py::array>(obj)) {
			py::array arr = py::cast<py::array>(obj);
			py::buffer_info info = arr.request();

			rows = (Index)info.shape[0];
			if (info.ndim == 2) {
				columns = (Index)info.shape[1];
			}
			else
			{
				//return false => superfunction will raise Error anyways
				PyWarning("Received numpy array with invalid dimension " + EXUstd::ToString(info.ndim));
				return false;
			}
		}
		//Check if the object is a list (or list of lists)
		else if (py::isinstance<py::list>(obj)) 
		{
			py::list lst = py::cast<py::list>(obj);

			rows = (Index)lst.size();
			if (rows > 0 && py::isinstance<py::list>(lst[0])) {
				// Object is a list of lists
				py::list first_row = py::cast<py::list>(lst[0]);
				columns = (Index)first_row.size();
			}
		}
		else
		{
			return false;
		}
		return true;
	}


	inline bool IsNodeIndex(const py::object& pyObject)
	{
		return	py::isinstance<NodeIndex>(pyObject) || !(
			py::isinstance<ObjectIndex>(pyObject) ||
			py::isinstance<MarkerIndex>(pyObject) ||
			py::isinstance<LoadIndex>(pyObject) ||
			py::isinstance<SensorIndex>(pyObject));
	}
	
	inline bool IsObjectIndex(const py::object& pyObject)
	{
		return	py::isinstance<ObjectIndex>(pyObject) || !(
			py::isinstance<NodeIndex>(pyObject) ||
			py::isinstance<MarkerIndex>(pyObject) ||
			py::isinstance<LoadIndex>(pyObject) ||
			py::isinstance<SensorIndex>(pyObject));
	}

	inline bool IsMarkerIndex(const py::object& pyObject)
	{
		return	py::isinstance<MarkerIndex>(pyObject) || !(
			py::isinstance<NodeIndex>(pyObject) ||
			py::isinstance<ObjectIndex>(pyObject) ||
			py::isinstance<LoadIndex>(pyObject) ||
			py::isinstance<SensorIndex>(pyObject));
	}

	inline bool IsLoadIndex(const py::object& pyObject)
	{
		return	py::isinstance<LoadIndex>(pyObject) || !(
			py::isinstance<NodeIndex>(pyObject) ||
			py::isinstance<MarkerIndex>(pyObject) ||
			py::isinstance<ObjectIndex>(pyObject) ||
			py::isinstance<SensorIndex>(pyObject));
	}

	inline bool IsSensorIndex(const py::object& pyObject)
	{
		return	py::isinstance<SensorIndex>(pyObject) || !(
			py::isinstance<NodeIndex>(pyObject) ||
			py::isinstance<MarkerIndex>(pyObject) ||
			py::isinstance<LoadIndex>(pyObject) ||
			py::isinstance<ObjectIndex>(pyObject));
	}

	//! Expect Index or NodeIndex; otherwise throws error
	inline Index GetNodeIndexSafely(const py::object& pyObject)
	{
		//if (IsPyTypeInteger(pyObject) || py::isinstance<NodeIndex>(pyObject))
		if (IsNodeIndex(pyObject))
		{
			return py::cast<Index>(pyObject);
		}
		//otherwise:
		PyError(STDstring("Expected NodeIndex, but received '" + EXUstd::ToString(pyObject) + "', type=" + EXUstd::ToString(pyObject.get_type()) +
			"'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...)!"));
		return EXUstd::InvalidIndex;
	}

	//! Expect Index or ObjectIndex; otherwise throws error
	inline Index GetObjectIndexSafely(const py::object& pyObject)
	{
		//if (IsPyTypeInteger(pyObject) || py::isinstance<ObjectIndex>(pyObject))
		if (IsObjectIndex(pyObject))
		{
			return py::cast<Index>(pyObject);
		}
		//otherwise:
		PyError(STDstring("Expected ObjectIndex, but received '" + EXUstd::ToString(pyObject) + "', type=" + EXUstd::ToString(pyObject.get_type()) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...)!"));
		return EXUstd::InvalidIndex;
	}

	//! Expect Index or MarkerIndex; otherwise throws error
	inline Index GetMarkerIndexSafely(const py::object& pyObject)
	{
		//check different int types ...
		//if (IsPyTypeInteger(pyObject) || py::isinstance<MarkerIndex>(pyObject))
		if (IsMarkerIndex(pyObject))
		{
			return py::cast<Index>(pyObject);
		}
		//otherwise:
		PyError(STDstring("Expected MarkerIndex, but received '" + EXUstd::ToString(pyObject) + "', type=" + EXUstd::ToString(pyObject.get_type()) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...)!"));
		return EXUstd::InvalidIndex;
	}

	//! Expect Index or LoadIndex; otherwise throws error
	inline Index GetLoadIndexSafely(const py::object& pyObject)
	{
		//if (IsPyTypeInteger(pyObject) || py::isinstance<LoadIndex>(pyObject))
		if (IsLoadIndex(pyObject))
		{
			return py::cast<Index>(pyObject);
		}
		//otherwise:
		PyError(STDstring("Expected LoadIndex, but received '" + EXUstd::ToString(pyObject) + "', type=" + EXUstd::ToString(pyObject.get_type()) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...)!"));
		return EXUstd::InvalidIndex;
	}

	//! Expect Index or SensorIndex; otherwise throws error
	inline Index GetSensorIndexSafely(const py::object& pyObject)
	{
		//if (IsPyTypeInteger(pyObject) || py::isinstance<SensorIndex>(pyObject))
		if (IsSensorIndex(pyObject))
		{
			return py::cast<Index>(pyObject);
		}
		//otherwise:
		PyError(STDstring("Expected SensorIndex, but received '" + EXUstd::ToString(pyObject) + "', type=" + EXUstd::ToString(pyObject.get_type()) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...)!"));
		return EXUstd::InvalidIndex;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// some conversion functions for conversion of (internal, C++) index arrays to arrays of NodeIndex, MarkerIndex, ...

	//! Expect Index or ArrayNodeIndex; otherwise throws error
	inline ArrayIndex GetArrayNodeIndexSafely(const py::object& pyObject)
	{
        if (pyObject.is_none())
        {
            return ArrayIndex(); //empty list
        }
		else if (py::isinstance<py::list>(pyObject) || py::isinstance<py::array>(pyObject))
		{
			py::list pylist = py::cast<py::list>(pyObject); //also works for numpy arrays (but gives different type!)
			ArrayIndex indexList; //initialize empty list
			for (auto item: pylist)
			{
				indexList.Append(GetNodeIndexSafely(py::cast<py::object>(item))); //will crash, if e.g. function in list ...
			}
			return indexList;
		}
		//otherwise:
		PyError(STDstring("Expected list of NodeIndex, but received '" + EXUstd::ToString(pyObject) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...) or inconsistent arrays for nodeNumbers, markerNumbers, ...!"));
		return ArrayIndex();
	}

	//! Expect list of 2 NodeIndex; otherwise throws error
	inline Index2 GetNodeIndex2Safely(const py::object& pyObject)
	{
		ArrayIndex arrayIndex = GetArrayNodeIndexSafely(pyObject);

		if (arrayIndex.NumberOfItems() != 2)
		{
			PyError(STDstring("Expected list of 2 NodeIndex, but received " + EXUstd::ToString(arrayIndex.NumberOfItems()) + " items in list"));
			return Index2({ EXUstd::InvalidIndex, EXUstd::InvalidIndex });
		}
		return Index2(arrayIndex, 0);
	}

	//! Expect list of 3 NodeIndex; otherwise throws error
	inline Index3 GetNodeIndex3Safely(const py::object& pyObject)
	{
		ArrayIndex arrayIndex = GetArrayNodeIndexSafely(pyObject);

		if (arrayIndex.NumberOfItems() != 3)
		{
			PyError(STDstring("Expected list of 3 NodeIndex, but received " + EXUstd::ToString(arrayIndex.NumberOfItems()) + " items in list"));
			return Index3({ EXUstd::InvalidIndex, EXUstd::InvalidIndex });
		}
		return Index3(arrayIndex, 0);
	}

	//! get pybind-convertible vector of NodeIndex from ArrayIndex
	inline std::vector<NodeIndex> GetArrayNodeIndex(const ArrayIndex& arrayIndex)
	{
		std::vector<NodeIndex> vectorNodeIndex;
		for (auto item : arrayIndex)
		{
			vectorNodeIndex.push_back(NodeIndex(item));
		}
		return vectorNodeIndex;
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//! Expect Index or ArrayMarkerIndex; otherwise throws error
	inline ArrayIndex GetArrayMarkerIndexSafely(const py::object& pyObject)
	{
        if (pyObject.is_none())
        {
            return ArrayIndex(); //empty list
        }
        else if (py::isinstance<py::list>(pyObject) || py::isinstance<py::array>(pyObject))
		{
			py::list pylist = py::cast<py::list>(pyObject); //hopefully also works for numpy arrays .. TEST!
			ArrayIndex indexList; //initialize empty list
			for (auto item : pylist)
			{
				indexList.Append(GetMarkerIndexSafely(py::cast<py::object>(item))); //will crash, if e.g. function in list ...
			}
			return indexList;
		}

		//otherwise:
		PyError(STDstring("Expected list of MarkerIndex, but received '" + EXUstd::ToString(pyObject) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, MarkerIndex, ...) or inconsistent arrays for nodeNumbers, markerNumbers, ...!"));
		return ArrayIndex();
	}

	//! get pybind-convertible vector of MarkerIndex from ArrayIndex
	inline std::vector<MarkerIndex> GetArrayMarkerIndex(const ArrayIndex& arrayIndex)
	{
		std::vector<MarkerIndex> vectorMarkerIndex;
		for (auto item : arrayIndex)
		{
			vectorMarkerIndex.push_back(MarkerIndex(item));
		}
		return vectorMarkerIndex;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! Expect Index or ArraySensorIndex; otherwise throws error
	inline ArrayIndex GetArraySensorIndexSafely(const py::object& pyObject)
	{
        if (pyObject.is_none())
        {
            return ArrayIndex(); //empty list
        }
        else if (py::isinstance<py::list>(pyObject) || py::isinstance<py::array>(pyObject))
		{
			py::list pylist = py::cast<py::list>(pyObject); //hopefully also works for numpy arrays .. TEST!
			ArrayIndex indexList; //initialize empty list
			for (auto item : pylist)
			{
				indexList.Append(GetSensorIndexSafely(py::cast<py::object>(item))); //will crash, if e.g. function in list ...
			}
			return indexList;
		}

		//otherwise:
		PyError(STDstring("Expected list of SensorIndex, but received '" + EXUstd::ToString(pyObject) + "'; check potential mixing of different indices (ObjectIndex, NodeIndex, SensorIndex, ...) or inconsistent arrays for nodeNumbers, markerNumbers, ...!"));
		return ArrayIndex();
	}

	//! get pybind-convertible vector of SensorIndex from ArrayIndex
	inline std::vector<SensorIndex> GetArraySensorIndex(const ArrayIndex& arrayIndex)
	{
		std::vector<SensorIndex> vectorSensorIndex;
		for (auto item : arrayIndex)
		{
			vectorSensorIndex.push_back(SensorIndex(item));
		}
		return vectorSensorIndex;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	inline bool CheckForValidFunction(const py::object pyObject)
	{
		if (py::isinstance<py::function>(pyObject))
		{
			return true;
		}
		else if (IsPyTypeInteger(pyObject))
		{
			if (py::cast<int>(pyObject) != 0) 
			{ 
				PyError(STDstring("Failed to convert PyFunction: must be either valid Python function or 0, but got ")+EXUstd::ToString(pyObject)); 
			}
			return false; //this is a valid value, but no function (0-function pointer means empty function (in C++: nullptr))
		}
		else
		{
			PyError(STDstring("Failed to convert PyFunction: must be either valid Python function or int, but got ")+ EXUstd::ToString(pyObject));
		}
		return false;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! cast py::object to std::function; accept also zero
	template<typename STDfunction>
	STDfunction GetSTDfunction(const py::object& function, const char* info="GetSTDfunction")
	{
		if (py::isinstance<py::int_>(function))
		{
			Index num = py::cast<py::int_>(function);
			if (num != 0)
			{
				CHECKandTHROWstring( (STDstring(info)+": parameter must be either a Python function or 0 but received: "+EXUstd::ToString(function)).c_str() );
			}
			else
			{
				return 0;
			}
		}
		else if (py::isinstance<py::function>(function))

		{
			return py::cast<STDfunction>(function);
		}
		else
		{
			CHECKandTHROWstring((STDstring(info) + ": parameter must be either a Python function or 0 but received '" + EXUstd::ToString(function)+"'").c_str());
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! assign a string 'item' of a dictionary 'd' safely to 'str' and return false (if failed) and 1 of value has been set
    template<class Tstring>
	inline bool SetStringSafely(const py::dict& d, const char* itemName, Tstring& destination)
	{
		if (d.contains(itemName))
		{
			py::object other = d[itemName]; //this is necessary to make isinstance work
			if (py::isinstance<py::str>(other))
			{
				destination = py::cast<std::string>(other); //! read out dictionary and cast to C++ type
				return true;
			}
		}
		PyError(STDstring("ERROR: failed to convert '") + itemName + "' into string; dictionary:\n" + EXUstd::ToString(d));
		return false;
		//pout << "ERROR: failed to convert '" << itemName << "' into string; dictionary:\n";
		//pout << d << "\n\n";
		//return false;
	}

	template<typename T, Index size>
	inline bool SetSlimVectorTemplateSafely(const py::dict& d, const char* item, SlimVectorBase<T, size>& destination)
	{
		if (d.contains(item))
		{
			py::object other = d[item]; //this is necessary to make isinstance work
			if (py::isinstance<py::list>(other) || py::isinstance<py::array>(other))
			{
				std::vector<T> stdlist = py::cast<std::vector<T>>(other); //! # read out dictionary and cast to C++ type
				if ((Index)stdlist.size() == size)
				{
					destination = stdlist;
					return true;
				}
				else
				{
					PyError("Vector" + EXUstd::ToString(size) + "D size mismatch: expected " + EXUstd::ToString(size) + " items in list!");
				}
			}
		}
		PyError(STDstring("ERROR: failed to convert '") + item + "' into Vector" + EXUstd::ToString(size) + "D; dictionary:\n" + EXUstd::ToString(d));
		return false;
	}

	//inline bool SetVector2DSafely(const py::dict& d, const char* item, Vector2D& destination) {
	//	return SetSlimVectorTemplateSafely<Real, 2>(d, item, destination); }

	//Delete:
	//inline bool SetVector3DSafely(const py::dict& d, const char* item, Vector3D& destination) {
	//	return SetSlimVectorTemplateSafely<Real, 3>(d, item, destination);}

	//inline bool SetVector4DSafely(const py::dict& d, const char* item, Vector4D& destination) {
	//	return SetSlimVectorTemplateSafely<Real, 4>(d, item, destination);}

	//inline bool SetVector6DSafely(const py::dict& d, const char* item, Vector6D& destination) {
	//	return SetSlimVectorTemplateSafely<Real, 6>(d, item, destination);}

	//inline bool SetVector7DSafely(const py::dict& d, const char* item, Vector7D& destination) {
	//	return SetSlimVectorTemplateSafely<Real, 7>(d, item, destination);}

	template<typename T, Index rows, Index columns>
	inline bool SetConstMatrixTypeTemplateSafely(const py::object& value, ConstSizeMatrixBase<T, rows*columns>& destination)
	{
		destination.SetNumberOfRowsAndColumns(rows, columns); //2023-01-16: added for safety in future: if matrix has not been initialized before, it gives zero-sized ConstSizeMatrix!
		if (py::isinstance<py::list>(value))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(value); //! # read out dictionary and cast to C++ type
			if ((Index)stdlist.size() == rows)
			{
				for (Index i = 0; i < rows; i++)
				{
					if (py::isinstance<py::list>(stdlist[i]))
					{
						std::vector<T> rowVector = py::cast<std::vector<T>>(stdlist[i]);
						if ((Index)rowVector.size() == columns)
						{
							for (Index j = 0; j < columns; j++)
							{
								destination(i, j) = rowVector[j];
							}
						}
					}
					else
					{
						PyError("Matrix size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
					}
				}
				return true;
			}
			else
			{
				PyError("Matrix size mismatch: expected " + EXUstd::ToString(rows) + " rows!");
				//pout << "ERROR: Vector7D size mismatch: expected 4 items in list!\n";
			}
		}
		else if (py::isinstance<py::array>(value))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(value); //! # cast to C++ type
			if ((Index)stdlist.size() == rows)
			{
				for (Index i = 0; i < rows; i++)
				{
					std::vector<T> rowVector = py::cast<std::vector<T>>(stdlist[i]);
					if ((Index)rowVector.size() == columns)
					{
						for (Index j = 0; j < columns; j++)
						{
							destination(i, j) = rowVector[j];
						}
					}
					else
					{
						PyError("Matrix size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
					}
				}
				return true;
			}
			else
			{
				PyError("Matrix size mismatch: expected " + EXUstd::ToString(rows) + " rows!");
				//pout << "ERROR: Vector7D size mismatch: expected 4 items in list!\n";
			}
		}
		PyError(STDstring("failed to convert to Matrix: " + py::cast<std::string>(value)));
		return false;
	}

	//! Set a ConstMatrix of any size from a py::object safely and return false (if failed) and true if value has been set
	template<Index rows, Index columns>
	inline bool SetConstMatrixTemplateSafely(const py::object& value, ConstSizeMatrix<rows*columns>& destination)
	{
		return SetConstMatrixTypeTemplateSafely<Real, rows, columns>(value, destination);
	}

	template<Index rows, Index columns>
	inline bool SetConstMatrixTemplateSafely(const py::dict& d, const char* item, ConstSizeMatrix<rows*columns>& destination)
	{

		if (d.contains(item))
		{
			py::object other = d[item]; //this is necessary to make isinstance work
			return SetConstMatrixTemplateSafely<rows,columns>(other, destination);
		}
		PyError(STDstring("ERROR: failed to convert '") + item + "' into Matrix; dictionary:\n" + EXUstd::ToString(d));

		return false;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! Set a general sized Matrix from a py::object safely and return false (if failed) and true if value has been set
	inline bool SetMatrixSafely(const py::object& value, Matrix& destination)
	{
		if (py::isinstance<py::list>(value))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(value); //! # read out dictionary and cast to C++ type
			Index rows = (Index)stdlist.size();
			Index columns;
			for (Index i = 0; i < rows; i++)
			{
				if (py::isinstance<py::list>(stdlist[i]) || py::isinstance<py::array>(stdlist[i]))
				{
					std::vector<Real> rowVector = py::cast<std::vector<Real>>(stdlist[i]);
					if (i == 0) 
					{ 
						columns = (Index)rowVector.size();
						destination.SetNumberOfRowsAndColumns(rows, columns);
					}
					if ((Index)rowVector.size() == columns)
					{
						for (Index j = 0; j < columns; j++)
						{
							destination(i, j) = rowVector[j];
						}
					}
					else
					{
						PyError("Matrix size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
					}
				}
				else
				{
					PyError("Matrix in illegal format!");
				}
			}
			return true;
		}
		else if (py::isinstance<py::array>(value))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(value); //! # read out dictionary and cast to C++ type
			Index rows = (Index)stdlist.size();
			Index columns;
			for (Index i = 0; i < rows; i++)
			{
				std::vector<Real> rowVector = py::cast<std::vector<Real>>(stdlist[i]);
				if (i == 0) 
				{ 
					columns = (Index)rowVector.size(); 
					destination.SetNumberOfRowsAndColumns(rows, columns);
				}
				if ((Index)rowVector.size() == columns)
				{
					for (Index j = 0; j < columns; j++)
					{
						destination(i, j) = rowVector[j];
					}
				}
				else
				{
					PyError("Matrix size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
				}
			}
			return true;
		}
		PyError(STDstring("failed to convert to Matrix: " + py::cast<std::string>(value)));
		return false;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! convert pyObject as list of lists, list of arrays or 2D numpy array into destArray; works for SlimVector and SlimArray
	//! Titem can be Real or Index
	template<class TArray, class Titem>
	inline bool SetListOfArraysSafely(const py::object& pyObject, ResizableArray<TArray>& destArray)
	{
		TArray test;
		Index columns = test.NumberOfItems();
		if (py::isinstance<py::list>(pyObject))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(pyObject); //! # read out dictionary and cast to C++ type
			Index rows = (Index)stdlist.size();
			destArray.SetNumberOfItems(rows);
			for (Index i = 0; i < rows; i++)
			{
				if (py::isinstance<py::list>(stdlist[i]) || py::isinstance<py::array>(stdlist[i]))
				{
					std::vector<Titem> rowVector = py::cast<std::vector<Titem>>(stdlist[i]);
					if ((Index)rowVector.size() == columns)
					{
						for (Index j = 0; j < columns; j++)
						{
							destArray[i][j] = rowVector[j];
						}
					}
					else
					{
						PyError("List of arrays size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
					}
				}
				else
				{
					PyError("List of arrays with illegal format!");
				}
			}
			return true;
		}
		else if (py::isinstance<py::array>(pyObject))
		{
			std::vector<py::object> stdlist = py::cast<std::vector<py::object>>(pyObject); //! # read out dictionary and cast to C++ type
			Index rows = (Index)stdlist.size();
			destArray.SetNumberOfItems(rows);
			for (Index i = 0; i < rows; i++)
			{
				std::vector<Titem> rowVector = py::cast<std::vector<Titem>>(stdlist[i]);
				if ((Index)rowVector.size() == columns)
				{
					for (Index j = 0; j < columns; j++)
					{
						destArray[i][j] = rowVector[j];
					}
				}
				else
				{
					PyError("List of arrays size mismatch: expected " + EXUstd::ToString(columns) + " columns in row " + EXUstd::ToString(i) + '!');
				}
			}
			return true;
		}
		PyError(STDstring("Failed to convert to list of arrays: " + py::cast<std::string>(pyObject)));
		return false;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//functions for py::object safe conversion:

    //! assign a string of a py::object safely to 'str' and return false (if failed) and 1 of value has been set
    template<class Tstring>
    inline bool SetStringSafely(const py::object& value, Tstring& destination)
    {
        if (py::isinstance<py::str>(value))
        {
            destination = py::cast<std::string>(value); //! read out dictionary and cast to C++ type
            return true;
        }
        PyError(STDstring("failed to convert to string: " + py::cast<std::string>(value)));
        return false;
    }

    template<class T, Index size>
	inline bool SetSlimVectorTemplateSafely(const py::object& value, SlimVectorBase<T, size>& destination)
	{
		if (py::isinstance<py::list>(value) || py::isinstance<py::array>(value))
		{
			std::vector<T> stdlist = py::cast<std::vector<T>>(value); //! # read out dictionary and cast to C++ type
			if ((Index)stdlist.size() == size)
			{
				destination = stdlist;
				return true;
			}
			else
			{
				PyError("Vector" + EXUstd::ToString(size) + "D size mismatch: expected " + EXUstd::ToString(size) + " items in list!");
			}
		}
		PyError(STDstring("failed to convert SlimVector" + EXUstd::ToString(size) + ": " + py::cast<std::string>(value)));
		return false;
	}

	inline bool SetVector2DSafely(const py::object& value, Vector2D& destination) {
		return SetSlimVectorTemplateSafely<Real,2>(value, destination);
	}
	inline bool SetVector3DSafely(const py::object& value, Vector3D& destination) {
		return SetSlimVectorTemplateSafely<Real, 3>(value, destination);
	}
	inline bool SetVector4DSafely(const py::object& value, Vector4D& destination) {
		return SetSlimVectorTemplateSafely<Real, 4>(value, destination);
	}
	inline bool SetVector6DSafely(const py::object& value, Vector6D& destination) {
		return SetSlimVectorTemplateSafely<Real, 6>(value, destination);
	}
	inline bool SetVector7DSafely(const py::object& value, Vector7D& destination) {
		return SetSlimVectorTemplateSafely<Real, 7>(value, destination);
	}

	template<class T>
	inline bool SetResizableArraySafely(const py::object& value, ResizableArray<T>& destination)
	{
		if (py::isinstance<py::list>(value) || py::isinstance<py::array>(value))
		{
			std::vector<T> stdlist = py::cast<std::vector<T>>(value); //! # read out dictionary and cast to C++ type
			destination = stdlist;
			return true;
		}
		PyError(STDstring("failed to convert array to ResizableArray: " + py::cast<std::string>(value)));
		return false;
	}

	template<class T, Index size>
	inline bool SetSlimArraySafely(const py::object& value, SlimArray<T, size>& destination)
	{
		if (py::isinstance<py::list>(value) || py::isinstance<py::array>(value))
		{
			std::vector<T> stdlist = py::cast<std::vector<T>>(value); //! # read out dictionary and cast to C++ type
			if ((Index)stdlist.size() == size)
			{
				destination = stdlist;
				return true;
			}
			else
			{
				PyError("SlimArray" + EXUstd::ToString(size) + " size mismatch: expected " + EXUstd::ToString(size) + " items in list or numpy array!");
			}
		}
		PyError(STDstring("failed to convert to SlimArray" + EXUstd::ToString(size) + ": " + py::cast<std::string>(value)));
		return false;
	}


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//!convert Vector to numpy vector
	template<typename TVector>
	inline py::array_t<Real> VectorRef2NumPy(const TVector& v, bool reference)
	{
		if (reference)
		{
			return py::array_t<Real>(
				{ v.NumberOfItems() },	//shape of the array (1D array with 'len' elements)
				{ sizeof(double) },		//stride (size of one element)
				v.GetDataPointer(),		//pointer to the data
				py::none()				//none(): pybind11 does not own the memory
			);
		} 
		else
		{
			//copy data using this constructor:
			return py::array_t<Real>(v.NumberOfItems(), v.GetDataPointer());
		}
	}

	//!convert Vector to numpy vector; COPY
	inline py::array_t<Real> Vector2NumPy(const Vector& v)
	{
		return py::array_t<Real>(v.NumberOfItems(), v.GetDataPointer()); //copy array (could also be referenced!)
	}

	//!convert SlimVector to numpy vector; COPY
	template<Index dataSize>
	inline py::array_t<Real> SlimVector2NumPy(const SlimVector<dataSize>& v)
	{
		return py::array_t<Real>(v.NumberOfItems(), v.GetDataPointer()); 
	}

	//!convert ArrayIndex to numpy vector; COPY
	inline py::array_t<Index> ArrayIndex2NumPy(const ArrayIndex& v)
	{
		return py::array_t<Index>(v.NumberOfItems(), v.GetDataPointer()); 
	}

	//!convert SlimVector to numpy vector; COPY
	template<Index dataSize>
	inline py::array_t<Index> SlimArrayIndex2NumPy(const SlimArray<Index, dataSize>& v)
	{
		return py::array_t<Index>(v.NumberOfItems(), v.GetDataPointer());
	}


	//!convert Matrix to numpy matrix; COPY
	template<class TMatrix>
	py::array_t<Real> Matrix2NumPyTemplate(const TMatrix& matrix)
	{
		return py::array_t<Real>(std::vector<std::ptrdiff_t>{(int)matrix.NumberOfRows(), (int)matrix.NumberOfColumns()}, matrix.GetDataPointer());
	}

	//!convert Matrix to numpy matrix; COPY
	template<class TMatrix>
	py::array_t<float> MatrixF2NumPyTemplate(const TMatrix& matrix)
	{
		return py::array_t<float>(std::vector<std::ptrdiff_t>{(int)matrix.NumberOfRows(), (int)matrix.NumberOfColumns()}, matrix.GetDataPointer());
	}


	//!convert Matrix to numpy matrix; COPY
	inline py::array_t<Real> Matrix2NumPy(const Matrix& matrix)
	{
		return py::array_t<Real>(std::vector<std::ptrdiff_t>{(int)matrix.NumberOfRows(), (int)matrix.NumberOfColumns()}, matrix.GetDataPointer());
	}

	//!convert MatrixI to numpy matrix; COPY
	inline py::array_t<Index> MatrixI2NumPy(const MatrixI& matrix)
	{
		return py::array_t<Index>(std::vector<std::ptrdiff_t>{(int)matrix.NumberOfRows(), (int)matrix.NumberOfColumns()}, matrix.GetDataPointer());
	}

	//!convert MatrixF to numpy matrix; COPY
	inline py::array_t<float> MatrixF2NumPy(const MatrixF& matrix)
	{
		return py::array_t<float>(std::vector<std::ptrdiff_t>{(int)matrix.NumberOfRows(), (int)matrix.NumberOfColumns()}, matrix.GetDataPointer());
	}


	//!convert numpy matrix to Matrix; COPY
	template<typename T, class TMatrix>
	inline void NumPy2Matrix(const py::array_t<T>& pyArray, TMatrix& m)
	{
		if (pyArray.size() == 0) //process empty arrays, which leads to empty matrix, but has no dimension 2
		{
			m.SetNumberOfRowsAndColumns(0, 0); //empty matrix
		}
		else if (pyArray.ndim() == 2)
		{
			auto mat = pyArray.template unchecked<2>(); //template keyword needed for gcc, see: https://github.com/pybind/pybind11/issues/1412
			UnsignedIndex nrows = mat.shape(0);
			UnsignedIndex ncols = mat.shape(1);

			m.SetNumberOfRowsAndColumns((Index)nrows, (Index)ncols);
			for (UnsignedIndex i = 0; i < nrows; i++)
			{
				for (UnsignedIndex j = 0; j < ncols; j++)
				{
					m((Index)i, (Index)j) = mat(i, j);
				}
			}
		}
		else
		{
			CHECKandTHROWstring("NumPy2Matrix: failed to convert numpy array to matrix: array must have dimension 2 (rows x columns)");
		}
	}

	//!convert numpy matrix to Vector
	template<typename T>
	inline void NumPy2Vector(const py::array_t<T>& pyArray, VectorBase<T>& v)
	{
		if (pyArray.ndim() == 1)
		{
			auto pyVec = pyArray.template unchecked<1>();//template keyword needed for gcc, see: https://github.com/pybind/pybind11/issues/1412
			v.SetNumberOfItems((Index)pyVec.shape(0));

			for (Index i = 0; i < v.NumberOfItems(); i++)
			{
				v[i] = pyVec(i);
			}
		}
		else
		{
			CHECKandTHROWstring("failed to convert numpy array to vector: array must have dimension 1 (list / matrix with 1 row, no columns)");
		}
	}

	//!convert numpy matrix to Matrix
	inline Matrix NumPy2Matrix(const py::array_t<Real>& pyArray)
	{
		Matrix m;
		NumPy2Matrix(pyArray, m);
		return m;
	}

	//!convert numpy matrix to Matrix
	inline MatrixI NumPy2MatrixI(const py::array_t<Index>& pyArray)
	{
		MatrixI m;
		NumPy2Matrix(pyArray, m);
		return m;
	}

	//!convert numpy matrix to ResizableMatrix
	inline ResizableMatrix NumPy2ResizableMatrix(const py::array_t<Real>& pyArray)
	{
		ResizableMatrix m;
		NumPy2Matrix(pyArray, m);
		return m;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//numpy conversions
	template<typename T>
	inline bool SetNumpyMatrixSafelyTemplate(const py::dict& d, const char* itemName, MatrixBase<T>& destination)
	{
		if (d.contains(itemName))
		{
			py::object other = d[itemName]; //this is necessary to make isinstance work

			NumPy2Matrix<T>(py::cast<py::array_t<T>>(other), destination);
			return true;
		}
		PyError(STDstring("ERROR: failed to convert '") + itemName + "' (expected: numpy matrix) into Matrix; dictionary:\n" + EXUstd::ToString(d));
		return false;
	}

	inline bool SetNumpyMatrixISafely(const py::dict& d, const char* itemName, MatrixI& destination)
	{
		return SetNumpyMatrixSafelyTemplate<Index>(d, itemName, destination);
	}

	inline bool SetNumpyMatrixSafely(const py::dict& d, const char* itemName, Matrix& destination)
	{
		return SetNumpyMatrixSafelyTemplate<Real>(d, itemName, destination);
	}

	inline bool SetNumpyVectorSafely(const py::dict& d, const char* itemName, Vector& destination)
	{
		if (d.contains(itemName))
		{
			py::object other = d[itemName]; //this is necessary to make isinstance work

			NumPy2Vector(py::cast<py::array_t<Real>>(other), destination);
			//destination = Vector(py::cast<std::vector<Real>>(other)); //alternative via std::vector
			return true;
		}
		PyError(STDstring("ERROR: failed to convert '") + itemName + "' (expected: numpy vector) into Vector; dictionary:\n" + EXUstd::ToString(d));
		return false;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	template<class TMatrix>
	inline bool SetNumpyMatrixSafely(const py::object& value, TMatrix& destination)
	{
		NumPy2Matrix<Real>(py::cast<py::array_t<Real>>(value), destination);
		return true;
	}
	inline bool SetNumpyMatrixISafely(const py::object& value, MatrixI& destination)
	{
		NumPy2Matrix<Index>(py::cast<py::array_t<Index>>(value), destination);
		return true;
	}
	inline bool SetNumpyVectorSafely(const py::object& value, Vector& destination)
	{
		NumPy2Vector(py::cast<py::array_t<Real>>(value), destination);
		return true;
	}



} //namespace HPyUtils

#endif
