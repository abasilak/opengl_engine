//----------------------------------------------------//
//                                                    //
// File: ShaderGLSL.h                                 //
// Handles basic GLSL functionality                   //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

#ifndef SHADERGLSL_H
#define SHADERGLSL_H

#pragma once
//using namespace


// includes ////////////////////////////////////////


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class ShaderGLSL
{
protected:
	// protected variable declarations
	std::string					m_name;
	GLuint						m_program;
	GLuint						m_vertex_shader;
	GLuint						m_geometry_shader;
	GLuint						m_fragment_shader;
	bool						m_has_geom_shader;
	bool						m_loaded;

	// protected function declarations


private:
	// private variable declarations
	void						Clear(void);
	void						Destroy(void);
	bool						LoadShader(std::string& /*error*/);
	void						UnloadShader(void);
	void						ProgramInfoLog(GLint linked);
	void						ShaderInfoLog(GLuint shader, GLint shader_type);
	bool						ReadShader(std::string& shader_path, std::string& name, std::string& error, std::string* vertex_source, std::string* geometry_source, std::string* fragment_source);

	// private function declarations

public:
	// Constructor
	ShaderGLSL(const char* name, bool has_geometry_shader = false);

	// Destructor
	~ShaderGLSL(void);

	// public function declarations
	bool								LoadAndCompile(void);
	unsigned int						GetProgram(void)								{return m_program;}
	bool								Loaded(void)									{return m_loaded;}
};

#endif //SHADERGLSL_H

// eof ///////////////////////////////// class ShaderGLSL