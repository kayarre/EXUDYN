
#stub information for class VisuGeneralContact functions
class VisuGeneralContact:
    @overload
    def Reset(self) -> None: ...

#stub information for class GeneralContact functions
class GeneralContact:
    @overload
    def GetPythonObject(self) -> dict: ...
    @overload
    def Reset(self, freeMemory=True) -> None: ...
    isActive:bool
    verboseMode:int
    visualization:VisuGeneralContact
    resetSearchTreeInterval:int
    sphereSphereContact:bool
    sphereSphereFrictionRecycle:bool
    minRelDistanceSpheresTriangles:float
    frictionProportionalZone:float
    frictionVelocityPenalty:float
    excludeOverlappingTrigSphereContacts:bool
    excludeDuplicatedTrigSphereContactPoints:bool
    ancfCableUseExactMethod:bool
    ancfCableNumberOfContactSegments:int
    ancfCableMeasuringSegments:int
    @overload
    def SetFrictionPairings(self, frictionPairings: ArrayLike) -> None: ...
    @overload
    def SetFrictionProportionalZone(self, frictionProportionalZone: float) -> None: ...
    @overload
    def SetSearchTreeCellSize(self, numberOfCells: [int,int,int]) -> None: ...
    @overload
    def SetSearchTreeBox(self, pMin: [float,float,float], pMax: [float,float,float]) -> None: ...
    @overload
    def AddSphereWithMarker(self, markerIndex: MarkerIndex, radius: float, contactStiffness: float, contactDamping: float, frictionMaterialIndex: int) -> int: ...
    @overload
    def AddANCFCable(self, objectIndex: ObjectIndex, halfHeight: float, contactStiffness: float, contactDamping: float, frictionMaterialIndex: int) -> int: ...
    @overload
    def AddTrianglesRigidBodyBased(self, rigidBodyMarkerIndex: MarkerIndex, contactStiffness: float, contactDamping: float, frictionMaterialIndex: int, pointList: List[[float,float,float]], triangleList: List[[int,int,int]]) -> int: ...
    @overload
    def GetItemsInBox(self, pMin: [float,float,float], pMax: [float,float,float]) -> Union[dict,bool]: ...
    @overload
    def GetMarkerBasedSphere(self, localIndex: int) -> dict: ...
    @overload
    def ShortestDistanceAlongLine(self, pStart: [float,float,float], direction: [float,float,float], minDistance: float, maxDistance: float, asDictionary: bool, cylinderRadius: float, typeIndex: ContactTypeIndex) -> Union[dict,float]: ...
    @overload
    def UpdateContacts(self, mainSystem: MainSystem) -> None: ...
    @overload
    def GetActiveContacts(self, typeIndex: ContactTypeIndex, itemIndex: int) -> List[int]: ...

#stub information for class SystemData functions
class SystemData:
    @overload
    def NumberOfLoads(self) -> int: ...
    @overload
    def NumberOfMarkers(self) -> int: ...
    @overload
    def NumberOfNodes(self) -> int: ...
    @overload
    def NumberOfObjects(self) -> int: ...
    @overload
    def NumberOfSensors(self) -> int: ...
    @overload
    def ODE2Size(self, configurationType: ConfigurationType) -> int: ...
    @overload
    def ODE1Size(self, configurationType=ConfigurationType.Current) -> int: ...
    @overload
    def AEsize(self, configurationType=ConfigurationType.Current) -> int: ...
    @overload
    def DataSize(self, configurationType=ConfigurationType.Current) -> int: ...
    @overload
    def SystemSize(self, configurationType=ConfigurationType.Current) -> int: ...
    @overload
    def GetTime(self, configurationType=ConfigurationType.Current) -> float: ...
    @overload
    def SetTime(self, newTime: float, configurationType: ConfigurationType) -> None: ...
    @overload
    def AddODE2LoadDependencies(self, loadNumber: float, globalODE2coordinates: List[int]) -> None: ...
    @overload
    def Info(self) -> None: ...
    @overload
    def InfoLTG(self) -> None: ...
    @overload
    def GetODE2Coordinates(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetODE2Coordinates(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetODE2Coordinates_t(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetODE2Coordinates_t(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetODE2Coordinates_tt(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetODE2Coordinates_tt(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetODE1Coordinates(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetODE1Coordinates(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetODE1Coordinates_t(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetODE1Coordinates_t(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetAECoordinates(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetAECoordinates(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetDataCoordinates(self, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def SetDataCoordinates(self, coordinates: List[float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetSystemState(self, configuration: ConfigurationType) -> List[List[float]]: ...
    @overload
    def SetSystemState(self, systemStateList: List[List[float]], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetObjectLTGODE2(self, objectNumber: int) -> List[int]: ...
    @overload
    def GetObjectLTGODE1(self, objectNumber: int) -> List[int]: ...
    @overload
    def GetObjectLTGAE(self, objectNumber: int) -> List[int]: ...
    @overload
    def GetObjectLTGData(self, objectNumber: int) -> List[int]: ...
    @overload
    def GetNodeLTGODE2(self, nodeNumber: int) -> List[int]: ...
    @overload
    def GetNodeLTGODE1(self, nodeNumber: int) -> List[int]: ...
    @overload
    def GetNodeLTGAE(self, nodeNumber: int) -> List[int]: ...
    @overload
    def GetNodeLTGData(self, nodeNumber: int) -> List[int]: ...

#stub information for class MainSystem functions
class MainSystem:
    @overload
    def Assemble(self) -> None: ...
    @overload
    def AssembleCoordinates(self) -> None: ...
    @overload
    def AssembleLTGLists(self) -> None: ...
    @overload
    def AssembleInitializeSystemCoordinates(self) -> None: ...
    @overload
    def AssembleSystemInitialize(self) -> None: ...
    @overload
    def Reset(self) -> None: ...
    @overload
    def GetSystemContainer(self) -> MainSystem: ...
    @overload
    def WaitForUserToContinue(self, printMessage=True) -> None: ...
    @overload
    def SendRedrawSignal(self) -> None: ...
    @overload
    def GetRenderEngineStopFlag(self) -> bool: ...
    @overload
    def SetRenderEngineStopFlag(self, stopFlag: bool) -> None: ...
    @overload
    def ActivateRendering(self, flag=True) -> None: ...
    @overload
    def SetPreStepUserFunction(self, value: Callable[[MainSystem, float],bool]) -> None: ...
    @overload
    def SetPostNewtonUserFunction(self, value: Callable[[MainSystem, float],[float,float]]) -> None: ...
    @overload
    def AddGeneralContact(self) -> GeneralContact: ...
    @overload
    def GetGeneralContact(self, generalContactNumber: int) -> GeneralContact: ...
    @overload
    def DeleteGeneralContact(self, generalContactNumber: int) -> None: ...
    @overload
    def NumberOfGeneralContacts(self) -> int: ...
    systemIsConsistent:bool
    interactiveMode:bool
    variables:dict
    sys:dict
    solverSignalJacobianUpdate:bool
    systemData:SystemData
    @overload
    def AddNode(self, pyObject: dict) -> NodeIndex: ...
    @overload
    def GetNodeNumber(self, nodeName: str) -> NodeIndex: ...
    @overload
    def GetNode(self, nodeNumber: NodeIndex) -> dict: ...
    @overload
    def ModifyNode(self, nodeNumber: NodeIndex, nodeDict: dict) -> None: ...
    @overload
    def GetNodeDefaults(self, typeName: str) -> dict: ...
    @overload
    def GetNodeOutput(self, nodeNumber: NodeIndex, variableType: OutputVariableType, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetNodeODE2Index(self, nodeNumber: NodeIndex) -> int: ...
    @overload
    def GetNodeODE1Index(self, nodeNumber: NodeIndex) -> int: ...
    @overload
    def GetNodeAEIndex(self, nodeNumber: NodeIndex) -> int: ...
    @overload
    def GetNodeParameter(self, nodeNumber: NodeIndex, parameterName: str) -> Any: ...
    @overload
    def SetNodeParameter(self, nodeNumber: NodeIndex, parameterName: str, value: Any) -> None: ...
    @overload
    def AddObject(self, pyObject: dict) -> ObjectIndex: ...
    @overload
    def GetObjectNumber(self, objectName: str) -> ObjectIndex: ...
    @overload
    def GetObject(self, objectNumber: ObjectIndex, addGraphicsData=False) -> dict: ...
    @overload
    def ModifyObject(self, objectNumber: ObjectIndex, objectDict: dict) -> None: ...
    @overload
    def GetObjectDefaults(self, typeName: str) -> dict: ...
    @overload
    def GetObjectOutput(self, objectNumber: ObjectIndex, variableType: OutputVariableType, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetObjectOutputBody(self, objectNumber: ObjectIndex, variableType: OutputVariableType, localPosition: [float,float,float], configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetObjectOutputSuperElement(self, objectNumber: ObjectIndex, variableType: OutputVariableType, meshNodeNumber: int, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetObjectParameter(self, objectNumber: ObjectIndex, parameterName: str) -> Any: ...
    @overload
    def SetObjectParameter(self, objectNumber: ObjectIndex, parameterName: str, value: Any) -> None: ...
    @overload
    def AddMarker(self, pyObject: dict) -> MarkerIndex: ...
    @overload
    def GetMarkerNumber(self, markerName: str) -> MarkerIndex: ...
    @overload
    def GetMarker(self, markerNumber: MarkerIndex) -> dict: ...
    @overload
    def ModifyMarker(self, markerNumber: MarkerIndex, markerDict: dict) -> None: ...
    @overload
    def GetMarkerDefaults(self, typeName: str) -> dict: ...
    @overload
    def GetMarkerParameter(self, markerNumber: MarkerIndex, parameterName: str) -> Any: ...
    @overload
    def SetMarkerParameter(self, markerNumber: MarkerIndex, parameterName: str, value: Any) -> None: ...
    @overload
    def GetMarkerOutput(self, markerNumber: MarkerIndex, variableType: OutputVariableType, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def AddLoad(self, pyObject: dict) -> LoadIndex: ...
    @overload
    def GetLoadNumber(self, loadName: str) -> LoadIndex: ...
    @overload
    def GetLoad(self, loadNumber: LoadIndex) -> dict: ...
    @overload
    def ModifyLoad(self, loadNumber: LoadIndex, loadDict: dict) -> None: ...
    @overload
    def GetLoadDefaults(self, typeName: str) -> dict: ...
    @overload
    def GetLoadValues(self, loadNumber: LoadIndex) -> List[float]: ...
    @overload
    def GetLoadParameter(self, loadNumber: LoadIndex, parameterName: str) -> Any: ...
    @overload
    def SetLoadParameter(self, loadNumber: LoadIndex, parameterName: str, value: Any) -> None: ...
    @overload
    def AddSensor(self, pyObject: dict) -> SensorIndex: ...
    @overload
    def GetSensorNumber(self, sensorName: str) -> SensorIndex: ...
    @overload
    def GetSensor(self, sensorNumber: SensorIndex) -> dict: ...
    @overload
    def ModifySensor(self, sensorNumber: SensorIndex, sensorDict: dict) -> None: ...
    @overload
    def GetSensorDefaults(self, typeName: str) -> dict: ...
    @overload
    def GetSensorValues(self, sensorNumber: SensorIndex, configuration: ConfigurationType) -> List[float]: ...
    @overload
    def GetSensorStoredData(self, sensorNumber: SensorIndex) -> ArrayLike: ...
    @overload
    def GetSensorParameter(self, sensorNumber: SensorIndex, parameterName: str) -> Any: ...
    @overload
    def SetSensorParameter(self, sensorNumber: SensorIndex, parameterName: str, value: Any) -> None: ...

#stub information for class SystemContainer functions
class SystemContainer:
    @overload
    def Reset(self) -> None: ...
    @overload
    def AddSystem(self) -> MainSystem: ...
    @overload
    def NumberOfSystems(self) -> int: ...
    @overload
    def GetSystem(self, systemNumber: int) -> MainSystem: ...
    visualizationSettings:VisualizationSettings
    @overload
    def GetRenderState(self) -> dict: ...
    @overload
    def SetRenderState(self, renderState: dict) -> None: ...
    @overload
    def RedrawAndSaveImage(self) -> None: ...
    @overload
    def WaitForRenderEngineStopFlag(self) -> bool: ...
    @overload
    def RenderEngineZoomAll(self) -> None: ...
    @overload
    def AttachToRenderEngine(self) -> bool: ...
    @overload
    def DetachFromRenderEngine(self) -> bool: ...
    @overload
    def SendRedrawSignal(self) -> None: ...
    @overload
    def GetCurrentMouseCoordinates(self, useOpenGLcoordinates=False) -> [float,float]: ...

#stub information for exudyn module functions
@overload
def GetVersionString(addDetails=False) -> str: ...
@overload
def Help() -> None: ...
@overload
def RequireVersion(requiredVersionString: str) -> None: ...
@overload
def StartRenderer(verbose=0) -> bool: ...
@overload
def IsRendererActive() -> bool: ...
@overload
def DoRendererIdleTasks(waitSeconds=0) -> None: ...
@overload
def SolveStatic(mbs: MainSystem, simulationSettings: SimulationSettings, updateInitialValues=False, storeSolver=True) -> bool: ...
@overload
def SolveDynamic(mbs: MainSystem, simulationSettings: SimulationSettings, solverType: DynamicSolverType, updateInitialValues=False, storeSolver=True) -> bool: ...
@overload
def ComputeODE2Eigenvalues(mbs: MainSystem, simulationSettings: SimulationSettings, useSparseSolver=False, numberOfEigenvalues=-1, setInitialValues=True, convert2Frequencies=False) -> bool: ...
@overload
def SetOutputPrecision(numberOfDigits: int) -> None: ...
@overload
def SetLinalgOutputFormatPython(flagPythonFormat: bool) -> None: ...
@overload
def SetWriteToConsole(flag: bool) -> None: ...
@overload
def SetWriteToFile(filename: str, flagWriteToFile=True, flagAppend=False) -> None: ...
@overload
def SetPrintDelayMilliSeconds(delayMilliSeconds: int) -> None: ...
@overload
def Print(*args: Any) -> None: ...
@overload
def SuppressWarnings(flag: bool) -> None: ...
@overload
def InfoStat(writeOutput=True) -> List[int]: ...
@overload
def Go() -> None: ...
@overload
def InvalidIndex() -> int: ...
variables:dict
sys:dict
