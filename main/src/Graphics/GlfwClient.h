/** ***********************************************************************************************
* @class        GlfwClient
* @brief        Hub to glfw class for 3D visualization using OpenGL
*
* @author       Gerstmayr Johannes
* @date         2019-05-24 (generated)
*
* @copyright    This file is part of Exudyn. Exudyn is free software: you can redistribute it and/or modify it under the terms of the Exudyn license. See "LICENSE.txt" for more details.
* @note         Bug reports, support and further information:
                - email: johannes.gerstmayr@uibk.ac.at
                - weblink: https://github.com/jgerstmayr/EXUDYN
                
************************************************************************************************ */
#ifndef GLFWCLIENT__H
#define GLFWCLIENT__H

#include "Utilities/ReleaseAssert.h"
#include "Utilities/BasicDefinitions.h"
#include "Main/rendererPythonInterface.h"

#include <ostream>
//#include <stdlib.h> //only works in MSVC for initialization with std::vector
#include <array>
#include <vector>
#include <stdio.h>
#include <thread>

#include "Graphics/GraphicsData.h"
#include "Autogenerated/VisualizationSettings.h"
#include "Graphics/VisualizationSystemContainerBase.h" //common interface to visualization system

#ifdef USE_GLFW_GRAPHICS

#define GLFW_INCLUDE_GLEXT
//#define GL_GLEXT_PROTOTYPES OpenGL3.2
#include <GLFW/glfw3.h>

#define USE_TEXTURED_BITMAP_FONTS //!< activate this flag for faster textured based fonts with glLists
#define NUMBER_OF_TEXTUREFONT_LISTS 2 //1 is standard; 2 are craeted to switch between transparent and font with background

#include "Graphics/GlfwClientText.h" //link to external library; include only if copyright is appropriate
#include "Graphics/GlfwClientBitmapText.h"

//currently done in preprocessor flags; check that openvr_api.dll is included in exudynCPP.pyd directory
//#define __EXUDYN_USE_OPENVR //done with preprocessor flags in VisualStudio or in setup.py
//#if !defined(__EXUDYN__APPLE__) && !defined(__EXUDYN__LINUX__ARM__) //would not work with APPLE


enum class RendererMode {
	_None, Move, Rotate, ZoomView, Select
};

////! size for bitmap fonts
//enum class FontSizeType {
//	small = 1,		//small text, e.g., for contour numbers
//	normal = 2,		//regular text
//	large = 4,		//large text
//	userText = 8,	//size of user texts
//};



//! variables that describe the renderer's state machine ==> zoom, move, etc. with mouse
class RenderStateMachine
{
public:
	bool leftMousePressed;
	bool rightMousePressed;
	bool shiftPressed;
	bool ctrlPressed;
	RendererMode mode;			//!< determines the state of any action

	double mousePositionX;		//!< currently recorded mouse position; use double for GLFW compatibility
	double mousePositionY;		//!< currently recorded mouse position; use double for GLFW compatibility
	double lastMousePressedX;	//!< last mouse button position pressed
	double lastMousePressedY;	//!< last mouse button position pressed

	float storedCenterPointX;	//!< stored centerpoint position during mouse-move
	float storedCenterPointY;	//!< stored centerpoint position during mouse-move

	Matrix4DF storedModelRotation;//!< stored rotation matrix before right mouse button pressed

	Vector3D storedJoystickPosition;    //!< stored position of joystick, if available
	Vector3D storedJoystickRotation;	//!< stored rotation of joystick, if available

	//++++++++++++++++++++++++++++++++++++
	//selection:
	Vector2D selectionMouseCoordinates; //!< mouse coordinates used for selection
	Index highlightIndex;			//!< item index of selected item (with mouse click)
	ItemType highlightType;			//!< item type of selected item
	Index highlightMbsNumber;			//!< mbs number of selected item
	Real highlightTimeout; 			//!< time at which highlighted item shall be removed, using EXUstd::GetTimeInSeconds(); 0, if no timeout

	std::string rendererMessage;		//!< rendererMessage to be shown in status line
	Real renderMessageTimeout; 			//!< time at which message shall be removed, using EXUstd::GetTimeInSeconds(); 0, if no timeout

};


//! this is the rendering module for displaying 3D model data
class GlfwRenderer
{
private:
	//static RenderState state;
	static bool rendererActive;			//!< signal that shows that renderer is active
	static bool stopRenderer;			//!< signal that shows that renderer should quit
	static bool useMultiThreadedRendering;		//!< according to visualizationSettings.general; always false for MACOS (__EXUDYN__APPLE__)
    static Real lastGraphicsUpdate;		//!< time of last graphics update
    static Real lastTryCloseWindow;		//!< time of last trial to close window (for security closing)
    static Real lastEventUpdate;		//!< time of last event polling
    static Real rendererStartTime;      //!< time then renderer was started; used to check if quit requires security question
	static bool callBackSignal;			//!< for single threaded applications, react if callback is sent=> update graphics immediately

	static GLFWwindow* window;
	static RenderState* state;		//!< this represents the current OpenGL parameters
	static RenderStateMachine stateMachine; //!< all variables (mouse, keyboard, ...) used for state machine (zoom, zoom-view, move, ...)
	static std::thread rendererThread;	//!< std::thread variable for rendererThread
	static Index rendererError;			//!< 0 ... no error, 1 ... glfwInit() failed, 2 ... glfwCreateWindow failed, 3 ... other error
	static Index verboseRenderer;        //!< initialized in SetupRenderer(bool verbose): output helpful information
	//static Index firstRun; //zoom all in first run
	static std::atomic_flag renderFunctionRunning;  //!< semaphore to check if Render(...)  function is currently running (prevent from calling twice)
	static std::atomic_flag showMessageSemaphore;   //!< semaphore to prevent calling ShowMessage twice


	//+++++++++++++++++++++++++++++++++++++++++
	static BitmapFont bitmapFont;				//!< bitmap font for regular texts and for textured fonts, initialized upon start of renderer
	static float fontScale;						//!< monitor scaling factor from windows, to scale fonts
	static constexpr float fontSmallFactor = 0.7500001f; //!< factor for small fonts
	static constexpr float fontLargeFactor = 1.400001f;  //!< factor for large fonts
	static constexpr float fontHugeFactor = 2.500001f;  //!< factor for large fonts
#ifndef USE_TEXTURED_BITMAP_FONTS
	static BitmapFont bitmapFontSmall;			//!< bitmap font for small texts, initialized upon start of renderer
	static BitmapFont bitmapFontLarge;			//!< bitmap font for large texts, initialized upon start of renderer
	static BitmapFont bitmapFontHuge;			//!< bitmap font for huge texts, initialized upon start of renderer
#else
	//+++ for textures with glLists +++:
    static bool depthMask;                      //!< used for font drawing; current state of depth mask
    static GLdouble zFactor;                    //!< for clipping in ortho projection
public:
	static GLuint textureNumberRGBbitmap[256*NUMBER_OF_TEXTUREFONT_LISTS];	//!< store texture numbers for every character of bitmap font
private:
    static GLuint bitmapFontListBase;			//!< starting index for GLlists for font bitmap textured quads
	static ResizableArray<GLubyte> charBuffer;	//!< buffer for converstion of UTF8 into internal unicode-like format
#endif

	static GLuint spheresListBase;			//!< starting index for GLlists for spheres
	static constexpr Index maxSpheresLists = 8; //!< max. number of GLlists for spheres (with resolution 2,4,8,16, etc.

	static constexpr Index invalidIndex = -1; //!< for some state machine variables (itemIndex, joystick number, ...)
	//+++++++++++++++++++++++++++++++++++++++++
	//link to GraphicsData and Settings:
	static ResizableArray<GraphicsData*>* graphicsDataList;					//!< link to graphics data; only works for one MainSystem, but could also be realized for several systems
	static VisualizationSettings* visSettings;  //!< link to visualization settings
	static VisualizationSystemContainerBase* basicVisualizationSystemContainer;
    //+++++++++++++++++++++++++++++++++++++++++
    //for sensor traces:
    static Vector3DList sensorTracePositions;
    static Vector3DList sensorTraceVectors; //synchronized with triads
    static Matrix3DList sensorTraceTriads;  //synchronized with vectors
    static Vector sensorTraceValues; //temporary storage for current sensor data
    //+++++++++++++++++++++++++++++++++++++++++

public:
	GlfwRenderer();
	~GlfwRenderer() 
	{	
		//glfwTerminate(); //move to destructor
	};

	//! Initializes and starts the Renderer in a separate thread;
	//  Returns false, if problems with glfw library or windows creation, otherwise true; 
	//  @todo test with apple and linux
	static bool SetupRenderer(Index verbose = 0);

	//! stop the renderer engine and its thread; @todo StopRenderer currently also stops also main thread (python)
	static void StopRenderer();

    //! return renderState object
    static RenderState GetRenderState() { return *state; }

    //! return renderState object
    static VisualizationSettings* GetVisualizationSettings() { return visSettings; }
    //static const VisualizationSettings& GetVisualizationSettings() const { return *visSettings; }

	//! reset some state machines, e.g., left mouse click, item select, etc.
	static void ResetStateMachine();

	static bool IsGlfwInitAndRendererActive()
	{
		if (window && rendererActive) { return true; }
		else { return false; }
	}

	//! return verbose flag for outside; can be 0, 1 or larger
	static Index Verbose() { return verboseRenderer; }

	//! Links the Renderer to a specific GraphicsData/settings; 
	//! Only one data linked at one time
	//! Returns true on success and false, if data is already linked (==> call DetachVisualizationSystem first)
	static bool LinkVisualizationSystem(ResizableArray<GraphicsData*>* graphicsDataListInit, VisualizationSettings* settingsInit,
										VisualizationSystemContainerBase* basicVisualizationSystemContainerInit, RenderState* renderStateInit)
	{
		if (graphicsDataList == nullptr)
		{
			graphicsDataList = graphicsDataListInit;
			visSettings = settingsInit;
			basicVisualizationSystemContainer = basicVisualizationSystemContainerInit;
			state = renderStateInit;

			basicVisualizationSystemContainer->InitializeView(); //initializes renderState; this is done to make OpenGL zoom and maxSceneCoordinates work
			basicVisualizationSystemContainer->SetComputeMaxSceneRequest(true); //computes maxSceneCoordinates for perspective and shadow
			if (visSettings->general.autoFitScene)
			{
				basicVisualizationSystemContainer->SetZoomAllRequest(true);
			}

			//moved to InitializeView:
			////now state can be initialized:
			//state->mouseCoordinates = Vector2D({ 0.,0. });
			//state->openGLcoordinates = Vector2D({ 0.,0. });
			//state->mouseLeftPressed = false;
			//state->mouseRightPressed = false;
			//state->mouseMiddlePressed = false;
			//state->joystickAvailable = invalidIndex;
			//state->displayScaling = 1;

			//state->projectionMatrix; //initialized in InitializeView()

			return true;
		}
		else { return false; }
	}

	//! Detach the GraphicsData/settings; enables the visualization of different MainSystems; return true on success
	static bool DetachVisualizationSystem(VisualizationSystemContainerBase* detachingVisualizationSystemContainer)
	{
		//only detach, if detachingVisualizationSystemContainer is still linked to GLFWrenderer
		if (detachingVisualizationSystemContainer == nullptr || basicVisualizationSystemContainer == detachingVisualizationSystemContainer)
		{
			StopRenderer();

			if (graphicsDataList == nullptr) { return false; }//this just shows that no system was linked yet
			else
			{
				graphicsDataList = nullptr;
				visSettings = nullptr;

				window = nullptr; //is set in StopRenderer anyway
				state = nullptr;
				basicVisualizationSystemContainer = nullptr;

				return true;
			}
		}
		return false;
	}

	static void UpdateGraphicsDataNow()
	{
		if (basicVisualizationSystemContainer)
		{
			basicVisualizationSystemContainer->UpdateGraphicsDataNow();
		}
	}

	//! write dictionary for selected item; return true if success; MAY ONLY BE CALLED FROM PYTHON THREAD!!!
	static bool PySetRendererSelectionDict(Index itemID);

	//! retrieve basic item information from MainSystemBacklink; return true if success; renderer thread safe (no Python calls)
	static bool GetItemInformation(Index itemID, STDstring& itemTypeName, STDstring& itemName);// , STDstring& itemInfo);

	//! print delayed via safe communication with main thread
	static void PrintDelayed(const STDstring& str, bool lineFeed = true) 
	{
		if (useMultiThreadedRendering)
		{
			if (lineFeed)
			{
				//PyQueueExecutableString("print('" + str + "')\n");
				outputBuffer.WriteVisualization(str+'\n'); //shall be faster by putting into outputBuffer, rather than executing python command
			}
			else
			{
				//PyQueueExecutableString("print('" + str + "', end='')\n");
				outputBuffer.WriteVisualization(str + '\n'); //shall be faster by putting into outputBuffer, rather than executing python command
			}
		}
		else
		{
			pout << str;
			if (lineFeed) { pout << "\n"; }
		}
	};

	static STDstring OnOffFromBool(bool flag) { if (flag) { return "on"; } else { return "off"; } }

	//! add status message, e.g., if button is pressed
	static void ShowMessage(const STDstring& str, Real timeout = 0);

	//! run renderer idle for certain amount of time; use this for single-threaded, interactive animations; waitSeconds==-1 waits forever
	static void DoRendererIdleTasks(Real waitSeconds, bool graphicsUpdateAndRender=false);

	//! check if separate thread used:
	static bool UseMultiThreadedRendering() { return useMultiThreadedRendering; }

	//! access keypress user function from current SC; only available if SC linked:
	static std::function<bool(int, int, int)> GetKeyPressUserFunction() 
	{ 
		if (IsGlfwInitAndRendererActive() && visSettings != nullptr)
		{
			return visSettings->window.keyPressUserFunction;
		}
		else
		{
			return 0;
		}
	}

	//! set projection matrix from outside GlfwClient, used for OpenVR; this needs to be set before rendering objects!
	//! projectionInfo used to switch between different modes
	static void SetProjectionMatrix(const Matrix4DF& p, Index projectionInfo = 0) 
	{ 
		state->projectionMatrix = p; 
		state->projectionInfo = projectionInfo;
	};

	//! get current glfw window size
	static void GetWindowSize(int& width, int& height)
	{
		glfwGetFramebufferSize(window, &width, &height);
	}

	//! Render 3D scence function called from Render(), containing 3D model without additional text, etc.; projection is supplied
	static void Render3Dobjects(int screenWidth, int screenHeight, float& screenRatio, float& zoom);

private: //to be called internally only!
	static void error_callback(int error, const char* description)
	{
		std::cout << description << "\n";
	}

	//! GLFW callback functions:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void window_close_callback(GLFWwindow* window);
	static void window_content_scale_callback(GLFWwindow* window, float xscale, float yscale) { SetContentScaling(xscale, yscale); }

	//! compute unified content scaling based on values provided by GLFW
	static void SetContentScaling(float xScale, float yScale);
	static float GetFontScaling() { if (state != nullptr) { return state->displayScaling; } else { return 1; } }
    static void SetFontScaling(float scaling);

	//! return true, if joystick available and updated values are available; if joystickNumber==invalidIndex, chose a joystick; 
	//! if joystickNumber!=invalidIndex, it uses the fixed joystick until end of Renderer
	static bool GetJoystickValues(Vector3D& position, Vector3D& rotation, Index& joystickNumber);
	
	//! read joystick values; if changed, send refresh signal for graphics
	static void ProcessJoystick();

	//! if callback function like mousemove is called, immediately refresh graphics independently of graphicsUpdateInterval
	static void SetCallBackSignal(bool flag = true) { callBackSignal = flag; }
	static bool GetCallBackSignal() { return callBackSignal; }
	
	//! zoom in to mouse position, used to render that area lateron (replacement for gluPickMatrix(...)
	static void SetViewOnMouseCursor(GLdouble x, GLdouble y, GLdouble deltax, GLdouble deltay, GLint viewport[4]);
	
	//! function to evaluate selection of items
	static void MouseSelectOpenGL(GLFWwindow* window, Index mouseX, Index mouseY, Index& itemID);

	//! function to evaluate selection of items, return true, if item selected
	static bool MouseSelect(GLFWwindow* window, Index mouseX, Index mouseY, Index& itemID);

	//! GlfwInit and glfw->CreateWindow() calls; returns false, if functions fail
	static void InitCreateWindow();

	//! loop which checks for keyboard/mouse input; check for visualization updates (new data, window size changed, zoom, mouse move, etc.) and calls Render()
	static void RunLoop();

	//! tasks which are regularly called by RunLoop(), used if no separate thread used in GLFW; use wait in seconds to do this 
	//! for single-threaded renderer, an immediate rendering can be requested
	static void DoRendererTasks(bool graphicsUpdateAndRender = false);

	//! tasks which are done if renderer is shut down
	static void FinishRunLoop();

	//! Render function called for every update of OpenGl window
	static void Render(GLFWwindow* window); //GLFWwindow* needed in argument, because of glfwSetWindowRefreshCallback

	//! set background for scene (gradient background)
	static void AddGradientBackground(float zoom, float ratio);
	
	//! load GL_PROJECTION and set according to zoom, perspective, etc.; one function for render and mouse select
	static void SetProjection(int width, int height, float ratio, float& zoom);

	//! set model view rotation and translation, unified for Render and mouse select
	static void SetModelRotationTranslation(); 
																								   
	//! check if frame shall be grabed and saved to file using visualization options
	static void SaveImage();

	//! save scene to a file with filename
	static void SaveSceneToFile(const STDstring& filename);
	
    //! Render particulary the graphics data of multibody system; selectionMode==true adds names
    static void RenderGraphicsData(bool selectionMode = false);

       //! render sensor traces if activated and available
    static void RenderSensorTraces();

    //! Render particulary the text of multibody system; selectionMode==true adds names
    static void RenderGraphicsDataText(GraphicsData* data, Index lastItemID, bool highlight, Index highlightID, Float4 highlightColor2, Float4 otherColor2, bool selectionMode=false);

	//! Render triangles with stencil shadow method (slow, but accurate and should be sufficient for some animations)
	static void DrawTrianglesWithShadow(GraphicsData* data); 
	
	//! compute maxSceneSize and center from graphicsData 
	static void ComputeMaxSceneSize(float& maxSceneSize, Float3& center);

	//! Zoom all graphics objects (for current configuration)
	static void ZoomAll(bool updateGraphicsData=true, bool computeMaxScene=true, bool render=true);

    //! Set all light functions for openGL
    static void SetGLLights();

    //! Set depth mask and track its state; used to track current state for font drawing
    static void SetGLdepthMask(bool flag) { depthMask = flag; glDepthMask(flag); };

    //! get internally stored state of depth mask
    static bool GetGLdepthMask() { return depthMask; };

	//!transform pixel coordinates (from bottom/left) into vertex coordinates
	//!works if model view is initialized with 	glMatrixMode(GL_MODELVIEW);glLoadIdentity();
	static Float2 PixelToVertexCoordinates(float x, float y);

	//! check whether a point is clipped (not visible due to clipping plane
	//! function assumes that valid index is already checked before!
	static bool IsClipped(const Float3& p)
	{
		return (p * visSettings->openGL.clippingPlaneNormal) > visSettings->openGL.clippingPlaneDistance;
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//FONTS
	//! initialize bitmap for bitmap font (loaded from characterBitmap.h
	static void InitFontBitmap(guint fontSize);// , guint fontSizeSmall, guint fontSizeLarge, guint fontSizeHuge);

	//! initialize some GLlists, e.g., for spheres
	static void InitGLlists();

	//! draw a 0-terminated text string with fontSize, including monitor scaling factor; (for line-characters: size=1: height=1; width=0.5 for one character; distance = 0.25)
	//! switches to strings drawn by textures (default) or lines
	static void DrawString(const char* text, float fontSizeScaled, const Float3& p, Float4 color, bool transparent = true);

	//! draw string with scalable bitmap fonts, using textures
	static void DrawStringWithTextures(const char* text, float fontSizeScaled, const Float3& p, const Float4& color,
		BitmapFont& font, ResizableArray<GLubyte>& charBuffer, GLuint listBase, bool transparent = true);

	//! create glTexImage2D objects for font characters, stored in textureNumberRGBbitmap
	static void CreateFontTextures();

	//! create glLists for texture with textureNumber
	static void CreateTexturedQuadsLists(GLuint& listBase, GLuint* textureNumber,
		guint nCharacters, guint wCharacter8, guint wCharacter, guint hCharacter, bool itemTags = false);

	//draw string with GLlists, previously created by CreateTexturedQuadsLists
	static void DrawStringWithGLlistTextures(const Float3& p, float fontSizeScaled, GLuint listBase, GLubyte *string, guint stringLen, GLuint listOffset);

	//! OpenGL testing functions for destructor
	static void DeleteFonts();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

};

extern GlfwRenderer glfwRenderer; //this is the (static) location of the renderer class; could also be made dynamic



#endif //USE_GLFW_GRAPHICS
#endif //include once
