/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	ogl_mngr.c - OGL + ext. initialization, etc.
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

/* INITGL - INITIALIZE OPENGL RENDERING SYSTEM */
int InitGL(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);

	glEnable(GL_ALPHA_TEST);

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 0.15f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 75.0f);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	InitGLExtensions();

	return true;
}

int InitGLExtensions(void)
{
	GLExtension_List = strdup(glGetString(GL_EXTENSIONS));
	int ExtListLen = strlen(GLExtension_List);
	int i;
	for(i = 0; i < ExtListLen; i++) {
		if(GLExtension_List[i] == ' ') GLExtension_List[i] = '\n';
	}

	if(strstr(GLExtension_List, "GL_ARB_multitexture")) {
		GLExtension_MultiTexture = true;
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB			= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_ARB_multitexture\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "GL_ARB_texture_mirrored_repeat")) {
		GLExtension_TextureMirror = true;
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsErrorMsg);
	}

	if(strstr(GLExtension_List, "GL_EXT_texture_filter_anisotropic")) {
		GLExtension_AnisoFilter = true;
	} else {
		GLExtensionsUnsupported = true;
		sprintf(GLExtensionsErrorMsg, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsErrorMsg);
	}

	if(GLExtensionsUnsupported) {
		sprintf(ErrorMsg, "The following OpenGL Extensions are not supported by your hardware:\n\n%s", GLExtensionsErrorMsg);
		MessageBox(hwnd, ErrorMsg, "Extension Error", MB_OK | MB_ICONERROR);
	}

	return 0;
}

/* DRAWGLSCENE - DRAW THE CURRENT SCENE USING THE MAP AND ACTOR DATA GATHERED BEFORE */
int DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int i = 0;

	if(AreaLoaded) {
		HelperFunc_CalculateFPS();

		sprintf(Renderer_CoordDisp, "Cam X: %4.2f, Y: %4.2f, Z: %4.2f", CamX, CamY, CamZ);

		glLoadIdentity();

		gluLookAt(CamX, CamY, CamZ,
				CamX + CamLX, CamY + CamLY, CamZ + CamLZ,
				0.0f, 1.0f, 0.0f);

		glScalef(0.005, 0.005, 0.005);

		/* #0 - GET COLLISION GL DLIST */
		unsigned int GLCollisionDList = Renderer_GLDisplayList;

		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			int j = 0;
			for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
				//do nothing, just simulating stuff
			}
			GLCollisionDList += j;
		}

		/* #1 - RENDER ACTORS, PASS 1 */
		glDisable(GL_BLEND);
		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			ActorInfo_CurrentCount[i] = 0;
		}
		ScActorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();

		/* #2 - RENDER MAP */
		Renderer_GLDisplayList_Current = Renderer_GLDisplayList;

		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			int j = 0;
			for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
				if(Renderer_EnableMap) glCallList(Renderer_GLDisplayList_Current + j);
			}
			Renderer_GLDisplayList_Current += j;
		}

		/* #3 - RENDER COLLISION */
		if(Renderer_EnableCollision) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, 0.0f);

			glEnable(GL_BLEND);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if(Renderer_EnableMap) { Renderer_CollisionAlpha = 0.3f; } else { Renderer_CollisionAlpha = 0.1f; }
			glCallList(GLCollisionDList);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glLineWidth(8.5f);
			glCallList(GLCollisionDList);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_BLEND);
		}

		/* #4 - RENDER ACTORS, PASS 2 */
		glDisable(GL_BLEND);
		for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
			ActorInfo_CurrentCount[i] = 0;
		}
		ScActorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();
	}

	return true;
}

/* RESIZEGLSCENE - RESIZES THE OPENGL RENDERING TARGET ALONG WITH THE MAIN WINDOW */
void ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0) height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* KILLGLTARGET - DESTROYS THE OPENGL RENDERING TARGET FOR PROPER EXITING */
void KillGLTarget(void)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, "Release of DC and RC failed!", "Error", MB_OK | MB_ICONERROR);
		if (!wglDeleteContext(hRC)) MessageBox(NULL, "Release of Rendering Context failed!", "Error", MB_OK | MB_ICONERROR);
		hRC = NULL;
	}

	if (hDC_ogl && !ReleaseDC(hwnd, hDC_ogl))
	{
		MessageBox(NULL, "Release of Device Context failed!", "Error", MB_OK | MB_ICONERROR);
		hDC_ogl = NULL;
	}
}

/* CREATEGLTARGET - CREATE AN OPENGL RENDERING TARGET WITH THE SPECIFIED PARAMETERS */
BOOL CreateGLTarget(int width, int height, int bits)
{
	GLuint PixelFormat;

	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	pfd.cColorBits = bits;

	if (!(hDC_ogl = GetDC(hogl)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't create OpenGL Device Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC_ogl, &pfd)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't find suitable PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!SetPixelFormat(hDC_ogl, PixelFormat, &pfd))
	{
		KillGLTarget();
		MessageBox(NULL,"Can't set PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC_ogl)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't create OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!wglMakeCurrent(hDC_ogl, hRC))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't activate OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!InitGL())
	{
		KillGLTarget();
		MessageBox(NULL, "Initialization failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

/*	------------------------------------------------------------ */
