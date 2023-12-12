#include <stdlib.h>
#include "esUtil.h"
#include "type_define.h"
#include "cam_mgr.h"
#include "tile_mgr.h"
#include "config.h"

typedef struct
{
	// Handle to a program object
	GLuint programObject;
	GLuint programColor;

	// Attribute locations
	GLint  positionLoc;
	GLint  texCoordLoc;
	GLint  uViewMatLoc;
	GLint  uProjMatLoc;

	// Attribute locations
	GLint  pos4colorLoc;
	GLint  uColorLoc;
	GLint  uViewMat4colorLoc;
	GLint  uProjMat4colorLoc;

	// Sampler locations
	GLint baseMapLoc;

	sCtrlParam ctrl;
	
	camManager* camMgr;
	tileManager* tileMgr;

} UserData;


//static float rangeList[] = {2,5,10,25,50,100,250,400,600,750,1000};
static float rangeList[] = { 2,5,10,25,50,100,250,400,600,750,1000 };
static short idx = 8;


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
	UserData *userData = (UserData*)esContext->userData;
	GLbyte vShaderStr[] =  
		"attribute vec4 a_position;                   \n"
		"attribute vec2 a_texCoord;                   \n"
		"varying vec2 v_texCoord;                     \n"
		"uniform mat4 viewMat;                        \n"
		"uniform mat4 projMat;                        \n"
		"void main()                                  \n"
		"{                                            \n"
		"   gl_Position = projMat*viewMat*a_position; \n"
		"   v_texCoord = a_texCoord;                  \n"
		"}                                            \n";

	GLbyte fShaderStr[] =  
		"precision mediump float;                            \n"
		"varying vec2 v_texCoord;                            \n"
		"uniform sampler2D texture;                          \n"
		"void main()                                         \n"
		"{                                                   \n"
		"  gl_FragColor = texture2D( texture, v_texCoord );  \n"
		"}                                                   \n";

	GLbyte vShaderStr_color[] =
		"attribute vec4 a_position;                   \n"
		"uniform mat4 viewMat;                        \n"
		"uniform mat4 projMat;                        \n"
		"void main()                                  \n"
		"{                                            \n"
		"   gl_Position = projMat*viewMat*a_position; \n"
		"}                                            \n";

	GLbyte fShaderStr_color[] =
		"precision mediump float;                            \n"
		"uniform vec4 ucolor;                                \n"
		"void main()                                         \n"
		"{                                                   \n"
		"  gl_FragColor = ucolor;                            \n"
		"}                                                   \n";

	// Load the shaders and get a linked program object
	userData->programObject = esLoadProgram ( (const char*)vShaderStr, (const char* )fShaderStr );
	userData->programColor = esLoadProgram ( (const char*)vShaderStr_color, (const char* )fShaderStr_color);

	// Get the attribute locations
	userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
	userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
	userData->uViewMatLoc = glGetUniformLocation ( userData->programObject, "viewMat" );
	userData->uProjMatLoc = glGetUniformLocation ( userData->programObject, "projMat" );

	// Get the sampler location
	userData->baseMapLoc = glGetUniformLocation ( userData->programObject, "texture" );

	// for color shader
	userData->pos4colorLoc = glGetAttribLocation(userData->programColor, "a_position");
	userData->uColorLoc    = glGetUniformLocation(userData->programColor, "ucolor");
	userData->uViewMat4colorLoc = glGetUniformLocation(userData->programColor, "viewMat");
	userData->uViewMat4colorLoc = glGetUniformLocation(userData->programColor, "projMat");

	glClearColor ( 0.0f, 0.0f, 0.5f, 0.0f );

	userData->ctrl.lon = 120.0;
	userData->ctrl.lat = 30.0;
	userData->ctrl.range = rangeList[idx]/108.0;

	int viewport[4] = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	userData->camMgr = new camManager(userData->ctrl,viewport);
	userData->tileMgr = new tileManager();


	//test display
	tileId id0(0, 0, 0);
	tileId id1(0, 1, 0);
	tileId id2(0, 2, 0);
	tileId id3(0, 3, 0);
	tileId id4(0, 0, 1);
	tileId id5(0, 1, 1);
	tileId id6(0, 2, 1);
	tileId id7(0, 3, 1);
	userData->tileMgr->addTile(id0);
	userData->tileMgr->addTile(id1);
	userData->tileMgr->addTile(id2);
	userData->tileMgr->addTile(id3);
	userData->tileMgr->addTile(id4);
	userData->tileMgr->addTile(id5);
	userData->tileMgr->addTile(id6);
	userData->tileMgr->addTile(id7);

	return TRUE;
}


//void (ESCALLBACK* keyFunc) (ESContext*, unsigned char, int, int)
void processKey(ESContext *esContext, unsigned char c, int curX, int curY)
{

	UserData* userData = (UserData*)esContext->userData;

	switch (c)
	{
	case 'a':
		userData->ctrl.lon += userData->ctrl.range / 2.0;
		if (userData->ctrl.lon > 180)
			userData->ctrl.lon -= 360.0;
		break;
	case 'd':
		userData->ctrl.lon -= userData->ctrl.range / 2.0;
		if (userData->ctrl.lon < -180)
			userData->ctrl.lon += 360.0;
		break;
	case 'w':
		userData->ctrl.lat -= userData->ctrl.range / 2.0;
		if (userData->ctrl.lat < -90)
			userData->ctrl.lat = -90.0;
		break;
	case 's':
		userData->ctrl.lat += userData->ctrl.range / 2.0;
		if (userData->ctrl.lat > 90)
			userData->ctrl.lat = 90.0;
		break;
	case 'k':
		idx++;
		if (idx >= sizeof(rangeList) / sizeof(rangeList[0]))
			idx = sizeof(rangeList) / sizeof(rangeList[0]) - 1;
		userData->ctrl.range = rangeList[idx];
		break;
	case 'l':
		idx--;
		if (idx <= 0)
			idx == 0;
		userData->ctrl.range = rangeList[idx];
		break;
	default:
		break;
	}

	userData->ctrl.range;

	return;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
	UserData *userData = (UserData * )esContext->userData;

	// Set the viewport
	int* vp = userData->camMgr->getViewport();
	glViewport (vp[0], vp[1], vp[2], vp[3]);

	userData->camMgr->updateCamera();
	Mat4f* pViewMat = userData->camMgr->getViewMat();
	glUniformMatrix4fv(userData->uViewMatLoc, 1, GL_FALSE, &(pViewMat[0].col[0].x));

	userData->camMgr->updateProjMat(userData->ctrl);
	Mat4f* pProjMat = userData->camMgr->getProjMat();
	glUniformMatrix4fv(userData->uProjMatLoc, 1, GL_FALSE, &(pProjMat[0].col[0].x));



	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	glUseProgram ( userData->programObject );

	glEnableVertexAttribArray ( userData->positionLoc );
	glEnableVertexAttribArray ( userData->texCoordLoc );
#if 1
	glDisable(GL_CULL_FACE);
#else
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
#endif
	auto& display_list = userData->tileMgr->tileList;
	for (auto& idx : display_list) {

		auto tile =userData->tileMgr->getTile(idx);

		RendererEle& renderEle = userData->tileMgr->getRenderEle(tile.renderIdx);
		if (renderEle.state != eRenderReady) {
			userData->tileMgr->UpdateRenderEle(tile);
			continue;
		}


		glBindTexture(GL_TEXTURE_2D, renderEle.texId);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(userData->baseMapLoc, 0);

		glBindBuffer(GL_ARRAY_BUFFER, renderEle.vbo);
		glVertexAttribPointer(userData->positionLoc, 2, GL_FLOAT,
			GL_FALSE, 0, 0);
		// Load the texture coordinate
		glBindBuffer(GL_ARRAY_BUFFER, userData->tileMgr->tbo);
		glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT,
			GL_FALSE,0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->tileMgr->ibo);
	    glDrawElements (GL_TRIANGLE_STRIP, TILE_I_NUM, GL_UNSIGNED_SHORT, NULL);
	    
	    glDrawElements ( GL_LINE_STRIP, TILE_I_NUM, GL_UNSIGNED_SHORT, NULL);
	}
#if 0
	glBindTexture(GL_TEXTURE_2D, userData->tileMgr->gTexId);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(userData->baseMapLoc, 0);

	glBindBuffer(GL_ARRAY_BUFFER, userData->tileMgr->gVBO);
	glVertexAttribPointer(userData->positionLoc, 2, GL_FLOAT,
		GL_FALSE, 0, 0);
	// Load the texture coordinate
	glBindBuffer(GL_ARRAY_BUFFER, userData->tileMgr->gTBO);
	glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT,
		GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->tileMgr->gIBO);
	glDrawElements(GL_TRIANGLE_STRIP, TILE_I_NUM, GL_UNSIGNED_SHORT, NULL);
#endif
#if 0
	// Use the program object
	glUseProgram(userData->programColor);

	glUniformMatrix4fv(userData->uViewMat4colorLoc, 1, GL_FALSE, &(pViewMat[0].col[0].x));
	glUniformMatrix4fv(userData->uViewMat4colorLoc, 1, GL_FALSE, &(pViewMat[0].col[0].x));
	GLfloat u_color[4] = {1.0,0.0,0.0,1.0};
	glUniform4fv(userData->uColorLoc, 1, u_color);

	glEnableVertexAttribArray(userData->pos4colorLoc);

	for (auto& idx : display_list) {

		auto tile = userData->tileMgr->getTile(idx);

		RendererEle& renderEle = userData->tileMgr->getRenderEle(tile.renderIdx);
		if (renderEle.state != eRenderReady) {
			//userData->tileMgr->UpdateRenderEle(tile);
			continue;
		}

		glBindBuffer(GL_ARRAY_BUFFER, renderEle.vbo);
		glVertexAttribPointer(userData->pos4colorLoc, 2, GL_FLOAT,
			GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->tileMgr->ibo);

		glDrawElements(GL_LINE_STRIP, TILE_I_NUM, GL_UNSIGNED_SHORT, NULL);
	}
#endif


	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}


///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
	UserData *userData = (UserData * )esContext->userData;

	// Delete texture object

	// Delete program object
	glDeleteProgram ( userData->programObject );
}

int main ( int argc, char *argv[] )
{
	ESContext esContext;
	UserData  userData;

	esInitContext ( &esContext );
	esContext.userData = &userData;

	esCreateWindow ( &esContext, "amap", 1080, 720, ES_WINDOW_RGB );

	if ( !Init ( &esContext ) )
		return 0;

	esRegisterDrawFunc ( &esContext, Draw );

	esRegisterKeyFunc( &esContext, processKey);

	esMainLoop ( &esContext );

	ShutDown ( &esContext );
}