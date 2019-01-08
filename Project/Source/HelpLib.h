//----------------------------------------------------//
//                                                    //
// File: HelpLib.h                                    //
// HelpLib contains some functions necessary for      //
// simplifying basic operations (writing to output    //
// window, basic OpenGL error checking, etc.)         //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

// Windows API and system calls
#include <windows.h>   
// C++ Standard Input and Output Library
#include <stdio.h>     
// C++ String
#include <string>     
// C++ Vector
#include <vector>     
// The OpenGL Extension Library
#include <gl/glew.h>
// The FREEGLUT Library for the user interface and windows 
#include <gl/freeglut.h>

//#define DEBUG_BUFFERS
//#define DEBUG_TEXT_RENDERING
//#define TONE_MAPPING

// ---------------- import GLM ---------------------------
#pragma warning( disable : 4244 )
#define GLM_PRECISION_MEDIUMP_FLOAT
#define GLM_FORCE_SSE2
//#define GLM_SWIZZLE // turned off: more compact data rep.  (...and more debugger-friendly)

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp> 

using namespace std;
using namespace glm;

#ifdef GLM_PRECISION_HIGHP_FLOAT
typedef glm::highp_float_t flt;
#else
typedef glm::mediump_float_t flt;
#endif

// basic definitions
#define PI			flt(3.14159265358979323846)
#define PI_2		flt(1.57079632679489661923)
#define PI_4		flt(0.78539816339744830961)
#define TWO_PI		flt(6.28318530717958647692)
#define FOUR_PI		flt(12.56637061435917295385)
#define DEG2RAD(_x) flt((_x) * 0.017453292519943295)		
#define RAD2DEG(_x) flt((_x) * 57.295779513082323)		

//----------------- declarations ------------------

#ifdef _DEBUG
#define DEBUG_CONTEXT
#endif // _DEBUG

// include GLEW libraries
#ifdef _DEBUG
#pragma comment (lib, "glew32d.lib")
#pragma comment (lib, "freetyped.lib")
#else // RELEASE
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "freetype.lib")
#endif // DEBUG OR RELEASE

// useful preprocessor defines
#define NULL_TERMINATED_CHAR  '\0'
#define NEW_LINE_CHAR		  '\n'
#define SAFE_FREE(_x)         { if ((_x) != nullptr) { free    ((_x)); _x = nullptr; } }
#define SAFE_DELETE(_x)       { if ((_x) != nullptr) { delete   (_x);  _x = nullptr; } }
#define SAFE_DELETE_ARRAY_POINTER(_x) { if ((_x) != nullptr) { delete[] (_x);  _x = nullptr; } }

#define SAFE_DELETE_ARRAY(_x, _s) for (int i = 0; i < _s; ++i) { \
	if ((_x[i]) != nullptr) { delete   (_x[i]);  _x[i] = nullptr; }}

// declarations for Library functions

// Print to VS output window
bool Print(const char * fmt, ...);

// Check for OpenGL errors
#ifdef _DEBUG
void									DebugGLOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam);
#endif // _DEBUG
bool glError(void);

// Check for FBO errors
bool glFrameBufferError(const char* str);

// read a file
bool ReadFile(std::string& filename, char** data);