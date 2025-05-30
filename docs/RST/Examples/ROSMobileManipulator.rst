
.. _examples-rosmobilemanipulator:

***********************
ROSMobileManipulator.py
***********************

You can view and download this file on Github: `ROSMobileManipulator.py <https://github.com/jgerstmayr/EXUDYN/tree/master/main/pythonDev/Examples/ROSMobileManipulator.py>`_

.. code-block:: python
   :linenos:

   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   # This is an EXUDYN python example
   #
   # Details:  This example shows how to communicate between an Exudyn simulation and ROS
   #           To make use of this example, you need to 
   #           install ROS (ROS1 noetic) including rospy (see rosInterface.py)
   #           prerequisite to use: 
   #           use a bash terminal to start the roscore with: 
   #               roscore 
   #           then run the simulation:
   #               python 3 ROSMobileManipulator.py
   #           You can use the prepared ROS node, ROSControlMobileManipulator to control the simulation
   #           use a bash terminal to start the recommended file  (see folder Examples/supplementary):
   #               python3 ROSControlMobileManipulator.py
   #           For even more ROS functionality create a ROS package (e.q. myExudynInterface) in a catkin workspace, 
   #           copy files ROSMobileManipulator.py, ROSbodykairos.stl and ROSControlMobileManipulator.py in corresponding folders within the package
   #           For more functionality see also: ROSMassPoint.py, ROSBringupTurtle.launch, ROSControlTurtleVelocity.py from the EXUDYN examples folder
   # 
   # Author:   Martin Sereinig, Peter Manzl 
   # Date:     2023-05-31 (created)
   # last Update: 2023-09-11
   # Copyright:This file is part of Exudyn. Exudyn is free software. 
   # You can redistribute it and/or modify it under the terms of the Exudyn license. 
   # See 'LICENSE.txt' for more details.
   #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   # general imports
   import numpy as np
   import roboticstoolbox as rtb
   from spatialmath import SE3
   
   # exudyn imports
   import exudyn as exu 
   from exudyn.utilities import * #includes itemInterface and rigidBodyUtilities
   import exudyn.graphics as graphics #only import if it does not conflict
   from exudyn.itemInterface import *
   from exudyn.rigidBodyUtilities import *
   from exudyn.graphicsDataUtilities import *
   from exudyn.robotics import *
   from exudyn.robotics.models import ManipulatorUR5, LinkDict2Robot
   from exudyn.robotics.motion import Trajectory, ProfileConstantAcceleration, ProfilePTP
   import exudyn.robotics.rosInterface as exuROS #exudyn ROS interface class
   import exudyn.robotics.mobile as mobile
   
   # ROS imports
   from geometry_msgs.msg import Twist
   from geometry_msgs.msg import Pose
   from std_msgs.msg import String
   
   
   # here build inherited class and using within a simple exudyn simulation of one mass spring-damper 
   class MyExudynROSInterface(exuROS.ROSInterface):
       def __init__(self):
           # use initialisation of super class
           # this initialize a rosnode with name
           super().__init__(name='ROSMobileManipulator')
           # initialization of all standard publisher done by super class
           # use standard publisher functions form super class
           # initialize all subscriber 
           # suitable callback function is auto generated by superclass (using lambda function)
           # twist subscriber: cmd_vel
           twistSubsrciberBase = '' 
           twistSubsrciberTopic = 'cmd_vel'     # topic to subscribe 
           self.cmd_vel = Twist()              # data type of topic, must be named: self.twistSubscriberTopic
           self.myTwistSubscriber = self.InitSubscriber(twistSubsrciberBase,twistSubsrciberTopic,Twist)
           # string subsriber: my_string
           stringSubsrciberBase = ''
           stringSubsrciberTopic = 'my_string'     # topic to subscribe 
           self.my_string = String()              # data type of topic, must be named: self.twistSubscriberTopic
           self.myStringSubscriber = self.InitSubscriber(stringSubsrciberBase,stringSubsrciberTopic,String)
           # string subsriber: my_pose
           poseSubsrciberBase = ''
           poseSubsrciberTopic = 'my_pose'     # topic to subscribe 
           self.my_pose = Pose()              # data type of topic, must be named: self.twistSubscriberTopic
           self.myPoseSubscriber = self.InitSubscriber(poseSubsrciberBase,poseSubsrciberTopic,Pose)        
   
   
   debugFlag = False  # turn prints on and off
   
   # function to check (exudyn) module version 
   def checkInstall(moduleName): 
       import importlib
       found = importlib.util.find_spec(moduleName) is not None
       if not(found): 
           print('Error! Please install the module {} Version 1.7 or higher using \npip install {}==1.7'.format(moduleName, moduleName))
           return False
       else: 
           return True
   
   # function to control the mobile manipulator behavior 
   def functionStateMachine(t, posPlatform, ThetaPlatform, PosObj, armStatus, myState, myDict):
       # initialize variables
       v = [0.0,0.0,0]
       # with None no arm movement is performed
       TArm = None
       grasp = None
       # check robotC control string send via ROS
       robotControlString = myROSInterface.my_string.data
       if debugFlag:
           if robotControlString!='':
               print('robot control string: ', robotControlString)
           else:
               print('no robot control string')
       if robotControlString == 'ms':
           TArm = None
           grasp = None
           # get velocity data from ROS /cmd_vel topic, please use: rostopic pub -r 100 /cmd_vel geometry_msgs/Twist "..."
           rosLinearVelo = myROSInterface.cmd_vel.linear
           rosAngularVelo = myROSInterface.cmd_vel.angular 
           v = [rosLinearVelo.x, rosLinearVelo.y, rosAngularVelo.z]
       elif robotControlString == 'a':
           # state for arm movement 
           v = [0.0,0.0,0]
           ArmPosition = [myROSInterface.my_pose.position.x, myROSInterface.my_pose.position.y, myROSInterface.my_pose.position.z]
           ArmOrientationQ = [myROSInterface.my_pose.orientation.w, myROSInterface.my_pose.orientation.x, myROSInterface.my_pose.orientation.y, myROSInterface.my_pose.orientation.z]
           ArmOrientationR = EulerParameters2RotationMatrix(ArmOrientationQ)
           # build homogenous transformation from rotation matrix existing rotation matrix
           TArmRot = np.eye(4)
           TArmRot[0:3,0:3] = ArmOrientationR
           TArm = HTtranslate(ArmPosition) @ TArmRot
       elif robotControlString == 'mk':
           # state for external cmd_vel (keyboard or other node)
           # get velocity data from ROS /cmd_vel topic, please use: rostopic pub -r 100 /cmd_vel geometry_msgs/Twist "..." but send via keyboard node
           rosLinearVelo = myROSInterface.cmd_vel.linear
           rosAngularVelo = myROSInterface.cmd_vel.angular 
           v = [rosLinearVelo.x, rosLinearVelo.y, rosAngularVelo.z]
       else:
           # print('no valid control string received')
           v = [0.0, 0.0, 0.0]
           TArm = None
           grasp = None
       return v, TArm, grasp, myState, myDict
   
   # set simulation system
   SC = exu.SystemContainer()
   mbs = SC.AddSystem()
   # function to simulate the mobile manipulator 
   def SimulationMobileRobot(funcStatMachine,myROSInterface, p0=[0,0], theta0=0, flagFixObject=False, flagOnlyGrasp=False, verboseMode = 0, sensorWriteToFile = False): 
       compensateStaticTorques = False
       mobRobSolutionPath = 'solution/'
       hstepsize = 5e-3 # step size 
       tEnd = 100 # simulation time 
       comShift=[0,0,-0.1]
       debugPlatformOffset = 125*9.81 /10**(6)
       constrainedAxesSet=[0,0,0,0,0,0]
       offsetUserFunctionParametersSet=[0,0,0,0,0,0]
       debugOffsetNumber= debugPlatformOffset
       
       #ground body and marker
       gGround = graphics.CheckerBoard(point = [0,0,0], size=8, nTiles = 12) # (centerPoint=[4,4,-0.001],size=[12,12,0.002], color=graphics.color.lightgrey[0:3]+[0.5])
       graphicsGroundList =[gGround]
       coordinateSystemGround = False
       if coordinateSystemGround:
           graphicsGroundList += [graphics.Cylinder([0,0,0], [0.5,0,0], 0.0035, graphics.color.red)]            # base coordinate system x
           graphicsGroundList +=[graphics.Cylinder([0,0,0], [0,0.5,0], 0.0035, graphics.color.green)]          # base coordinate system y
           graphicsGroundList +=[graphics.Cylinder([0,0,0], [0,0,0.5], 0.0035, graphics.color.blue)]           # base coordinate system z
       oGround = mbs.AddObject(ObjectGround(visualization=VObjectGround(graphicsData=graphicsGroundList)))
       markerGround = mbs.AddMarker(MarkerBodyRigid(bodyNumber=oGround, localPosition=[0,0,0]))
       comShiftPlatform = [0,0,0]
       
       # define mobile manipulator KAIROS     
       mobileRobot = { 'gravity':                  [0,0,-9.81],                # gravity in m/s^2
                       'platformDimensions':       [0.575, 0.718 , 0.2495],    # [width, length, hight]   [0.575, 0.718 , 0.495]
                       'platformMass':             125-18.4,                   # platform mass- manipulator mass 
                       'platformInitialPose':      HTtranslate([p0[0],p0[1],(0.495+(-0.12))]) @ HTrotateZ(theta0),  # platform initial pose as HT middle of platform (box representation) 
                       'platformInitialOmega':     [0,0,0],                # platform initial rotational velocity around x,y,z axis
                       'platformInitialVelocity':  [0,0,0],                # platform initial translational velocity in x,y,z direction
                       'platformCOM':              comShift,               # center of mass shift to base coordinate system
                       'comShiftPlatform':         comShiftPlatform,       # the shift of the platform alone
                       'platformBaseCoordinate':   [0.0 ,0.0 ,0.0],        # geometric center  in middle of platform                   
                       'platformInertia':          InertiaCuboid,          # platform inertia w.r.t. COM!                    
                       'platformRepresentation':   'box',                  # 'box' or 'stl' graphical representation of the mobile platform 
                       'platformStlFile':          'ROSbodykairos.stl',    # path to the used stl file 
                       'friction':                 [1 ,0.0, 0.0],          # [dryFriction1, dryFriction2,rollFriction]= [0.4,0.0075,0.05] for LeoBot (Master Thesis Manzl)
                       'viscousFrictionWheel':     [0.1, 0.1],             # orthotropic damping in the rotated roller frame; see also Exudyn documentation of 
                       'frictionAngle':            np.pi/4 ,               # friction angle theta=pi/4 for mecanum wheel, theta=0 for standard wheel  
                       'wheelType':                0,                      # 0=wheelType wheel o-config, 1=mecanum wheel x-config, 2=standard wheel  (always in bottom view)
                       'wheelBase':                0.430,                  # distance between center of wheels (wheel axes) between front and back  
                       'wheelTrack':               0.390,                  # distance between center of wheels between left and right 
                       'wheelRoh':                 200*8,                  # density of wheel in kg/m^3
                       'wheelRadius':              0.254/2,                # radius of wheel in m 
                       'wheelWidth':               0.1,                    # width of wheel in m, just for graphics     
                       'wheelMass':                8,                      # Mass of one mecanum wheel, leobot measured
                       'wheelInertia':             InertiaCylinder,        # inertia for infinitely small ring:
                       'wheelNumbers':             4,                      # number of wheels on platform
                       'wheelContactStiffness':    10**(6), 
                       'wheelContactDamping':      50*np.sqrt(10**(5)), 
                       'serialRobotMountpoint':    HTtranslate([0.178 , 0, 0.12]), 
                       'proportionalZone':         1e-2,                   # friction regularization
                       'debugOffset':              debugOffsetNumber
                       }  
           
       #################### Build mobile robot and add it to existing mbs
       mobileRobotBackDic = mobile.MobileRobot2MBS(mbs, mobileRobot, markerGround)
       mbs.variables['mobileRobotBackDic'] = mobileRobotBackDic # to be able to use all variables in all functions (make it global useable)
       # add mbs.variable for ROS sensor
       mbs.variables['nodeNumber'] = mobileRobotBackDic['nPlatformList'][0] # just needed if nodeNumber is used for sensor information 
       # for shorter writing: 
       Lx = mobileRobot['wheelTrack']
       Ly = mobileRobot['wheelBase']
       R = mobileRobot['wheelRadius']
       #initialize mobile platform kinematics 
       platformKinematics = mobile.MobileKinematics(R,Lx,Ly,wheeltype=0)
   
       def UFoffset(mbs,t,itemIndex,offsetUserFunctionParameters):
           return offsetUserFunctionParameters
       mbs.SetObjectParameter(mobileRobotBackDic['oPlatformList'][0],'offsetUserFunctionParameters',offsetUserFunctionParametersSet)
       mbs.SetObjectParameter(mobileRobotBackDic['oPlatformList'][0],'offsetUserFunction',UFoffset)
       mbs.SetObjectParameter(mobileRobotBackDic['oPlatformList'][0],'constrainedAxes',constrainedAxesSet)
       ######################## Sensor data from mobile platform ###
       WheelSpringDamper = [0]*4
       MotorDataNode = [0]*4
       cWheelBrakes = [0]*4
       for i in range(4):     
           RM0 = mbs.GetObject(mobileRobotBackDic['oAxlesList'][i])['rotationMarker0']
           RM1 = mbs.GetObject(mobileRobotBackDic['oAxlesList'][i])['rotationMarker1']
           
       # wheel controller for KAIROS Platform  
           paramOpt = {'kMotor': 100, 'fact_dMotor': 0.5} 
           kWheelControl = paramOpt['kMotor'] 
           dWheelControl = kWheelControl * paramOpt['fact_dMotor']
           MotorDataNode[i] = mbs.AddNode(NodeGenericData(numberOfDataCoordinates = 1, initialCoordinates=[0]))
           WheelSpringDamper[i] = mbs.AddObject(TorsionalSpringDamper(name='Wheel{}Motor'.format(i), 
                                               markerNumbers=[mobileRobotBackDic['mAxlesList'][i], mobileRobotBackDic['mWheelsList'][i]],
                                               nodeNumber= MotorDataNode[i], # for continuous Rotation
                                               stiffness = kWheelControl, damping =  dWheelControl, offset = 0,
                                               rotationMarker0=RM0, 
                                               rotationMarker1=RM1))
           cWheelBrakes[i] = mbs.AddObject(GenericJoint(markerNumbers=
                                                   [mobileRobotBackDic['mAxlesList'][i], mobileRobotBackDic['mWheelsList'][i]], 
                                                   constrainedAxes = [0]*6, 
                                                   rotationMarker0=RM0, 
                                                   rotationMarker1=RM1, 
                                                   ))
           mbs.variables['flagBrakeActive'] = False
   
       # wheel user function  
       mbs.variables['signWheels'] = [-1,1,1,-1]
       mbs.variables['t0'] = 0
       mbs.variables['phiWheel'] = [0,0,0,0]
       vMax = 3.0
       wMax = vMax / mobileRobot['wheelRadius'] # m/s
       mbs.variables['wHistory'] = [[],[],[],[]] # for debug 
       def PreStepUFWheel(mbs, t, w= [0,0,0,0]):
           if t == 0: 
               return True
           dt =  mbs.sys['dynamicSolver'].it.currentStepSize 
           dwMax = wMax * dt
           
           if debugFlag: print('dwMax = ', dwMax)
           
           for i in range(4):
               wOld = mbs.GetObjectParameter(WheelSpringDamper[i], 'velocityOffset')
               if w[i] > wOld + dwMax: 
                   w[i] = wOld + dwMax 
               elif w[i] < wOld - dwMax: 
                   w[i] = wOld - dwMax 
               mbs.variables['phiWheel'][i] += (t-mbs.variables['t0'])*w[i] #* mbs.variables['signWheels'][i]
               mbs.SetObjectParameter(WheelSpringDamper[i], 'offset', mbs.variables['phiWheel'][i])
               mbs.SetObjectParameter(WheelSpringDamper[i], 'velocityOffset', w[i])
               mbs.variables['wHistory'][i] += [w[i]]
               # mbs.SetObjectParameter(WheelSpringDamper[i], 'offset', SmoothStep(t, 0.5 , 2, 0, 1)*t * mbs.variables['signWheels'][i])    
           mbs.variables['t0'] = t
           return True
       sPlatformPosition = mbs.AddSensor(SensorMarker(name='platformpos',markerNumber=mobileRobotBackDic['mPlatformList'][-1], 
                                                           fileName=mobRobSolutionPath + '/rollingDiscCarPos.txt', 
                                                           outputVariableType = exu.OutputVariableType.Position, writeToFile = sensorWriteToFile,storeInternal=True))
       sPlatformVelocity = mbs.AddSensor(SensorMarker(name='platformvelo',markerNumber=mobileRobotBackDic['mPlatformList'][-1], 
                                                           fileName=mobRobSolutionPath + '/rollingDiscCarVel.txt', 
                                                           outputVariableType = exu.OutputVariableType.Velocity, writeToFile = sensorWriteToFile,storeInternal=True))   
       sPlatformOrientation = mbs.AddSensor(SensorBody(name='platformRot',bodyNumber=mobileRobotBackDic['bPlatformList'][0], 
                                                           fileName=mobRobSolutionPath + '/rollingDiscCarOrientation.txt', 
                                                           outputVariableType = exu.OutputVariableType.Rotation , writeToFile = sensorWriteToFile))
       mbs.variables['sensorList'] = [sPlatformPosition, sPlatformOrientation]
       # ad manipulator to model 
       if 1: 
           mode='newDH'
           qOffset = [-np.pi * 1/4, 0,0,0,0,0]
           q0 = [-3*np.pi/4, np.pi - 1e-15  , np.pi*1.5/2 ,0- 1e-15 ,0- 1e-15 ,0- 1e-15 ] #zero angle configuration
           tx = 0.03
           zOff = -0.2
           toolSize= [tx*0.5, 0.06,0.12]
           r6 = 0.04
           graphicsToolList = []
           graphicsToolList += [graphics.Cylinder(pAxis=[0,0,zOff], vAxis= [0,0,np.abs(zOff)*0.8], radius=r6, color=graphics.color.red)]
           graphicsToolList+= [graphics.Brick([ tx,0, 0], toolSize, graphics.color.grey)]
           graphicsToolList+= [graphics.Brick([-tx,0, 0], toolSize, graphics.color.grey)]
           graphicsToolList+= [graphics.Brick([0,0, -0.05], [tx*5,0.09,0.04], graphics.color.grey)]
           graphicsToolList += [graphics.Basis(length=0.2)]
           basePoseHT=mobileRobot['platformInitialPose'] @ mobileRobot['serialRobotMountpoint'] @ HTrotateZ(qOffset[0]) #robot base position and orientation  
   
           # manipulator input with included function from exudyn robotics models
           myRobotList = ManipulatorUR5()
           robot = Robot(gravity=[0,0,-9.81],
               base = RobotBase(HT=basePoseHT), #visualization=VRobotBase(graphicsData=graphicsBaseList)),
               tool = RobotTool(HT=HTtranslate([0,0,0.155]), #  @ HTrotateZ(np.pi/2), 
                               visualization=VRobotTool(graphicsData=graphicsToolList)),
               referenceConfiguration = q0) #referenceConfiguration created with 0s automatically
           robot = LinkDict2Robot(myRobotList, robotClass=robot)
           #control parameters, per joint:
           fc=0.5
           Pcontrol = np.array([4000, 4000, 4000, 100, 100, 100])
           Dcontrol = np.array([60,   60,   60,   6,   6,   0.6])
           Pcontrol = fc*Pcontrol
           Dcontrol = fc*Dcontrol
           
           # change predefined control parameters
           for i in range(robot.NumberOfLinks()):
               robot.links[i].PDcontrol = (Pcontrol[i], Dcontrol[i])
           #trajectory generated with optimal acceleration profiles:
           trajectory = Trajectory(initialCoordinates=q0, initialTime=0)
           trajectory.Add(ProfileConstantAcceleration(q0,0.5))
           jointList = [0]*robot.NumberOfLinks() #this list must be filled afterwards with the joint numbers in the mbs!
           def ComputeMBSstaticRobotTorques(robot):
               q=[]
               for joint in jointList:
                   q += [mbs.GetObjectOutput(joint, exu.OutputVariableType.Rotation)[2]] #z-rotation
               HT=robot.JointHT(np.array(q)+qOffset)
               return robot.StaticTorques(HT)
           
           #++++++++++++++++++++++++++++++++++++++++++++++++
           #base, graphics, object and marker:
           #baseMarker; could also be a moving base according to doc but found no examples!
           baseMarker = mobileRobotBackDic['mPlatformList'][-1] # mbs.AddMarker(MarkerBodyRigid(bodyNumber=objectGround, localPosition=[0,0,0]))
           sArmTorque = [0,0,0,0,0,0]
           
           #++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
           #build mbs robot model:
   
           robotDict = robot.CreateRedundantCoordinateMBS(mbs, baseMarker=baseMarker)
           jointList = robotDict['jointList'] #must be stored there for the load user function
           torsionalSDlist = robotDict['springDamperList']
           
           for i in range(len(robotDict['springDamperList'])): 
               sArmTorque[i] = mbs.AddSensor(SensorObject(objectNumber= robotDict['springDamperList'][i], 
                                           fileName=mobRobSolutionPath+ '/ArmMotorTorque'+str(i) + '.txt', 
                                           outputVariableType = exu.OutputVariableType.TorqueLocal, writeToFile=sensorWriteToFile))
               # mbs.SetObjectParameter(torsionalSDlist[i], 'offset', q0[i] + qOffset[i])
           mEndeffektor = mbs.AddMarker(MarkerBodyRigid(name='Endeffektor', bodyNumber=robotDict['bodyList'][-1], localPosition=[0,0,0.157]))
           sEndeffektor = mbs.AddSensor(SensorMarker(markerNumber=mEndeffektor, writeToFile=False, outputVariableType=exu.OutputVariableType.Position))
           # robotics toolbox by Peter Corke 
           UR5_rtb = rtb.models.DH.UR5()
           UR5_rtb.tool = SE3([0,0,0.2])
           def getNewTraj(TArm, qLast): 
               vm = [np.pi * 1.1/2]*6
               am = [5 * 0.6]*6
               nAttempts = 10
               qNew = UR5_rtb.ikine_LM(TArm, q0 = qLast) # , q0=qLast-qOffset)
               iAttempt = 1
               if not(qNew.success): 
                   for iAttempt in range(1, nAttempts): 
                       # randomize initial angles to try to get a solution for the inverse kinematics
                       qNew = UR5_rtb.ikine_LM(TArm, q0 = (np.random.random(6)-1)*np.pi*2) # , q0=qLast-qOffset)
                       if qNew.success: 
                           break
                   if not(qNew.success): 
                       print('Inverse Kinematics could not be solved after {} attempts. \nPlease check if the given Pose \n{}\nis in the workspace.'.format(iAttempt, TArm))
                       return None, None
               teMax = 0       
               qNew = qNew.q # - qOffset
               for i in range(len(qNew)): 
                   te = abs(qNew[i] - qLast[i])/vm[i] + vm[i]/am[i]
                   if te > teMax: 
                       teMax = te
               print('Planned new PTP motion after {} attempts from:\n{}\nto\n{}\nin {}s. '.format(iAttempt, np.round(qLast, 2), np.round(qNew, 2), np.round(teMax, 2)))
               # qNew[1] += np.pi*2
               return qNew, teMax
           
           def activateBrakes(cJoints, oTSD, flag): 
               for i in range(len(cJoints)): 
                   # mbs.SetObjectParameter(cJoints[i], 'constrainedAxes', [0,0,0,0,0,1*bool(flag)])
                   if flag: # deactivate motors 
                       mbs.SetObjectParameter(oTSD[i], 'stiffness', 100)
                       mbs.SetObjectParameter(oTSD[i], 'damping', 5)
                   else: 
                       mbs.SetObjectParameter(oTSD[i], 'stiffness', kWheelControl)
                       mbs.SetObjectParameter(oTSD[i], 'damping', dWheelControl)
                   
               print('un'*bool(not(flag)) + 'locking wheels')
               return True
           
           hObj = 0.08
           xTable, yTable, hTable = 0.2, 0.4, 0.645
           if flagFixObject: 
               PosObj = [3.0,1.0, hTable + hObj/2] # for testing of grasp
           else: 
               PosObj = (np.random.rand(3)-0.5) * [0.4,0.8,0] + [1.5 ,0, hTable + hObj/2]
           
           graphicsTarget = graphics.Cylinder(pAxis = [0,0,-hObj/2], vAxis = [0,0,hObj ], radius = 0.02, color=graphics.color.lightgreen)
           inertiaTarget = InertiaCylinder(500, hObj , 0.02, 2)
           # old interface to rigid body:
           # nObj, bObj = AddRigidBody (mainSys = mbs, 
           #             inertia = inertiaTarget, 
           #             nodeType = str(exu.NodeType.RotationEulerParameters), 
           #             position = PosObj, 
           #             rotationMatrix = np.eye(3),
           #             angularVelocity =  [0,0,0],
           #             velocity= [0,0,0],
           #             gravity = [0,0,0], 
           #             graphicsDataList = [graphicsTarget])
           dictObj = mbs.CreateRigidBody(
                         inertia=inertiaTarget, 
                         referencePosition=PosObj, 
                         referenceRotationMatrix=np.eye(3),
                         gravity=[0, 0, 0], 
                         graphicsDataList=[graphicsTarget],
                         returnDict=True)
           [nObj, bObj] = [dictObj['nodeNumber'], dictObj['bodyNumber']]
           
           mObj = mbs.AddMarker(MarkerBodyRigid(bodyNumber=bObj))
           cGrasp = mbs.AddObject(RigidBodySpringDamper(markerNumbers=[mEndeffektor, mObj], stiffness = np.eye(6)*1e3, damping = np.eye(6)*1e2, 
                                                       visualization={'show': False, 'drawSize': -1, 'color': [0]*4}, activeConnector=False))
           graphicsTable = graphics.Brick(centerPoint = [0,0,0], size = [xTable, yTable, hTable], color=graphics.color.darkgrey2)
           
           # nTable, bTable = AddRigidBody (mainSys = mbs, 
           #                 inertia = inertiaTarget, 
           #                 nodeType = str(exu.NodeType.RotationEulerParameters), 
           #                 position = list(PosObj[0:2]) + [hTable/2], 
           #                 rotationMatrix = np.eye(3),
           #                 angularVelocity =  [0,0,0],
           #                 velocity= [0,0,0],
           #                 gravity = [0,0,0], 
           #                 graphicsDataList = [graphicsTable])
           dictTable = mbs.CreateRigidBody(
                         inertia=inertiaTarget, 
                         referencePosition=list(PosObj[0:2]) + [hTable/2], 
                         referenceRotationMatrix=np.eye(3),
                         gravity=[0, 0, 0], 
                         graphicsDataList=[graphicsTable],
                         returnDict=True)
           [nTable, bTable] = [dictTable['nodeNumber'], dictTable['bodyNumber']]
   
               
           mbs.variables['myDict'] = {}
           
           #prestep user functions
           for i in range(6): 
               mbs.variables['qDebug{}'.format(i)] = []
           mbs.variables['state'] = 0
           mbs.variables['trajectory'] = trajectory
           #user function which is called only once per step, speeds up simulation drastically
           
           def PreStepUF(mbs, t):
               if compensateStaticTorques:
                   staticTorques = ComputeMBSstaticRobotTorques(robot)
               else:
                   staticTorques = np.zeros(len(jointList))
               
               PosPlatform =  mbs.GetSensorValues(mbs.variables['sensorList'][0]) - [0.178, 0 , 0 ]#mbs.variables['sensorRecord{}'.format(mbs.variables['sensorRecorders'][0])]
               ThetaPlatform = mbs.GetSensorValues(mbs.variables['sensorList'][1])[-1]
               
               phi = np.zeros(len(robot.links))
               for i in range(len(robot.links)): 
                       phi[i] = mbs.GetObjectOutput(jointList[i], exu.OutputVariableType.Rotation)[2] #z-rotation
                       
               if t > mbs.variables['trajectory'][-1]['finalTime']: 
                   armStatus = 1 # current trajectory finished
               else: 
                   armStatus = 0
               
               # here functionstatemachine in preStepUserFunction call 
               vel, TArm, grasp, mbs.variables['state'], mbs.variables['myDict'] = funcStatMachine(t, PosPlatform, ThetaPlatform, 
                                                       PosObj, armStatus, mbs.variables['state'], mbs.variables['myDict'])
               if type(TArm) == np.ndarray: 
                   TArm = SE3(TArm)
               if mbs.variables['state'] == -1: 
                   mbs.SetRenderEngineStopFlag(True)
                   print('finished Statemachine. ')
               
               # platform kinematics calculation 
               w = platformKinematics.GetWheelVelocities(vel)
   
               if TArm != None: 
                   lastTraj = mbs.variables['trajectory'][-1]
                   qLast = lastTraj['coordinateSets'][-1] # the last desired joint angles
                   tLast = lastTraj['finalTime'] # 
                   qNew, Ttraj = getNewTraj(TArm, qLast)
                   if type(qNew) != type(None): 
                       mbs.variables['trajectory'].Add(ProfileConstantAcceleration(qLast,t-tLast+0.1))
                       mbs.variables['trajectory'].Add(ProfileConstantAcceleration(qNew,Ttraj))
                   # print('\n\nTTraj is: \n{}\n\n'.format(np.round(Tj, 3)))
               
               if grasp:
                   pEE = mbs.GetSensorValues(sEndeffektor)
                   distanceGrasp = pEE - PosObj - [0,0,0]
                   # print('distance grasp = ', distanceGrasp)
                   if np.linalg.norm(distanceGrasp) < 0.1: # distance of grasp to  
                       print('grasp successful!')    
                       # activate constraint for grasp 
                       RotEE = mbs.GetNodeOutput(robotDict['nodeList'][-1], exu.OutputVariableType.RotationMatrix).reshape([3,3])
                       mbs.SetObjectParameter(cGrasp, 'rotationMarker1', RotEE)
                       # mbs.SetObjectParameter(cGrasp, '', RotEE) # position
                       
                       # mbs.SetObjectParameter(cGrasp, 'constrainedAxes', [1]*6)
                       # mbs.SetObjectParameter(cGrasp, '', [1]*6)
                       mbs.SetObjectParameter(cGrasp, 'activeConnector', True)
                       offset_local = list(RotEE @ distanceGrasp) 
                       offset_local[1] = 0
                       mbs.SetObjectParameter(cGrasp, 'offset', offset_local + [0,0,0]) # list(RotEE @ distanceGrasp) + [0,0,-hObj*0])
                   else: 
                       print('grasping the object was not successful. Calculated distacne = {}'.format(np.round(distanceGrasp, 3)))
               [u,v,a] = mbs.variables['trajectory'].Evaluate(t)
               for i in range(len(robot.links)):
                   tsd = torsionalSDlist[i]
                   mbs.SetObjectParameter(tsd, 'offset', u[i] + qOffset[i])
                   mbs.SetObjectParameter(tsd, 'velocityOffset', v[i])
                   mbs.SetObjectParameter(tsd, 'torque', staticTorques[i]) #additional torque from given velocity 
               # send velocity data to ROS
               myROSInterface.PublishTwistUpdate(mbs,t)
               # send position data to ROS 
               myROSInterface.PublishPoseUpdate(mbs,t)
               PreStepUFWheel(mbs, t, w)
               
               if np.linalg.norm(w) < 1e-5 and t > 0.5:
                   if not(mbs.variables['flagBrakeActive']): 
                       activateBrakes(cWheelBrakes, WheelSpringDamper, True)
                       mbs.variables['flagBrakeActive'] = True                  
               else: 
                   if mbs.variables['flagBrakeActive']: 
                       activateBrakes(cWheelBrakes, WheelSpringDamper, False)
                       mbs.variables['flagBrakeActive'] = False      
               
               return True
           
       mbs.SetPreStepUserFunction(PreStepUF)
       SC.visualizationSettings.interactive.trackMarker = mobileRobotBackDic['mPlatformList'][0]
       # start simulation: 
       mbs.Assemble()
       PreStepUF(mbs, 0)
       
       SC.visualizationSettings.connectors.showJointAxes = True
       SC.visualizationSettings.connectors.jointAxesLength = 0.02
       SC.visualizationSettings.connectors.jointAxesRadius = 0.002
       SC.visualizationSettings.nodes.showBasis = True
       SC.visualizationSettings.nodes.basisSize = 0.1
       SC.visualizationSettings.loads.show = False
       SC.visualizationSettings.openGL.multiSampling=4
       SC.visualizationSettings.openGL.shadow = 0.5
       SC.visualizationSettings.openGL.light0position = [0, -2, 10, 0]
       SC.visualizationSettings.openGL.shadowPolygonOffset = 0.1
       #mbs.WaitForUserToContinue()
   
       simulationSettings = exu.SimulationSettings() #takes currently set values or default values
       simulationSettings.timeIntegration.numberOfSteps = int(tEnd/hstepsize)
       simulationSettings.timeIntegration.endTime = tEnd
       simulationSettings.solutionSettings.solutionWritePeriod = hstepsize #0.005
       simulationSettings.solutionSettings.sensorsWritePeriod = hstepsize # 0.005
       simulationSettings.solutionSettings.binarySolutionFile = False
       simulationSettings.solutionSettings.writeSolutionToFile = False
   
       simulationSettings.timeIntegration.simulateInRealtime = True
       #simulationSettings.timeIntegration.realtimeFactor = 0.25
       simulationSettings.timeIntegration.verboseMode = verboseMode
       #simulationSettings.timeIntegration.newton.useModifiedNewton = True
       #simulationSettings.timeIntegration.generalizedAlpha.useIndex2Constraints = True
       #simulationSettings.timeIntegration.generalizedAlpha.useNewmark = simulationSettings.timeIntegration.generalizedAlpha.useIndex2Constraints
       simulationSettings.timeIntegration.newton.useModifiedNewton = True
       #simulationSettings.timeIntegration.generalizedAlpha.spectralRadius = 0.5 # 0.25
       simulationSettings.timeIntegration.discontinuous.maxIterations = 3
       simulationSettings.timeIntegration.adaptiveStepRecoveryIterations = 10
       simulationSettings.timeIntegration.generalizedAlpha.computeInitialAccelerations= True # False
   
       simulationSettings.displayComputationTime = True
       simulationSettings.displayStatistics = True
       #simulationSettings.linearSolverType = exu.LinearSolverType.EigenSpars
   
   
       SC.visualizationSettings.general.autoFitScene=False
       SC.visualizationSettings.general.renderWindowString = 'Mobile Robot Simulation'
       SC.visualizationSettings.window.renderWindowSize=[1920,1200]
       SC.visualizationSettings.window.startupTimeout=5000
       SC.visualizationSettings.interactive.selectionLeftMouse = False
       SC.visualizationSettings.interactive.selectionRightMouse = False
       
       SC.visualizationSettings.openGL.initialModelRotation =RotationMatrixZ(-0.2) @ RotationMatrixX(np.pi/2.7)  #
       SC.visualizationSettings.openGL.initialZoom = 1.5
       SC.visualizationSettings.openGL.initialCenterPoint = [0, 2, 0] # -1.7, -2, -2]
       # SC.visualizationSettings.contour.outputVariable = exu.OutputVariableType.Displacement
       # SC.visualizationSettings.contour.outputVariableComponent = 2 #0=x, 1=y, 2=z
       exu.StartRenderer()
       mbs.WaitForUserToContinue()
       exu.SolveDynamic(mbs, simulationSettings, showHints=True, storeSolver = True)
       #mbs.WaitForRenderEngineStopFlag()
       exu.StopRenderer()
       
       # for debug 
       if debugFlag: 
           import matplotlib.pyplot as plt
           for i in range(4): 
               plt.plot(mbs.variables['wHistory'][i], label='wheel ' + str(i+1))
           plt.legend()
           plt.show()
           mbs.PlotSensor(sensorNumbers=[sPlatformPosition], components=[0,1,2], labels=['x(m); ','y','z'], colorCodeOffset=2, closeAll=False)
           mbs.PlotSensor(sensorNumbers=[sPlatformVelocity], components=[0,1,2], labels=['vx(m); ','vy','vz'], colorCodeOffset=2, closeAll=False)
       return mbs
   
   # main function 
   if __name__ == '__main__': 
       if not(checkInstall('exudyn')): 
           print('Error! Simulation can not be started!')
           import sys
           sys.exit()
   
       print('Starting Simulation...')
       # initialize ROS interface from own subclass
       myROSInterface = MyExudynROSInterface()
       # start simulation
       SimulationMobileRobot(functionStateMachine,myROSInterface,p0=[0,0],flagFixObject=True)

