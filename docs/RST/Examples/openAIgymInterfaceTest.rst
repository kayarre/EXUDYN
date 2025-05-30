
.. _examples-openaigyminterfacetest:

*************************
openAIgymInterfaceTest.py
*************************

You can view and download this file on Github: `openAIgymInterfaceTest.py <https://github.com/jgerstmayr/EXUDYN/tree/master/main/pythonDev/Examples/openAIgymInterfaceTest.py>`_

.. code-block:: python
   :linenos:

   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   # This is an EXUDYN example
   #
   # Details:  This file shows integration with OpenAI gym by testing a double pendulum example
   #
   # Author:   Johannes Gerstmayr
   # Date:     2022-05-18
   #
   # Copyright:This file is part of Exudyn. Exudyn is free software. You can redistribute it and/or modify it under the terms of the Exudyn license. See 'LICENSE.txt' for more details.
   #
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   #to run this example the following worked:
   #conda create -n venvGym python=3.10 numpy matplotlib spyder-kernels=2.4 ipykernel -y
   #pip install gym[spaces]
   #pip install stable-baselines3==1.7.0
   #pip install exudyn
   
   import exudyn as exu
   from exudyn.utilities import * #includes itemInterface and rigidBodyUtilities
   import exudyn.graphics as graphics #only import if it does not conflict
   from exudyn.artificialIntelligence import *
   import math
   
   #%%++++++++++++++++++++++++++++++++++++++++++++++
   
   #**class: interface class to set up Exudyn model which can be used as model in open AI gym;
   #         see specific class functions which contain 'OVERRIDE' to integrate your model;
   #         in general, set up a model with CreateMBS(), map state to initial values, initial values to state and action to mbs;
   class InvertedDoublePendulumEnv(OpenAIGymInterfaceEnv):
           
       #**classFunction: OVERRIDE this function to create multibody system mbs and setup simulationSettings; call Assemble() at the end!
       #                 you may also change SC.visualizationSettings() individually; kwargs may be used for special setup
       def CreateMBS(self, SC, mbs, simulationSettings, **kwargs):
   
           #this model uses kwargs: thresholdFactor
           thresholdFactor = kwargs['thresholdFactor']
           gravity = 9.81
           self.length = 1.
           width = 0.1*self.length
           masscart = 1.
           massarm = 0.1
           total_mass = massarm + masscart
           armInertia = self.length**2*0.5*massarm
           self.force_mag = 10.0
           self.stepUpdateTime = 0.02  # seconds between state updates
           
           background = graphics.CheckerBoard(point= [0,0.5*self.length,-0.5*width], 
                                                 normal= [0,0,1], size=10, size2=6, nTiles=20, nTiles2=12)
               
           oGround=self.mbs.AddObject(ObjectGround(referencePosition= [0,0,0],  #x-pos,y-pos,angle
                                              visualization=VObjectGround(graphicsData= [background])))
           nGround=self.mbs.AddNode(NodePointGround())
           
           gCart = graphics.Brick(size=[0.5*self.length, width, width], 
                                              color=graphics.color.dodgerblue)
           self.nCart = self.mbs.AddNode(Rigid2D(referenceCoordinates=[0,0,0]));
           oCart = self.mbs.AddObject(RigidBody2D(physicsMass=masscart, 
                                             physicsInertia=0.1*masscart, #not needed
                                             nodeNumber=self.nCart,
                                             visualization=VObjectRigidBody2D(graphicsData= [gCart])))
           mCartCOM = self.mbs.AddMarker(MarkerNodePosition(nodeNumber=self.nCart))
           
           gArm1 = graphics.Brick(size=[width, self.length, width], color=graphics.color.red)
           gArm1joint = graphics.Cylinder(pAxis=[0,-0.5*self.length,-0.6*width], vAxis=[0,0,1.2*width], 
                                             radius=0.0625*self.length, color=graphics.color.darkgrey)
           self.nArm1 = self.mbs.AddNode(Rigid2D(referenceCoordinates=[0,0.5*self.length,0]));
           oArm1 = self.mbs.AddObject(RigidBody2D(physicsMass=massarm, 
                                             physicsInertia=armInertia, #not included in original paper
                                             nodeNumber=self.nArm1,
                                             visualization=VObjectRigidBody2D(graphicsData= [gArm1, gArm1joint])))
           
           mArm1COM = self.mbs.AddMarker(MarkerNodePosition(nodeNumber=self.nArm1))
           mArm1JointA = self.mbs.AddMarker(MarkerBodyPosition(bodyNumber=oArm1, localPosition=[0,-0.5*self.length,0]))
           mArm1JointB = self.mbs.AddMarker(MarkerBodyPosition(bodyNumber=oArm1, localPosition=[0, 0.5*self.length,0]))
   
           gArm2 = graphics.Brick(size=[width, self.length, width], color=graphics.color.red)
           self.nArm2 = self.mbs.AddNode(Rigid2D(referenceCoordinates=[0,1.5*self.length,0]));
           oArm2 = self.mbs.AddObject(RigidBody2D(physicsMass=massarm, 
                                             physicsInertia=armInertia, #not included in original paper
                                             nodeNumber=self.nArm2,
                                             visualization=VObjectRigidBody2D(graphicsData= [gArm2, gArm1joint])))
           
           mArm2COM = self.mbs.AddMarker(MarkerNodePosition(nodeNumber=self.nArm2))
           mArm2Joint = self.mbs.AddMarker(MarkerBodyPosition(bodyNumber=oArm2, localPosition=[0,-0.5*self.length,0]))
           
           mCartCoordX = self.mbs.AddMarker(MarkerNodeCoordinate(nodeNumber=self.nCart, coordinate=0))
           mCartCoordY = self.mbs.AddMarker(MarkerNodeCoordinate(nodeNumber=self.nCart, coordinate=1))
           mGroundNode = self.mbs.AddMarker(MarkerNodeCoordinate(nodeNumber=nGround, coordinate=0))
           
           #gravity
           self.mbs.AddLoad(Force(markerNumber=mCartCOM, loadVector=[0,-masscart*gravity,0]))
           self.mbs.AddLoad(Force(markerNumber=mArm1COM, loadVector=[0,-massarm*gravity,0]))
           self.mbs.AddLoad(Force(markerNumber=mArm2COM, loadVector=[0,-massarm*gravity,0]))
           
           #control force
           self.lControl = self.mbs.AddLoad(LoadCoordinate(markerNumber=mCartCoordX, load=1.))
           
           #joints and constraints:
           self.mbs.AddObject(RevoluteJoint2D(markerNumbers=[mCartCOM, mArm1JointA]))
           self.mbs.AddObject(RevoluteJoint2D(markerNumbers=[mArm1JointB, mArm2Joint]))
           self.mbs.AddObject(CoordinateConstraint(markerNumbers=[mCartCoordY, mGroundNode]))
           
           
           
           
           #%%++++++++++++++++++++++++
           self.mbs.Assemble() #computes initial vector
           
           self.simulationSettings.timeIntegration.numberOfSteps = 1
           self.simulationSettings.timeIntegration.endTime = 0 #will be overwritten in step
           self.simulationSettings.timeIntegration.verboseMode = 0
           self.simulationSettings.solutionSettings.writeSolutionToFile = False
           #self.simulationSettings.timeIntegration.simulateInRealtime = True
           
           self.simulationSettings.timeIntegration.newton.useModifiedNewton = True
           
           self.SC.visualizationSettings.general.drawWorldBasis=True
           self.SC.visualizationSettings.general.graphicsUpdateInterval = 0.01 #50Hz
           self.SC.visualizationSettings.openGL.multiSampling=4
           
           #self.simulationSettings.solutionSettings.solutionInformation = "Open AI gym"
           
           #+++++++++++++++++++++++++++++++++++++++++++++++++++++
           # Angle at which to fail the episode
           # these parameters are used in subfunctions
           self.theta_threshold_radians = thresholdFactor* 12 * 2 * math.pi / 360
           self.x_threshold = thresholdFactor*2.4
           
           #must return state size
           stateSize = 6 #the number of states (position/velocity that are used by learning algorithm)
           return stateSize
   
       #**classFunction: OVERRIDE this function to set up self.action_space and self.observation_space
       def SetupSpaces(self):
   
           high = np.array(
               [
                   self.x_threshold * 2,
                   np.finfo(np.float32).max,
                   self.theta_threshold_radians * 2,
                   np.finfo(np.float32).max,
                   self.theta_threshold_radians * 2,
                   np.finfo(np.float32).max,
               ],
               dtype=np.float32,
           )
   
           #+++++++++++++++++++++++++++++++++++++++++++++++++++++
           #see https://github.com/openai/gym/blob/64b4b31d8245f6972b3d37270faf69b74908a67d/gym/core.py#L16
           #for Env:
           self.action_space = spaces.Discrete(2)
           self.observation_space = spaces.Box(-high, high, dtype=np.float32)        
           #+++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   
       #**classFunction: OVERRIDE this function to map the action given by learning algorithm to the multibody system, e.g. as a load parameter
       def MapAction2MBS(self, action):
           force = self.force_mag if action == 1 else -self.force_mag        
           self.mbs.SetLoadParameter(self.lControl, 'load', force)
   
       #**classFunction: OVERRIDE this function to collect output of simulation and map to self.state tuple
       #**output: return bool done which contains information if system state is outside valid range
       def Output2StateAndDone(self):
           
           #+++++++++++++++++++++++++
           #compute some output:
           cartPosX = self.mbs.GetNodeOutput(self.nCart, variableType=exu.OutputVariableType.Coordinates)[0]
           arm1Angle = self.mbs.GetNodeOutput(self.nArm1, variableType=exu.OutputVariableType.Coordinates)[2]
           arm2Angle = self.mbs.GetNodeOutput(self.nArm2, variableType=exu.OutputVariableType.Coordinates)[2]
           cartPosX_t = self.mbs.GetNodeOutput(self.nCart, variableType=exu.OutputVariableType.Coordinates_t)[0]
           arm1Angle_t = self.mbs.GetNodeOutput(self.nArm1, variableType=exu.OutputVariableType.Coordinates_t)[2]
           arm2Angle_t = self.mbs.GetNodeOutput(self.nArm2, variableType=exu.OutputVariableType.Coordinates_t)[2]
   
           #finally write updated state:
           self.state = (cartPosX, cartPosX_t, arm1Angle, arm1Angle_t, arm2Angle, arm2Angle_t)
           #++++++++++++++++++++++++++++++++++++++++++++++++++
   
           done = bool(
               cartPosX < -self.x_threshold
               or cartPosX > self.x_threshold
               or arm1Angle < -self.theta_threshold_radians
               or arm1Angle > self.theta_threshold_radians
               or arm2Angle < -self.theta_threshold_radians
               or arm2Angle > self.theta_threshold_radians
           )
           return done
   
       
       #**classFunction: OVERRIDE this function to maps the current state to mbs initial values
       #**output: return [initialValues, initialValues\_t] where initialValues[\_t] are ODE2 vectors of coordinates[\_t] for the mbs
       def State2InitialValues(self):
           #+++++++++++++++++++++++++++++++++++++++++++++
           #set specific initial state:
           (xCart, xCart_t, phiArm1, phiArm1_t, phiArm2, phiArm2_t) = self.state
           
           initialValues = np.zeros(9) #model has 3*3 = 9  redundant states
           initialValues_t = np.zeros(9)
           
           #build redundant cordinates from self.state
           initialValues[0] = xCart
           initialValues[3+0] = xCart - 0.5*self.length * sin(phiArm1)
           initialValues[3+1] = 0.5*self.length * (cos(phiArm1)-1)
           initialValues[3+2] = phiArm1
           initialValues[6+0] = xCart - 1.*self.length * sin(phiArm1) - 0.5*self.length * sin(phiArm2)
           initialValues[6+1] = self.length * cos(phiArm1) + 0.5*self.length * cos(phiArm2) - 1.5*self.length
           initialValues[6+2] = phiArm2
           
           initialValues_t[0] = xCart_t
           initialValues_t[3+0] = xCart_t - phiArm1_t*0.5*self.length * cos(phiArm1)
           initialValues_t[3+1] = -0.5*self.length * sin(phiArm1)  * phiArm1_t
           initialValues_t[3+2] = phiArm1_t
           initialValues_t[6+0] = xCart_t - phiArm1_t*1.*self.length * cos(phiArm1) - phiArm2_t*0.5*self.length * cos(phiArm2)
           initialValues_t[6+1] = -self.length * sin(phiArm1)  * phiArm1_t - 0.5*self.length * sin(phiArm2)  * phiArm2_t
           initialValues_t[6+2] = phiArm2_t
           
           return [initialValues,initialValues_t]
           
   
   
   
   
   
   
   
   
   #%%+++++++++++++++++++++++++++++++++++++++++++++
   if __name__ == '__main__': #this is only executed when file is direct called in Python
       import time
       
   
       #%%++++++++++++++++++++++++++++++++++++++++++++++++++
       #create model and do reinforcement learning
       env = InvertedDoublePendulumEnv(thresholdFactor=1)
       
       #check if model runs:
       #env.TestModel(numberOfSteps=1000, seed=42)
   
       #use some learning algorithm:
       from stable_baselines3 import A2C
   
       #main learning task; 1e7 steps take 2-3 hours
       ts = -time.time()
       model = A2C('MlpPolicy', env, verbose=1)
       model.learn(total_timesteps=1e4) #1e6 may work, 2e7 is quite robust!
       print('*** learning time total =',ts+time.time(),'***')
   
       #save learned model
       model.save("openAIgymDoublePendulum1e4")
       del model
   
       #%%++++++++++++++++++++++++++++++++++++++++++++++++++
       #only load and test
       model = A2C.load("openAIgymDoublePendulum1e5")
       env = InvertedDoublePendulumEnv(thresholdFactor=15) #larger threshold for testing
       solutionFile='solution/learningCoordinates.txt'
       env.TestModel(numberOfSteps=2500, model=model, solutionFileName=solutionFile, 
                     stopIfDone=False, useRenderer=True, sleepTime=0) #just compute solution file
   
       #++++++++++++++++++++++++++++++++++++++++++++++
       #visualize (and make animations) in exudyn:
       from exudyn.interactive import SolutionViewer
       env.SC.visualizationSettings.general.autoFitScene = False
       solution = LoadSolutionFile(solutionFile)
       env.mbs.SolutionViewer(solution) #loads solution file via name stored in mbs
   


