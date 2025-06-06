#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# This is an EXUDYN example
# 
# Details:  Mini example for class MarkerSuperElementPosition
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

#set up a mechanical system with two nodes; it has the structure: |~~M0~~M1
#==>further examples see objectGenericODE2Test.py, objectFFRFTest2.py, etc.
nMass0 = mbs.AddNode(NodePoint(referenceCoordinates=[0,0,0]))
nMass1 = mbs.AddNode(NodePoint(referenceCoordinates=[1,0,0]))
mGround = mbs.AddMarker(MarkerBodyPosition(bodyNumber=oGround, localPosition = [1,0,0]))

mass = 0.5 * np.eye(3)      #mass of nodes
stif = 5000 * np.eye(3)     #stiffness of nodes
damp = 50 * np.eye(3)      #damping of nodes
Z = 0. * np.eye(3)          #matrix with zeros
#build mass, stiffness and damping matrices (:
M = np.block([[mass,         0.*np.eye(3)],
              [0.*np.eye(3), mass        ] ])
K = np.block([[2*stif, -stif],
              [ -stif,  stif] ])
D = np.block([[2*damp, -damp],
              [ -damp,  damp] ])

oGenericODE2 = mbs.AddObject(ObjectGenericODE2(nodeNumbers=[nMass0,nMass1], 
                                               massMatrix=M, 
                                               stiffnessMatrix=K,
                                               dampingMatrix=D))

#EXAMPLE for single node marker on super element body, mesh node 1; compare results to ObjectGenericODE2 example!!! 
mSuperElement = mbs.AddMarker(MarkerSuperElementPosition(bodyNumber=oGenericODE2, meshNodeNumbers=[1], weightingFactors=[1]))
mbs.AddLoad(Force(markerNumber = mSuperElement, loadVector = [10, 0, 0])) 

#assemble and solve system for default parameters
mbs.Assemble()

mbs.SolveDynamic(solverType = exudyn.DynamicSolverType.TrapezoidalIndex2)

#check result at default integration time
exudynTestGlobals.testResult = mbs.GetNodeOutput(nMass1, exu.OutputVariableType.Position)[0]

exu.Print("example for MarkerSuperElementPosition completed, test result =", exudynTestGlobals.testResult)

