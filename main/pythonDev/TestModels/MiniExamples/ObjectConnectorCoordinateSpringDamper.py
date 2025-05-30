#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# This is an EXUDYN example
# 
# Details:  Mini example for class ObjectConnectorCoordinateSpringDamper
# 
# Copyright:This file is part of Exudyn. Exudyn is free software. You can redistribute it and/or modify it under the terms of the Exudyn license. See 'LICENSE.txt' for more details.
# 
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

import sys
sys.path.append('../TestModels')
sys.path.append('../../TestModels') #for direct run in directory

import exudyn as exu
from exudyn.utilities import *
import exudyn.graphics as graphics

from modelUnitTests import ExudynTestStructure, exudynTestGlobals
import numpy as np

#create an environment for mini example
SC = exu.SystemContainer()
mbs = SC.AddSystem()

oGround=mbs.AddObject(ObjectGround(referencePosition= [0,0,0]))
nGround = mbs.AddNode(NodePointGround(referenceCoordinates=[0,0,0]))

#define user function:
#NOTE: removed 2023-01-21: dryFriction, dryFrictionProportionalZone
def springForce(mbs, t, itemNumber, u, v, k, d, offset): 
    return 0.1*k*u+k*u**3+v*d

nMass=mbs.AddNode(Point(referenceCoordinates = [2,0,0]))
massPoint = mbs.AddObject(MassPoint(physicsMass = 5, nodeNumber = nMass))

groundMarker=mbs.AddMarker(MarkerNodeCoordinate(nodeNumber= nGround, coordinate = 0))
nodeMarker  =mbs.AddMarker(MarkerNodeCoordinate(nodeNumber= nMass, coordinate = 0))

#Spring-Damper between two marker coordinates
mbs.AddObject(CoordinateSpringDamper(markerNumbers = [groundMarker, nodeMarker], 
                                     stiffness = 5000, damping = 80, 
                                     springForceUserFunction = springForce)) 
loadCoord = mbs.AddLoad(LoadCoordinate(markerNumber = nodeMarker, load = 1)) #static linear solution:0.002

#assemble and solve system for default parameters
mbs.Assemble()
mbs.SolveDynamic()

#check result at default integration time
exudynTestGlobals.testResult = mbs.GetNodeOutput(nMass, 
                                             exu.OutputVariableType.Displacement)[0]

exu.Print("example for ObjectConnectorCoordinateSpringDamper completed, test result =", exudynTestGlobals.testResult)

