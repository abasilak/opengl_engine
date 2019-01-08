//----------------------------------------------------//
//                                                    //
// File: Node.h										  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This is the basic node class						  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef NODE_H
#define NODE_H

#pragma once
//using namespace

// includes ////////////////////////////////////////


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

unsigned int						CreateNodeID(void);
unsigned int						GetCurrentID(void);

class Node
{
protected:
	// protected variable declarations
	Node*								m_parent;
	class Root*							m_root;
	std::string							m_name;

	// protected function declarations

private:
	// private variable declarations


	// private function declarations


public:
	// Constructor
	Node(const char* name);
		
	// Destructor
	virtual ~Node(void);

	// public function declarations
	virtual void						Init(void);
	virtual void						Update(void);
	virtual void						Draw(int shader_type);

	// get functions
	virtual	glm::mat4x4					GetTransform(void);
	virtual	glm::mat4x4					GetNormalMatrix(void);
	virtual glm::vec3					GetWorldPosition(void);

	virtual Node *						GetParent(void)								{return m_parent;}
	virtual class Root *				GetWorld(void)								{return m_root;}
	virtual const char*					GetName(void);

	// set functions
	virtual void						SetWorld(Root *w)							{m_root = w;}
	virtual void						SetParent(Node *p)							{m_parent = p;}
	virtual void						SetName(const char * str);
};

#endif //NODE_H

// eof ///////////////////////////////// class Node