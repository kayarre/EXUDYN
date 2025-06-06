/** ***********************************************************************************************
* @brief		Implementation of class VisualizationSystemContainer
* @details		Details:
 				- a visualization system container; links to OpenGL renderer and contains several visualization systems
*
* @author		Gerstmayr Johannes
* @date			2019-06-11 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See 'LICENSE.txt' for more details.
* @note			Bug reports, support and further information:
* 				- email: johannes.gerstmayr@uibk.ac.at
* 				- weblink: https://github.com/jgerstmayr/EXUDYN
* 				
*
* *** Example code ***
*
************************************************************************************************ */

#include "Utilities/ExceptionsTemplates.h" //for exceptions in solver steps
#include "Graphics/VisualizationSystemContainer.h"
#include "Graphics/VisualizationPrimitives.h"

#include "Graphics/GlfwClient.h" //in order to link to graphics engine
#include "Main/MainSystem.h" //for MainSystemBacklink

//#ifdef USE_GLFW_GRAPHICS
//#endif

bool VisualizationSystemContainer::AttachToRenderEngine(bool warnNoRenderer)
{
#ifdef USE_GLFW_GRAPHICS

	glfwRenderer.DetachVisualizationSystem(nullptr); //nullptr means, that every new systemcontainer links to the render engine and the old container is lost; necessary if an old systemcontainer is still linked

	if (!glfwRenderer.LinkVisualizationSystem(&graphicsDataList, &settings, this, &renderState)) 
		//(&graphicsData, &settings, this, &renderState))
	{
		//should never happen:
		SysError("VisualizationSystemContainer::AttachToRenderEngine: Renderer cannot be linked to several SystemContainers at the same time; detach other SystemContainer first!");
		return false;
	}
	return true;
#else
	if (warnNoRenderer)
	{
		PyWarning("AttachToRenderEngine(): has no effect as GLFW_GRAPHICS is deactivated in your exudyn module (needs recompile or another version)");
	}
	return false;
#endif
}

bool VisualizationSystemContainer::DetachFromRenderEngine(VisualizationSystemContainer* detachingVisualizationSystemContainer, bool warnNoRenderer)
{
#ifdef USE_GLFW_GRAPHICS
	return glfwRenderer.DetachVisualizationSystem(detachingVisualizationSystemContainer);
#else
	if (warnNoRenderer)
	{
		PyWarning("DetachFromRenderEngine(): has no effect as GLFW_GRAPHICS is deactivated in your exudyn module (needs recompile or another version)");
	}
	return false;
#endif
	
}


//! OpenGL renderer sends message that graphics shall be updated ==> update graphics data
void VisualizationSystemContainer::UpdateGraphicsData()
{
	//this is now done in GLFWclient for zoom all, InitializeView done
	//if (zoomAllRequest)
	//{
	//	zoomAllRequest = false; //this is not fully thread safe, but should not happen very often ==> user needs to zoom manually then ...

	//	InitializeView();
	//}
	if (updateGraphicsDataNow) { updateGraphicsDataNowInternal = true; updateGraphicsDataNow = false; } //enables immediate new set of updateGraphicsDataNow
	if (saveImage) { saveImageOpenGL = true; } //as graphics are updated now, the saveImageOpenGL flag can be set

	Index cnt = 0;
	for (auto item : visualizationSystems)
	{
		//pout << "UpdateGraphicsData1\n";
		item->UpdateGraphicsData(*this);
		//pout << "UpdateGraphicsData2\n";
		if (cnt == 0 && settings.general.drawWorldBasis)
		{
			EXUvis::DrawOrthonormalBasis(Vector3D({ 0,0,0 }), EXUmath::unitMatrix3D, settings.general.worldBasisSize,
				0.005*settings.general.worldBasisSize, item->GetGraphicsData(), Index2ItemID(-1, ItemType::_None, 0)); //world basis has no special index
		}
		cnt++;
	}

	updateGraphicsDataNowInternal = false; //only valid for one run; may not be earlier, as item->UpdateGraphicsData(...) needs this flag!
}

// put this to SystemContainer ...
//! perform render update and save the current openGL window to file using the visualization settings
void VisualizationSystemContainer::RedrawAndSaveImage()
{
	//now a new saveImage message can be sent
	saveImage = true;			//flag initiates saveImageOpenGL at next UpdateGraphicsData() called from Renderer
	saveImageOpenGL = false;	//after graphics update, the scene is saved and flags (saveImage, saveImageOpenGL) are set to false
	UpdateGraphicsDataNow();	//if a current redraw is performed, it will also initiate a second redraw operation ...

	Index timeOut = 500; //max iterations to wait, before frame is redrawn and saved
	Index timerMilliseconds = settings.exportImages.saveImageTimeOut / timeOut;
	if (timerMilliseconds == 0) { timerMilliseconds = 1; } //min wait time per iteration

#ifdef USE_GLFW_GRAPHICS
	if (!glfwRenderer.UseMultiThreadedRendering()) //otherwise, user functions are anyway processed
	{
		//in this case, we need to update graphics, otherwise it is not saved

		glfwRenderer.DoRendererIdleTasks(0., true); //request immediate redraw
		//now image should be saved ...
	}
#endif

	//now wait until the saveImage flag has been deleted by the current redraw operation
	Index i = 0;
	while (i++ < timeOut && (saveImageOpenGL || saveImage)) //wait timeOut*timerMilliseconds seconds for last operation to finish
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(timerMilliseconds));
#ifdef USE_GLFW_GRAPHICS
		if (glfwRenderer.UseMultiThreadedRendering()) //otherwise, user functions are anyway processed
		{
			//Needed ?: PyProcessExecuteQueue(); //use time to execute incoming python tasks
			//process user functions
			for (auto item : visualizationSystems)
			{
				item->postProcessData->ProcessUserFunctionDrawing(); //check if user functions to be drawn and do user function evaluations
			}
		}
#endif
	}
	if (saveImageOpenGL || saveImage)
	{
		PyWarning("PostProcessData::RedrawAndSaveImage: save frame to image file did not finish; increase timeout parameter");
	}
}


//! Renderer reports to simulation that simulation shall be interrupted
void VisualizationSystemContainer::StopSimulation(bool flag)
{
	//as we do not know, which simulation is executed, all system computations are interrupted
	stopSimulationFlagSC = flag; //tell also VisualizationSystemContainer
	for (auto item : visualizationSystems)
	{
		item->postProcessData->stopSimulation = flag;
	}

}

//! Renderer reports to simulation that simulation shall be interrupted
void VisualizationSystemContainer::ForceQuitSimulation(bool flag)
{
	//as we do not know, which simulation is executed, all system computations are interrupted
	for (auto item : visualizationSystems)
	{
		item->postProcessData->forceQuitSimulation = flag;
	}

}


//! Renderer reports to simulation that simulation can be continued
void VisualizationSystemContainer::ContinueSimulation()
{
	//as we do not know, which simulation is executed, all system computations are interrupted
	for (auto item : visualizationSystems)
	{
		item->postProcessData->simulationPaused = false;
	}

}

//! Renderer reports to simulation that pause flag shall be switched
void VisualizationSystemContainer::SwitchPauseSimulation()
{
    //as we do not know, which simulation is executed, all system computations are interrupted
    bool isPaused = true; //if flag is cleared, if any pause is false (this means, as soon as any pause in mbs is released, all pauses are released)
    for (auto item : visualizationSystems)
    {
        if (!item->postProcessData->simulationPaused) { isPaused = false; }
    }

    for (auto item : visualizationSystems)
    {
        item->postProcessData->simulationPaused = !isPaused; //use !isPaused, as we would like to switch from current state
    }

}

//not needed any more, as visualizationSystem has backlink possibility to VisualizationSystemContainer; 
//==> VisualizationSystemContainer has RendererIsRunning()
////! renderer signals that visualizationIsRunning flag should be set to "flag"
//void VisualizationSystemContainer::SetVisualizationIsRunning(bool flag)
//{
//	//as we do not know, which simulation is executed, all system computations are interrupted
//	for (auto item : visualizationSystems)
//	{
//		item->postProcessData->visualizationIsRunning = flag;
//	}
//}

//! check GLFW if renderer is running
bool VisualizationSystemContainer::RendererIsRunning() const
{
#ifdef USE_GLFW_GRAPHICS
	return glfwRenderer.IsGlfwInitAndRendererActive();
#else
	return false;
#endif
}


//! this function does any idle operations (execute some python commands) and returns false if stop flag in the render engine, otherwise true;
bool VisualizationSystemContainer::DoIdleOperations()
{
#ifdef USE_GLFW_GRAPHICS
	if (!stopSimulationFlagSC && RendererIsRunning())
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		PyProcessExecuteQueue(); //use time to execute incoming python tasks
		for (auto item : visualizationSystems)
		{
			item->postProcessData->ProcessUserFunctionDrawing(); //check if user functions to be drawn and do user function evaluations
		}
		RendererDoSingleThreadedIdleTasks();
		return true;
	}
	else
	{
		//if (stopSimulationFlagSC) { pout << "VisualizationSystemContainer::DoIdleOperations(): stopSimulationFlagSC was 1\n"; }
		stopSimulationFlagSC = false; //initialize the flag, if used several times; this is thread safe
		//StopSimulation(false); //also reset all simulation stop flags?
	}
#endif
	return false;
}


//! this function waits for the stop flag in the render engine;
bool VisualizationSystemContainer::WaitForRenderEngineStopFlag()
{
#ifdef USE_GLFW_GRAPHICS
	while (DoIdleOperations())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
#endif
	return true;
}

//! if the system has changed or loaded, compute maximum box of all items and reset scene to the maximum box
void VisualizationSystemContainer::InitializeView()
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	renderState.zoom = settings.openGL.initialZoom;
	renderState.maxSceneSize = settings.openGL.initialMaxSceneSize;
	renderState.centerPoint = settings.openGL.initialCenterPoint; //this is the initial centerPoint; hereafter it can be changed!
	renderState.rotationCenterPoint.SetAll(0);
	renderState.displayScaling = 1;

	renderState.currentWindowSize = settings.window.renderWindowSize;
	if (renderState.currentWindowSize[0] < 1) { renderState.currentWindowSize[0] = 1; } //avoid division by zero
	if (renderState.currentWindowSize[1] < 1) { renderState.currentWindowSize[1] = 1; } //avoid division by zero

	//set modelRotation to identity matrix (4x4); Use rotation part only from Float9 initialModelRotation
	renderState.modelRotation.SetScalarMatrix(4,1.f);
	for (Index i = 0; i < 3; i++)
	{
		for (Index j = 0; j < 3; j++)
		{
			renderState.modelRotation(i, j) = settings.openGL.initialModelRotation[i][j];
		}
	}

	renderState.projectionMatrix.SetScalarMatrix(4, 1.f);
	renderState.projectionInfo = 0;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//moved here from GlfwClient:
	renderState.mouseCoordinates.SetAll(0.);
	renderState.openGLcoordinates.SetAll(0.);
	renderState.mouseLeftPressed = false;
	renderState.mouseRightPressed = false;
	renderState.mouseMiddlePressed = false;
	renderState.joystickPosition.SetAll(0.);
	renderState.joystickRotation.SetAll(0.);
	renderState.joystickAvailable = -1; //GlfwClient::invalidIndex
	renderState.displayScaling = 1;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//openVR:
	renderState.openVRstate.Initialize(false); //disable
}

//! any multi-line text message from computation to be shown in renderer (e.g. time, solver, ...)
std::string VisualizationSystemContainer::GetComputationMessage(bool solverInformation, bool solutionInformation, bool solverTime)
{
	//workaround: take message of first system to be shown
	if (visualizationSystems.NumberOfItems())
	{
		return visualizationSystems[0]->GetComputationMessage(solverInformation, solutionInformation, solverTime);
	}
	return std::string();
}

//get marker position and orientation
void VisualizationSystemContainer::GetMarkerPositionOrientation(Index markerNumber, Index mbsNumber, Vector3D& position, Matrix3D& orientation, bool& hasPosition, bool& hasOrientation)
{
	position = Vector3D(0);
	orientation = EXUmath::unitMatrix3D;
	hasPosition = false;
	hasOrientation = false;
	if (mbsNumber >= 0 && mbsNumber < NumberOFMainSystemsBacklink())
	{
		const CSystem& cSystem = GetMainSystemBacklink(mbsNumber)->GetCSystem();
		if (markerNumber >= 0 && markerNumber < cSystem.GetSystemData().GetCMarkers().NumberOfItems())
		{
			const CMarker& marker = cSystem.GetSystemData().GetCMarker(markerNumber);
			if (EXUstd::IsOfType(marker.GetType(), Marker::Position))
			{
				marker.GetPosition(cSystem.GetSystemData(), position, ConfigurationType::Visualization);
				hasPosition = true;
			}
			if (EXUstd::IsOfType(marker.GetType(), Marker::Orientation))
			{
				marker.GetRotationMatrix(cSystem.GetSystemData(), orientation, ConfigurationType::Visualization);
				hasOrientation = true;
			}
		}
	}
	//no error, as this is inside graphics ...
}


//get sensor data list (if available)
bool VisualizationSystemContainer::GetSensorsPositionsVectorsLists(Index mbsNumber, Index positionSensorIndex, Index vectorSensorIndex, Index triadSensorIndex,
    Vector3DList& sensorTracePositions, Vector3DList& sensorTraceVectors, Matrix3DList& sensorTraceTriads, Vector sensorTraceValues, 
    const VSettingsSensorTraces& traces)
{
    sensorTracePositions.SetNumberOfItems(0);
    sensorTraceVectors.SetNumberOfItems(0);
    sensorTraceTriads.SetNumberOfItems(0);

    //bool showVectors, bool showTriads, bool showPast, bool showFuture, bool showCurrent

    if (mbsNumber >= 0 && mbsNumber < NumberOFMainSystemsBacklink())
    {
		const CSystem& cSystem = GetMainSystemBacklink(mbsNumber)->GetCSystem();
        Real time = cSystem.GetSystemData().GetCData().GetVisualization().GetTime();
        if (positionSensorIndex >= 0 && positionSensorIndex < cSystem.GetSystemData().GetCSensors().NumberOfItems())
        {
            const CSensor& sensor = *cSystem.GetSystemData().GetCSensors()[positionSensorIndex];
            if (EXUstd::IsOfType(sensor.GetOutputVariableType(), OutputVariableType::Position))
            {
                const ResizableMatrix& data = sensor.GetInternalStorage();
                if (data.NumberOfRows() > 0 and data.NumberOfColumns() == 4) //must be position compatible data
                {
                    for (Index i = 0; i < data.NumberOfRows(); i++)
                    {
                        if ((traces.showPast && data(i, 0) <= time) ||
                            (traces.showFuture && data(i, 0) > time) ||
                            (traces.showCurrent && fabs(data(i, 0) - time) < 1e-10)) //needed for current vectors
                        {
                            Vector3D v({ data(i,1), data(i,2), data(i,3) }); //time not used
                            sensorTracePositions.Append(v);
                        }
                    }
                }

                if (traces.showVectors && vectorSensorIndex >= 0 && vectorSensorIndex < cSystem.GetSystemData().GetCSensors().NumberOfItems())
                {
                    const CSensor& vectorSensor = *cSystem.GetSystemData().GetCSensors()[vectorSensorIndex];
                    const ResizableMatrix& data = vectorSensor.GetInternalStorage();
                    if (data.NumberOfRows() > 0 && data.NumberOfColumns() == 4) //must be Vector3D data
                    {
                        for (Index i = 0; i < data.NumberOfRows(); i++)
                        {
                            if ((traces.showPast && data(i, 0) <= time) ||
                                (traces.showFuture && data(i, 0) > time) ||
                                (traces.showCurrent && fabs(data(i, 0) - time) < 1e-10) )
                            {
                                Vector3D v({ data(i,1), data(i,2), data(i,3) }); //time not used
                                sensorTraceVectors.Append(v);
                            }
                        }
                        //this won't work, as in solution viewer current values are not "current"!
                        //if (showCurrent)
                        //{
                        //    vectorSensor.GetSensorValues(cSystem.GetSystemData(), sensorTraceValues, ConfigurationType::Visualization);
                        //    if (sensorTraceValues.NumberOfItems() == 3)
                        //    {
                        //        Vector3D v({ sensorTraceValues[0],
                        //            sensorTraceValues[1],
                        //            sensorTraceValues[2] }); //time not used
                        //        sensorTraceVectors.Append(v);
                        //    }
                        //}
                    }
                }
                if (traces.showTriads && triadSensorIndex >= 0 && triadSensorIndex < cSystem.GetSystemData().GetCSensors().NumberOfItems())
                {
                    //std::cout << "A," << GetOutputVariableTypeString(triadSensor.GetOutputVariableType()) << ";\n";
                    const CSensor& triadSensor = *cSystem.GetSystemData().GetCSensors()[triadSensorIndex];
                    const ResizableMatrix& data = triadSensor.GetInternalStorage();
                    if (data.NumberOfRows() > 0 && data.NumberOfColumns() == 10 &&
                        EXUstd::IsOfType(triadSensor.GetOutputVariableType(), OutputVariableType::RotationMatrix)) //must be Matrix3D data
                    {
                        for (Index i = 0; i < data.NumberOfRows(); i++)
                        {
                            if ((traces.showPast && data(i, 0) <= time) ||
                                (traces.showFuture && data(i, 0) > time) ||
                                (traces.showCurrent && fabs(data(i, 0) - time) < 1e-10) )
                            {
                                Matrix3D m(3, 3, { data(i,1), data(i,2), data(i,3),
                                data(i,4), data(i,5), data(i,6),
                                data(i,7), data(i,8), data(i,9) }); //time not used
                                sensorTraceTriads.Append(m);
                                //std::cout << "out";
                            }
                        }
                        //if (showCurrent)
                        //{
                        //    triadSensor.GetSensorValues(cSystem.GetSystemData(), sensorTraceValues, ConfigurationType::Visualization);
                        //    if (sensorTraceValues.NumberOfItems() == 10)
                        //    {
                        //        Matrix3D m(3, 3, { sensorTraceValues[1], sensorTraceValues[2], sensorTraceValues[3],
                        //            sensorTraceValues[4], sensorTraceValues[5], sensorTraceValues[6],
                        //            sensorTraceValues[7], sensorTraceValues[8], sensorTraceValues[9] }); //time not used
                        //        sensorTraceTriads.Append(m);
                        //    }
                        //}
                    }
                }
            }
        }
        if (positionSensorIndex < cSystem.GetSystemData().GetCSensors().NumberOfItems()-1)
        {
            return true;
        }
    }
    return false;
}

//! REMOVE: get backlink of ith main system (0 if not existing), temporary for selection
MainSystem* VisualizationSystemContainer::GetMainSystemBacklink(Index iSystem)
{
	if (iSystem < visualizationSystems.NumberOfItems())
	{
		return visualizationSystems[iSystem]->GetMainSystemBacklink();
	}
	else
	{
		return nullptr;
	}
}

Index VisualizationSystemContainer::NumberOFMainSystemsBacklink() const
{
	return visualizationSystems.NumberOfItems();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! object graphics data for ground objects, rigid bodies and mass points
bool PyWriteBodyGraphicsDataList(const py::dict& d, const char* item, BodyGraphicsData& data)
{
	data.FlushData(); //this is the body data, which cannot be filled incrementally
	if (d.contains(item))
	{
		GenericExceptionHandling([&]
		{
			py::object other = d[item]; //this is necessary to make isinstance work
			return PyWriteBodyGraphicsDataList(other, data, false);
		}, "Exception raised when writing BodyGraphicsData: check format");

	}//if "GraphicsData" does not exist, no error is displayed
	return true;
}
//! python function to read BodyGraphicsData from py::object, which must be a list of graphicsData dictionaries
bool PyWriteBodyGraphicsDataList(const py::object object, BodyGraphicsData& data, bool eraseData)
{
	if (eraseData) { data.FlushData(); }

	if (EPyUtils::IsPyTypeListOrArray(object)) //must be a list of graphicsData dictionaries
	{
		py::list list = (py::list)(object);

		for (auto graphicsItem : list)
		{
			//now read out dictionaries, containing graphics structures:
			//{'type':'Line', 'color':[1,0,0,1], 'data':[x1,y1,z1, x2,y2,z2, ...]}
			//{'type':'Circle', 'color':[1,0,0,1], 'radius': r, 'position': [x,y,z], 'normal': [x,y,z]}
			//{'type':'Text', 'color':[1,0,0,1], 'text':"sample text", 'position':[x,y,z]}

			if (py::isinstance<py::dict>(graphicsItem)) //must be a dictionary of graphicsData
			{
				const py::dict& gDict = (py::dict&)graphicsItem;
				if (gDict.contains("type"))
				{
					py::object pyType = gDict["type"]; //this is necessary to make isinstance work
					if (py::isinstance<py::str>(pyType))
					{
						std::string pyTypeStr = py::cast<std::string>(pyType); //! read out dictionary and cast to C++ type

						//add lines, circles, text, and triangles ....
						if (pyTypeStr == "Line")
						{

							GLLine line; line.itemID = -1;
							line.color1 = line.color2 = EXUvis::defaultColorFloat4;
							if (gDict.contains("color"))
							{
								py::object gColor = gDict["color"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gColor)) 
								{
									py::list colorList = (py::list)(gColor);
									std::vector<float> stdColorList = py::cast<std::vector<float>>(colorList); //! # read out dictionary and cast to C++ type

									if (stdColorList.size() == 4)
									{
										line.color1 = line.color2 = Float4(stdColorList);
									}
									else { PyError("GraphicsData Line: color must be a float list or numpy array with 4 components"); return false; }
								}
								else { PyError("GraphicsData Line: color must be a float list or numpy array with 4 components"); return false; }
							}
							if (gDict.contains("data"))
							{
								py::object gData = gDict["data"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gData)) 
								{
									py::list dataList = (py::list)(gData);
									std::vector<float> gd = py::cast<std::vector<float>>(dataList); //! # read out dictionary and cast to C++ type

									Index n = (Index)gd.size() / 3;
									if (n * 3 != (Index)gd.size() || n < 2)
									{
										PyError("GraphicsData Line: data must be a float list or numpy array with exactly 3*n components and n > 1"); return false;
									}

									for (Index k = 1; k < n; k++)
									{
										line.point1 = Float3({ gd[3 * (k - 1)],gd[3 * (k - 1) + 1] ,gd[3 * (k - 1) + 2] });
										line.point2 = Float3({ gd[3 * k],gd[3 * k + 1] ,gd[3 * k + 2] });
										data.glLines.Append(line);
									}
								}
								else { PyError("GraphicsData Line: data must be a float list or numpy array with 3*n components"); return false; }

							}
							else { PyError("GraphicsData Line: must contain 'data' with (x1,y1,z1,...) line coordinates "); return false; }
						} //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						else if (pyTypeStr == "Lines")
						{
							GLLine line; line.itemID = -1;
							line.color1 = line.color2 = EXUvis::defaultColorFloat4;
							std::vector<float> stdColorsList;
							std::vector<float> stdPointsList;
							Index nLines = 0;

							if (gDict.contains("points"))
							{
								py::object gData = gDict["points"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gData)) 
								{
									py::list dataList = (py::list)(gData);
									stdPointsList = py::cast<std::vector<float>>(dataList); //! # read out dictionary and cast to C++ type

									nLines = (Index)stdPointsList.size() / 6;
									if (nLines * 6 != (Index)stdPointsList.size() || nLines < 1)
									{
										PyError("GraphicsData Lines: for n lines, points must be a float list or numpy array with exactly 6*n components and n > 1"); return false;
									}
								}
								else { PyError("GraphicsData Lines: for n lines, points must be a float list or numpy array with 6*n floats, 3 floats per point"); return false; }
							}
							else { PyError("GraphicsData Lines: must contain 'points' with (x1,y1,z1,...) line coordinates "); return false; }

							if (gDict.contains("colors"))
							{
								py::object gColor = gDict["colors"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gColor)) 
								{
									py::list colorList = (py::list)(gColor);
									stdColorsList = py::cast<std::vector<float>>(colorList); //! # read out dictionary and cast to C++ type

									if (stdColorsList.size() != nLines * 8)
									{ PyError("GraphicsData Line: for n lines, colors must contain 8*n floats, 4 floats per line point"); return false; }
								}
								else { PyError("GraphicsData Line: for n lines, colors must contain 8*n floats, 4 floats per line point"); return false; }
							}
							else { PyError("GraphicsData Lines: must contain 'colors', containing 4 floats per line point"); return false; }

							for (Index k = 0; k < nLines; k++)
							{
								line.point1 = Float3({ stdPointsList[6 * k + 0],stdPointsList[6 * k + 1],stdPointsList[6 * k + 2] });
								line.point2 = Float3({ stdPointsList[6 * k + 3],stdPointsList[6 * k + 4],stdPointsList[6 * k + 5] });
								line.color1 = Float4({ stdColorsList[8 * k + 0],stdColorsList[8 * k + 1],stdColorsList[8 * k + 2],stdColorsList[8 * k + 3] });
								line.color2 = Float4({ stdColorsList[8 * k + 4],stdColorsList[8 * k + 5],stdColorsList[8 * k + 6],stdColorsList[8 * k + 7] });
								data.glLines.Append(line);
							}

						} //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						else if (pyTypeStr == "Circle")
						{
							GLCircleXY circle; circle.itemID = -1;
							circle.color = EXUvis::defaultColorFloat4;
							circle.numberOfSegments = 0; //use default

							if (gDict.contains("color"))
							{
								py::object gColor = gDict["color"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gColor)) 
								{
									py::list colorList = (py::list)(gColor);
									std::vector<float> stdColorList = py::cast<std::vector<float>>(colorList); //! # read out dictionary and cast to C++ type

									if (stdColorList.size() == 4)
									{
										circle.color = Float4(stdColorList);
									}
									else { PyError("GraphicsData Circle: color must be a float list or numpy array with 4 components"); return false; }
								}
								else { PyError("GraphicsData Circle: color must be a float list or numpy array with 4 components"); return false; }
							}

							if (gDict.contains("radius"))
							{
								py::object gData = gDict["radius"]; //this is necessary to make isinstance work
								if (py::isinstance<py::float_>(gData) || py::isinstance<py::int_>(gData)) //must be a scalar value
								{
									circle.radius = (py::float_)(gData);
								}
								else { PyError("GraphicsData Circle: radius must be a scalar value"); return false; }

							}
							else { PyError("GraphicsData Circle: must contain 'radius'"); return false; }

							if (gDict.contains("position"))
							{
								py::object gData = gDict["position"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gData))  //must be a list of 3 coordinates
								{
									py::list dataList = (py::list)(gData);
									std::vector<float> gd = py::cast<std::vector<float>>(dataList); //! # read out dictionary and cast to C++ type

									if (gd.size() != 3)
									{
										PyError("GraphicsData Circle: position must be a float list or numpy array with 3 components"); return false;
									}

									circle.point = Float3(gd);
								}
								else { PyError("GraphicsData Circle: position must be a float list or numpy array with 3 components"); return false; }

							}
							else { PyError("GraphicsData Circle: must contain 'position'"); return false; }

							data.glCirclesXY.Append(circle);

						} //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						else if (pyTypeStr == "Text")
						{
							GLText text;
							text.color = EXUvis::defaultColorFloat4;
							text.offsetX = 0.f;
							text.offsetY = 0.f;
							text.size = 0; //indicates to use default size


							if (gDict.contains("color"))
							{
								py::object gColor = gDict["color"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gColor)) 
								{
									py::list colorList = (py::list)(gColor);
									std::vector<float> stdColorList = py::cast<std::vector<float>>(colorList); //! # read out dictionary and cast to C++ type

									if (stdColorList.size() == 4)
									{
										text.color = Float4(stdColorList);
									}
									else { PyError("GraphicsData Text: color must be a float list or numpy array with 4 components"); return false; }
								}
								else { PyError("GraphicsData Text: color must be a float list or numpy array with 4 components"); return false; }
							}

							if (gDict.contains("position"))
							{
								py::object gData = gDict["position"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gData))  //must be a list of 3 coordinates
								{
									py::list dataList = (py::list)(gData);
									std::vector<float> gd = py::cast<std::vector<float>>(dataList); //! # read out dictionary and cast to C++ type

									if (gd.size() != 3)
									{
										PyError("GraphicsData Text: position must be a float list or numpy array with 3 components"); return false;
									}

									text.point = Float3(gd);

								}
								else { PyError("GraphicsData Text: position must be a float list or numpy array with 3 components"); return false; }
							}
							else { PyError("GraphicsData Text: must contain 'position'"); return false; }

							if (gDict.contains("text"))
							{
								py::object gData = gDict["text"]; //this is necessary to make isinstance work
								if (py::isinstance<py::str>(gData)) //must be a scalar value
								{
									std::string gText = (py::str)(gData);
									int len = (int)gText.size();
									text.text = new char[len + 1]; //will be deleted in destructor of GraphicsData
									//strcpy_s(text.text, len + 1, gText.c_str()); //not working with gcc
									strcpy(text.text, gText.c_str());

									data.glTexts.Append(text);
								}
								else { PyError("GraphicsData Text: 'text' must be of type string"); return false; }

							}
							else { PyError("GraphicsData Text: must contain 'text' providing a string"); return false; }

						} //end Text ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						else if (pyTypeStr == "TriangleList")
						{
							//dataTriangleList = { 'type':'TriangleList', 
							//'points' : [x0,y0,z0, x1,y1,z1, ...],
							//'colors' : [R0,G0,B0,A0, R1,G2,B1,A1, ...],
							//'normals' : [n0x,n0y,n0z, ...],
							//'triangles' : [T0point0, T0point1, T0point2, ...] }

							ResizableArray<Float3> points; //temporary data, used during evaluation of data
							ResizableArray<Float3> normals; //temporary data, used during evaluation of data (must be size of points)
							ResizableArray<Float4> colors; //temporary data, used during evaluation of data (must be size of points)

							if (gDict.contains("points"))
							{
								py::object gDictList = gDict["points"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gDictList)) 
								{
									py::list gList = (py::list)(gDictList);
									std::vector<float> stdGList = py::cast<std::vector<float>>(gList); //! # read out dictionary and cast to C++ type

									if ((stdGList.size() % 3) == 0)
									{
										Index n = (Index)stdGList.size() / 3;
										points.SetNumberOfItems(n);
										for (Index i = 0; i < n; i++)
										{
											points[i] = Float3({ stdGList[i * 3],stdGList[i * 3 + 1],stdGList[i * 3 + 2] });
											//pout << "p" << i << " = " << points[i] << "\n";
										}
									}
									else { PyError("GraphicsData::TriangleList::points must be a float list or numpy array with 3*n components, n being the number of points"); return false; }
								}
								else { PyError("GraphicsData::TriangleList::points must be a float list or numpy array"); return false; }
							}
							else
							{
								PyError("GraphicsData::TriangleList must contain 'points' being a float list or numpy array with n*(x,y,z)-components, n being the number of points");
							}

							if (gDict.contains("colors"))
							{
								py::object gDictList = gDict["colors"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gDictList)) 
								{
									py::list gList = (py::list)(gDictList);
									std::vector<float> stdGList = py::cast<std::vector<float>>(gList); //! # read out dictionary and cast to C++ type

									if ((stdGList.size() % 4) == 0 && (int)(stdGList.size() / 4) == points.NumberOfItems())
									{
										Index n = (Index)stdGList.size() / 4;
										colors.SetNumberOfItems(n);
										for (Index i = 0; i < n; i++)
										{
											colors[i] = Float4({ stdGList[i * 4], stdGList[i * 4 + 1], stdGList[i * 4 + 2], stdGList[i * 4 + 3] });
										}
									}
									else { PyError("GraphicsData::TriangleList::colors must be a float list or numpy array with 4*n components (R,G,B,A), n being the identical to the number of points"); return false; }
								}
								else { PyError("GraphicsData::TriangleList::colors must be a float list or numpy array"); return false; }
							}
							else
							{   //set default color
								colors.SetNumberOfItems(points.NumberOfItems());
								for (auto& color : colors) { color = EXUvis::defaultColorBlue4; }
							}

							bool normalsDefined = true;
							if (gDict.contains("normals"))
							{
								py::object gDictList = gDict["normals"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gDictList)) 
								{
									py::list gList = (py::list)(gDictList);
									std::vector<float> stdGList = py::cast<std::vector<float>>(gList); //! # read out dictionary and cast to C++ type

									if ((stdGList.size() % 3) == 0 && (int)(stdGList.size() / 3) == points.NumberOfItems())
									{
										Index n = (Index)stdGList.size() / 3;
										normals.SetNumberOfItems(n);
										for (Index i = 0; i < n; i++)
										{
											normals[i] = Float3({ stdGList[i * 3],stdGList[i * 3 + 1],stdGList[i * 3 + 2] });
										}
									}
									else { PyError("GraphicsData::TriangleList::normals must be a float list or numpy array with 3*n components (nx,ny,nz), n being the identical to the number of points"); return false; }
								}
								else { PyError("GraphicsData::TriangleList::normals must be a float list or numpy array"); return false; }
							}
							else
							{   //set default normal
								normals.SetNumberOfItems(points.NumberOfItems());
								for (auto& normal : normals) { normal = Float3({ 0,0,0 }); }
								normalsDefined = false;
							}

							if (gDict.contains("triangles"))
							{
								py::object gDictList = gDict["triangles"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gDictList)) 
								{
									py::list gList = (py::list)(gDictList);
									std::vector<Index> stdGList = py::cast<std::vector<Index>>(gList); //! # read out dictionary and cast to C++ type

									if ((stdGList.size() % 3) == 0)
									{
										Index n = (Index)stdGList.size() / 3;
										Index np = points.NumberOfItems();
										GLTriangle trig;
										trig.isFiniteElement = false;
										for (Index i = 0; i < n; i++)
										{
											Index3 pointInd = Index3({ stdGList[i * 3], stdGList[i * 3 + 1], stdGList[i * 3 + 2] });
											for (Index j = 0; j < 3; j++)
											{
												Index ind = pointInd[j];
												if (EXUstd::IndexIsInRange(ind, 0, np))
												{
													trig.points[j] = points[ind];
													trig.normals[j] = normals[ind];
													trig.colors[j] = colors[ind];
												}
												else
												{
													PyError(STDstring("GraphicsData::TriangleList::triangles: point indices need to be in range [0, points.size()-1], but got index: ") + EXUstd::ToString(ind)); return false;
												}
											}
											if (!normalsDefined)
											{
												EXUvis::ComputeTriangleNormals(trig.points, trig.normals);
												//Float3 v1 = trig.points[1] - trig.points[0];
												//Float3 v2 = trig.points[2] - trig.points[0];
												//Float3 n = v1.CrossProduct(v2); //@todo: need to check correct outward normal direction in openGL
												//float len = n.GetL2Norm();
												//if (len != 0.f) { n *= 1.f/len; }
												//trig.normals[0] = n;
												//trig.normals[1] = n;
												//trig.normals[2] = n;
											}
											data.glTriangles.Append(trig);
											//pout << "trig" << i << " = " << trig.points[0] << "," << trig.points[0] << "," << trig.points[0] << "\n";
										}
									}
									else { PyError("GraphicsData::TriangleList::triangles must be a float list or numpy array with 3*n components, n being the number of triangles"); return false; }
								}
								else { PyError("GraphicsData::TriangleList::triangles must be a float list or numpy array"); return false; }
							}
							else
							{
								PyError("GraphicsData::TriangleList must contain 'triangles' being a float list or numpy array with n*(point0,point1,point2)-components, n being the number of triangles; point0, point1, point2 ... point indices of one triangle");
							}

							//++++++++++++++++++++++++++++++++++++++++
							//NEW: add edges:
							Float4 edgeColor({0,0,0,1});
							if (gDict.contains("edgeColor"))
							{
								py::object gColor = gDict["edgeColor"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gColor)) 
								{
									py::list colorList = (py::list)(gColor);
									std::vector<float> stdColorList = py::cast<std::vector<float>>(colorList); //! # read out dictionary and cast to C++ type

									if (stdColorList.size() == 4)
									{
										edgeColor = Float4(stdColorList);
									}
									else { PyError("GraphicsData TriangleList: edgeColor must be a float list or numpy array with 4 components"); return false; }
								}
								else { PyError("GraphicsData TriangleList: edgeColor must be a float list or numpy array with 4 components"); return false; }
							}

							if (gDict.contains("edges"))
							{
								std::vector<int> stdEdgesList;
								Index nEdges = 0;
								py::object gData = gDict["edges"]; //this is necessary to make isinstance work
								if (EPyUtils::IsPyTypeListOrArray(gData)) 
								{
									py::list dataList = (py::list)(gData);
									stdEdgesList = py::cast<std::vector<int>>(dataList); //! # read out dictionary and cast to C++ type

									nEdges = (Index)stdEdgesList.size() / 2;

									if (nEdges * 2 != (Index)stdEdgesList.size())
									{
										PyError("GraphicsData TriangleList: for n edges, points must be an int vector with exactly 2*n components"); return false;
									}
								}
								else { PyError("GraphicsData TriangleList: 'edges' must be a list with 2*n integer values"); return false; }

								GLLine line; line.itemID = -1;
								line.color1 = edgeColor;
								line.color2 = edgeColor;
								for (Index k = 0; k < nEdges; k++)
								{
									line.point1 = points[stdEdgesList[2 * k]];
									line.point2 = points[stdEdgesList[2 * k+1]];
									data.glLines.Append(line);
								}
							}








						} //end triangles +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						else
						{
							PyError(STDstring("GraphicsData type '") + pyTypeStr + "' not supported");
						}
					} //if (py::isinstance<py::str>(pyType))
				} //gDict.contains("type")
				else
				{
					PyError("GraphicsData must contain a 'type'"); return false;
				}
			}
		}//for-loop graphics items
	}
	else 
	{ 
		PyError("GraphicsData must be of type list: [graphicsDict1, graphicsDict2, ...]"); return false; 
	}
	return true;
}

////Note 2024-11-10: (std::vector<float>)v replaced by py::array_t<float>
//template<typename TVector>
//auto FloatVector2NumPy(const TVector& v)
//{
//	return (std::vector<float>)v;
//}

//! uniquely return color, point, etc. information
template<typename TVector>
py::array_t<float> FloatVector2NumPy(const TVector& v)
{
	return py::array_t<float>(v.NumberOfItems(), v.GetDataPointer()); //copy
};

template<typename TVector>
py::array_t<int> IntVector2NumPy(const TVector& v)
{
	return py::array_t<int>(v.NumberOfItems(), v.GetDataPointer()); //copy
};

//! python function to write BodyGraphicsData to dictionary, e.g. for testing; 
py::list PyGetBodyGraphicsDataList(const BodyGraphicsData& data, bool addGraphicsData)
{
	auto list = py::list();

	if (!addGraphicsData)
	{
		auto d = py::dict();
		d["graphicsData"] = std::string("<not requested>");
		list.append(d);
	}
	else
	{
		//CHECKandTHROWstring("PyGetBodyGraphicsDataDictionary: to be implemented");
		for (const auto& item : data.glCirclesXY)
		{
			auto d = py::dict();
			d["type"] = std::string("Circle");
			d["radius"] = item.radius;
			d["color"] = FloatVector2NumPy(item.color);
			d["position"] = FloatVector2NumPy(item.point);
			list.append(d);
		}

		for (const auto& item : data.glTexts)
		{
			auto d = py::dict();
			d["type"] = std::string("Text");
			d["text"] = std::string(item.text);
			d["color"] = FloatVector2NumPy(item.color);
			d["position"] = FloatVector2NumPy(item.point);
			list.append(d);
		}

		Index nLines = data.glLines.NumberOfItems();
		if (nLines != 0)
		{
			auto d = py::dict();
			d["type"] = std::string("Lines");

			ResizableArray<float> colors(nLines * 8);
			ResizableArray<float> points(nLines * 6);

			for (const auto& item : data.glLines)
			{
				for (Index i = 0; i < 4; i++) { colors.Append(item.color1[i]); }
				for (Index i = 0; i < 4; i++) { colors.Append(item.color2[i]); }
				for (Index i = 0; i < 3; i++) { points.Append(item.point1[i]); }
				for (Index i = 0; i < 3; i++) { points.Append(item.point2[i]); }
			}
			d["colors"] = FloatVector2NumPy(colors);
			d["points"] = FloatVector2NumPy(points);
			list.append(d);
		}

		Index nTrigs = data.glTriangles.NumberOfItems();
		if (nTrigs != 0)
		{
			auto d = py::dict();
			d["type"] = std::string("TriangleList");
			ResizableArray<float>  points(nTrigs * 3 * 3);
			ResizableArray<float>  colors(nTrigs * 4 * 3);
			ResizableArray<float> normals(nTrigs * 3 * 3);
			ResizableArray<int> triangles(nTrigs * 3);

			Index cnt = 0; //counts points
			for (const auto& item : data.glTriangles)
			{
				for (Index i = 0; i < 3; i++) { triangles.Append(cnt++); } //local indices of points
				for (Index j = 0; j < 3; j++)
				{
					for (Index i = 0; i < 4; i++) { colors.Append(item.colors[j][i]); }
					for (Index i = 0; i < 3; i++) { points.Append(item.points[j][i]); }
					for (Index i = 0; i < 3; i++) { normals.Append(item.normals[j][i]); }
				}
			}
			d["points"] = FloatVector2NumPy(points);
			d["colors"] = FloatVector2NumPy(colors);
			d["normals"] = FloatVector2NumPy(normals);
			d["triangles"] = IntVector2NumPy(triangles);
			list.append(d);
		}
	}
	return list;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//for BodyGraphicsData lists (KinematicTree)
//! object graphics data for ground objects, rigid bodies and mass points
bool PyWriteBodyGraphicsDataListOfLists(const py::dict& d, const char* item, BodyGraphicsDataList& data)
{
	data.Flush(); //erases all objects
	if (d.contains(item))
	{
		py::object other = d[item]; //this is necessary to make isinstance work
		return PyWriteBodyGraphicsDataListOfLists(other, data);
	}//if "GraphicsDataList" does not exist, no error is displayed
	return true;
}

//! python function to read BodyGraphicsData from py::object, which must be a list of graphicsData dictionaries
bool PyWriteBodyGraphicsDataListOfLists(const py::object object, BodyGraphicsDataList& data)
{

	if (py::isinstance<py::list>(object)) //must be a list of graphicsData dictionaries
	{
		GenericExceptionHandling([&]
		{
			data.Flush(); //erases all objects
			py::list list = (py::list)(object);

			for (auto graphicsItem : list)
			{
				const py::object& pyObject = (const py::object&)graphicsItem;
				BodyGraphicsData oneData;
				Index i = data.Append(oneData);

				PyWriteBodyGraphicsDataList(pyObject, data[i], false);
			}
		}, "Exception raised when writing BodyGraphicsDataList: check format");
	}
	else
	{
		PyError("GraphicsDataList must be of type list: [graphicsData, graphicsData, ...]"); return false;
	}
	return true;
}

//! python function to write BodyGraphicsData to dictionary, e.g. for testing; 
py::list PyGetBodyGraphicsDataListOfLists(const BodyGraphicsDataList& data, bool addGraphicsData)
{
	auto listOfLists = py::list();

	//CHECKandTHROWstring("GetBodyGraphicsDataList: not yet implemented");

	for (auto graphicsData : data)
	{
		auto list = PyGetBodyGraphicsDataList(*graphicsData, addGraphicsData);
	}
	return listOfLists;
}

