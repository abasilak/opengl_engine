//----------------------------------------------------//
//                                                    //
// File: HelpLib.cpp                                  //
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

#include "HelpLib.h" // - Header file for the HelpLib

#ifdef _DEBUG
void CALLBACK DebugGLOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam)
{
#ifndef DEBUG_CONTEXT
	return;
#else
	if (id == 2 || id == 102 || id == 131185 || id == 131186 || id == 131218 || id == 131154)
		return;

	char debSource[16], debType[30], debSev[20];
	if (source == GL_DEBUG_SOURCE_API_ARB)
		strcpy_s(debSource, 16, "OpenGL");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		strcpy_s(debSource, 16, "Windows");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		strcpy_s(debSource, 16, "Shader Compiler");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		strcpy_s(debSource, 16, "Third Party");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		strcpy_s(debSource, 16, "Application");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		strcpy_s(debSource, 16, "Other");
	else
		strcpy_s(debSource, 16, "Undefined");

	if (type == GL_DEBUG_TYPE_ERROR_ARB)
		strcpy_s(debType, 30, "Error");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
		strcpy_s(debType, 30, "Deprecated behavior");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		strcpy_s(debType, 30, "Undefined behavior");
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
		strcpy_s(debType, 30, "Portability");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
		strcpy_s(debType, 30, "Performance");
	else if (type == GL_DEBUG_TYPE_MARKER)
		strcpy_s(debType, 30, "Command stream annotation");
	else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy_s(debType, 30, "Group Pushing");
	else if (type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy_s(debType, 30, "Group Poping");
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)
		strcpy_s(debType, 30, "Other");
	else
		strcpy_s(debType, 30, "Undefined");

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		strcpy_s(debSev, 20, "High");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		strcpy_s(debSev, 20, "Medium");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		strcpy_s(debSev, 20, "Low");
	else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		strcpy_s(debSev, 20, "Notification");
	else
		strcpy_s(debSev, 20, "Undefined");

	Print("Source:%s, Type:%s, ID:%d, Severity:%s, Message:%s", debSource, debType, id, debSev, message);
#endif // DEBUG_CONTEXT
}

bool glError(void)
{
	// already handled by the debug extension
#if defined(DEBUG_CONTEXT)
	if (GLEW_ARB_debug_output)
		return false;
#endif

	uint32 _enum = glGetError();
	if (_enum != GL_NO_ERROR)
	{
		std::string er = "";
		switch (_enum)
		{
		case 0x500:		er = "GL_INVALID_ENUM 0x0500\nAn unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag."; break;
		case 0x501:		er = "GL_INVALID_VALUE 0x0501\nA numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."; break;
		case 0x502:		er = "GL_INVALID_OPERATION 0x0502\nThe specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."; break;
		case 0x503:		er = "GL_STACK_OVERFLOW 0x0503\nThis command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag."; break;
		case 0x504:		er = "GL_STACK_UNDERFLOW 0x0504\nThis command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag."; break;
		case 0x505:		er = "GL_OUT_OF_MEMORY 0x0505\nThere is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."; break;
		case 0x8031:	er = "GL_TABLE_TOO_LARGE 0x8031\nThe specified table exceeds the implementation's maximum supported table size.  The offending command is ignored and has no other side effect than to set the error flag."; break;
		default:		er = "Undefined GL error"; break;
		};

		Print("%s", er.c_str());
		return true;
	}
	return false;
}

#else // RELEASE
bool glError(void)
{
	return false;
}
#endif


bool		Print(const char * fmt, ...)
{
	char* text;									// Holds Our String
	int len;							
	va_list args;								// Pointer To List Of Arguments

	if (fmt == NULL)							// If There's No Text
		return false;							// Do Nothing

	va_start(args, fmt);						// Parses The String For Variables
	len = _vscprintf(fmt, args) + 2;			// _vscprintf doesn't count
	text = (char*)malloc(len * sizeof(char));
	vsprintf_s(text, len, fmt, args);			// And Converts Symbols To Actual Numbers
	va_end(args);								// Results Are Stored In Text

	text[len - 2] = NEW_LINE_CHAR; 
	text[len - 1] = NULL_TERMINATED_CHAR; 
	OutputDebugStringA(LPCSTR(text));

	SAFE_DELETE(text)

	return true;
}

#ifdef _DEBUG
std::string GetFrameBufferErrorStr(GLenum status)
{
	switch (status)
	{
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";
		break;
	default:
		return "Undefined Error";
	};
}

bool glFrameBufferError(const char* str)
{
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
	{
		Print("%s, error: %s", str, GetFrameBufferErrorStr(err).c_str());
		return true;
	}
	return false;
}
#else
bool glFrameBufferError(const char* str)
{
	return false;
}
#endif

bool		ReadFile(std::string& filename, char** data)
{
	FILE* File = NULL;
	fopen_s(&File, filename.c_str(), "r");
	if (File != NULL)
	{
		size_t count = 0;
		fseek(File, 0, SEEK_END);
		count = ftell(File);
		rewind(File);
		if (count > 0) 
		{
			*data = new char[count + 1];
			memset(*data, 0, count);
			count = fread(*data, sizeof(char), count, File);
			(*data)[count] = NULL_TERMINATED_CHAR;
			fclose(File);
			return true;
		}
		fclose(File);
		return false;
	}
	return false;
}