//----------------------------------------------------//
//                                                    //
// File: Main.cpp                                     //
// Handles the entry point of the application,  	  //
// creates the window and initializes the GLContext   //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

// Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "HelpLib.h" 
// Header file for our OpenGL functions
#include "Renderer.h" 
#include "Main.h" 

WindowGL window;

void main(int argc, char* argv[])
{
	// This is the entry point of the application.
	// This function initializes GLUT, creates the window
	// and sets the callbacks for several GLUT functions
	// which calls functions necessary for rendering a frame,
	// reshaping the window and handling key and mouse events

	// remove the console
	FreeConsole();

	// glutInit initializes the GLUT library.
	// if something goes wrong, this will terminate with an error message
	glutInit(&argc, argv);

	// set OpenGL Context as the default
	glutInitContextVersion(window.gl_major_version , window.gl_minor_version);

	// Do not include any deprecated features (meaningless for OpenGL3.2+ core)
	int context_args = GLUT_FORWARD_COMPATIBLE;
#if defined(_DEBUG) && defined(DEBUG_CONTEXT)
	// used via GLEW_ARB_debug_output
	context_args |= GLUT_DEBUG;
#endif // _DEBUG
	glutInitContextFlags(context_args);

	// Do not include any features removed in previous versions (fixed-function pipeline, etc)
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// inform FREEGLUT that we want to leave the main loop when the X button is pressed
	// this is CRITICAL since we need to release any allocated memory before
	// exiting the application
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	// glutInitDisplayMode tells GLUT what sort of rendering context we want.
	// Here we initialize the GL Context with double buffering enabled
	// and RGBA pixel type.
	// Multiple options are asked by OR-ing the parameters using the | operator
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

	// glutInitWindowSize tells GLUT the dimensions of the window we want to create
	glutInitWindowSize(window.m_width, window.m_height);

	// glutInitWindowPosition sets the parameters for the place on our screen we want
	// the window to appear on.
	// In this case the top left corner of the window will appear at position 50,50 of our screen
	glutInitWindowPosition(window.m_position.x, window.m_position.y);

	// glutCreateWindow will create a window based on the setting set above
	// with the name passed as an argument
	glutCreateWindow(window.m_title.c_str());

	// Callbacks for the GL and GLUT events:

	// Call back for the rendering function.
	// Each time glutPostRedisplay() is called, the function passed as an 
	// argument here is called to redraw the window.
	glutDisplayFunc(Render);
	
	// The reshape callback function is called when the window changes size
	// (so we can adjust our viewports and redraw stuff)
	glutReshapeFunc(Resize);

	// Input Callbacks

	// Callback for keyboard control
	// Each time a keyboard key is pressed, GLUT calls the keyboard function
	glutKeyboardFunc(Keyboard);

	// This callback is responsible for handling special keys (e.g: arrow keys)
	glutSpecialFunc(KeyboardSpecial);

	// This callback is responsible for handling mouse events
	glutMouseFunc(Mouse);

	// This callback is responsible for handling mouse motion
	glutMotionFunc(MouseMotion);

	// This callback is responsible for handling idle events (when no events are being received)
	glutIdleFunc(Idle);

	// This callback redraws the sync every 16ms (62.5fps) in order to fix issues where vsync is
	// disabled
	//glutTimerFunc(0, TimerSync, window.m_redisplay_time);

	// Now initialize our renderer settings (if any)
	// InitializeRenderer() is a function of Renderer.cpp which takes care of some one-off settings
	// before we go into the main rendering loop
	bool ok = Init();

	// We are done
	// Now enter main event handling loop
	if (ok) glutMainLoop();

	ReleaseGLUT(); // error occurs!!

	exit(EXIT_SUCCESS);
}  