#pragma once

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void Resize(int, int);
protected:
	glm::vec3 GetUV(float u, float v);

	GLuint m_programID; 
	GLuint m_vaoID; 
	GLuint m_vboID; 
	GLuint m_ibID;  

	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	GLuint	m_loc_mvp; 

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 c;
	};

	static const int N	= 6;
	static const int M	= 10;
	static const int numSteps = N;
	bool pressed = false;

	
};

