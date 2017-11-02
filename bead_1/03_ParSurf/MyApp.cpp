#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_ibID = 0;
	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

glm::vec3 CMyApp::GetUV(float u, float v)
{
	u *= 2 * 3.1415f;
	float h = 3.0f;  // h a magasság
	v = (v - 0.5f) * h;
	float cu = cosf(u), su = sinf(u);
	float r = 1; // r a sugár

	return glm::vec3(r*cu, -v, r*su);
}

bool CMyApp::Init()
{
	//saját márkanáciság ellenörzõ :)
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);

	std::cout << "\nGraphic is running on: " << vendor << " - " << renderer << "\n";

	glClearColor(0, 0, 0, 1.0f);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK); 

	Vertex vert[2 * N +(N+1)*(M+1)];

	vert[0].p = glm::vec3(-0.125, 0.5, 0);
	vert[0].c = glm::vec3(0, 0, 0);
	for (int i = 0; i < N-1; ++i) {
		vert[i + 1].p = glm::vec3(cos(2 * M_PI / 20.0*i)*0.04 - 0.125, sin(2 * M_PI / 20.0*i)*0.04 + 0.5, 0);
		vert[i + 1].c = glm::vec3(0, 0, 0);
	}

	vert[N].p = glm::vec3(0.125, 0.5, 0);
	vert[N].c = glm::vec3(0, 0, 0);
	for (int i = N+1; i < 2 * N; ++i) {
		vert[i].p = glm::vec3(cos(2 * M_PI / 20.0*i)*0.04 + 0.125, sin(2 * M_PI / 20.0*i)*0.04 + 0.5, 0);
		vert[i].c = glm::vec3(0, 0, 0);
	}

	for (int i=0; i<=N; ++i)
		for (int j=0; j<=M; ++j)
		{
			float u = i/(float)N;
			float v = j/(float)M;

			vert[i + j*(N+1)].p = GetUV(u, v);
			vert[i + j*(N+1)].c = glm::normalize( vert[i + j*(N+1)].p ) / 4.0f + 0.5f;
		}

    GLushort indices[3*2*(N)*(M)];
	for (int i=0; i<N; ++i)
		for (int j=0; j<M; ++j)
		{
			indices[6*i + j*3*2*(N) + 0] = (i)		+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 1] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 2] = (i)		+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 3] = (i+1)	+ (j)*	(N+1);
			indices[6*i + j*3*2*(N) + 4] = (i+1)	+ (j+1)*(N+1);
			indices[6*i + j*3*2*(N) + 5] = (i)		+ (j+1)*(N+1);
		}


	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	
	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData( GL_ARRAY_BUFFER,	
				  sizeof(vert),		
				  vert,	
				  GL_STATIC_DRAW);	
	

	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(
		0,				
		3,				
		GL_FLOAT,		
		GL_FALSE,		
		sizeof(Vertex),	
		0				
	); 

	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(sizeof(glm::vec3)) );

	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 

	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	glBindAttribLocation(	m_programID,	
							0,				
							"vs_in_pos");	
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	glLinkProgram(m_programID);

	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );

	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	m_loc_mvp = glGetUniformLocation( m_programID, "MVP");

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_ibID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	m_matView = glm::lookAt(glm::vec3(0,  0,  30), // kamera pozicioja 
							glm::vec3( 0,  0,  0),	 // hova nezunk	
							glm::vec3( 0,  1,  0));	 // felfele irany
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram( m_programID );

	glBindVertexArray(m_vaoID);


	//Ehhez, a fragment shadernek átadunk egy idõbõl kiszámolt paramétert(
	// 2 * 3.14152f										-> egy teljes periódus (radiánban 2*PI)
	// 2 * 3.14152f * SDL_GetTicks()					-> mp-ként 1000 periódus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f			-> mp-ként 1 periódus
	// 2 * 3.14152f * SDL_GetTicks() / 1000.0f / 6.0f	-> mp-ként 1/13 periódus, azaz 13 mp-ként 1 periódus


	//Az így kirajzolt formációt mozgasd a következõ pályán: egy origó középpontú, 9 sugarú gömb felületén (ne síkbeli legyen a pálya!) (25%)
	//float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float u = 2 * 3.1415f;
	float v = 3.1415f;
	float r = 9;

	float moveSpeedu = sinf(2 * 3.14152f * SDL_GetTicks() / 1000.0f / 8.0f);
	float moveSpeedv = cosf(3.14152f * SDL_GetTicks() / 1000.0f / 8.0f);

	float x = r * sinf(moveSpeedu) * cosf(moveSpeedv);
	float y = r * sinf(moveSpeedu) * sinf(moveSpeedv);
	float z = r * cosf(moveSpeedu);
	
	glm::mat4 move = glm::translate<float>(glm::vec3(x, y, z));

	glm::mat4 mvp;

	glm::mat4 scale = glm::scale<float>(glm::vec3(1.0f, 1.0f, 1.0f));

	/*A felhasználó a következõképpen állíthassa a színtérbeli történéseket : a szóköz billentyû lenyomására kezdjenek el forogni a formációban résztvevõ geometriák a saját X tengelyük körül
	(tehát ne az egész alakzat, hanem külön - külön a geometriák).Újabb szóköz billentyû esetén álljon meg ez a forgás..
	A kamerát a színtérbe úgy helyezd le, hogy látható legyen minden!(25 % )*/

	float rot = 360 * SDL_GetTicks() / 1000.0f / 200.0f;

	if (pressed)
	{											   //x, y, z
		scale = glm::rotate<float>(rot, glm::vec3(1.0f, 0, 0)); // az "x" körül forog // d) részfeladat
	}
	else
	{
		scale = glm::scale<float>(glm::vec3(1.0f, 1.0f, 1.0f));
	}

	//Készíts egy alkalmazást amiben létrehozol egy egy XZ síkban fekvõ, 1 sugarú kör alapú, 3 egység magas hengert.A létrehozott geometria minden háromszögének legyen különbözõ színe!(25 % )
	//6 henger kipakolása
	//sorozat ami kell (in any order): -1,0,0 || 1,0,0 || 0,1,0, || 0,-1,0 || 0,0,-1 || 0,0,1

	for (int i = -1; i < 2; i++) //x tengely x,0,0 || -x,0,0
	{
		for (int j = -1; j < 2; j++) //y tengely 0,y,0 || 0,-y,0
		{
			for (int k = -1; k < 2; k++) // z tengely 0,0,z || 0,0,-z
			{
				//if (i == - 1 && j == 0 && k == 0 || i == 1 && j == 0 && k == 0 || i == 0 && j == -1 && k == 0 || i == 0 && j == 1 && k == 0 || i == 0 && j == 0 && k == -1 || i == 0 && j == 0 && k == 1)
				if (i == 0 && j == 0 && k != 0 || i == 0 && k == 0 && j != 0 || j == 0 && k == 0 && i != 0)
				{
					//std::cout << "i: " << i << "j: " << j << "k: " << k << "\n";
					//azért sqrt 2 mert a 2 élhosszú oktaéder magassága gyök 2 és ezért ide kell kirakni a hengereket (a csúcspontok = magasságpontok)
					glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

					m_matWorld = move * glm::translate<float>(glm::vec3(i * sqrt(2), j * sqrt(2), k * sqrt(2))) * scale;

					glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

					mvp = m_matProj * m_matView * m_matWorld;

					glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

					glDrawElements(GL_TRIANGLES, 3 * 2 * (N)*(M), GL_UNSIGNED_SHORT, 0);
				}
			}
		}
	}

	glBindVertexArray(0);

	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	(key.keysym.sym == SDLK_SPACE && pressed == false) ? pressed = true : pressed = false;
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective(  45.0f, _w/(float)_h, 0.01f, 100.0f); 
}