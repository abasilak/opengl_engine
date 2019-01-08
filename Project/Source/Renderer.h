#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include "TextRendering.h"

// This function is called before GLUT goes into its main loop.
bool Init(void);

// This function draws everything in the OpenGL window
void Render(void);

// When a resize event occurs, this function is called
void Resize(int width, int height);

// This function handles the presses of "character keys"
void Keyboard(unsigned char key, int x, int y);

// This function handles special keys, like arrow or function keys
void KeyboardSpecial(int key, int x, int y);

// This function handles mouse events
void Mouse(int button, int state, int x, int y);

// This function handles mouse motion
void MouseMotion(int x, int y);

// Release all memory allocated by pointers using new
// Also need to delete any gluQuadrics
void ReleaseGLUT(); 

// This function is called when no events are received
void Idle();

// This function redraws the scene every x ms
void TimerSync(int ms);

#endif //RENDERER_H

// eof ///////////////////////////////// 