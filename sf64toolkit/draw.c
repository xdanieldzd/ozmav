#include "globals.h"

void gl_Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void gl_LookAt(const GLdouble p_EyeX, const GLdouble p_EyeY, const GLdouble p_EyeZ, const GLdouble p_CenterX, const GLdouble p_CenterY, const GLdouble p_CenterZ)
{
	GLdouble l_X = p_EyeX - p_CenterX;
	GLdouble l_Y = p_EyeY - p_CenterY;
	GLdouble l_Z = p_EyeZ - p_CenterZ;

	if(l_X == l_Y && l_Y == l_Z && l_Z == 0.0f) return;

	if(l_X == l_Z && l_Z == 0.0f) {
		if(l_Y < 0.0f)
			glRotated(-90.0f, 1, 0, 0);
		else
			glRotated(90.0f, 1, 0, 0);
		glTranslated(-l_X, -l_Y, -l_Z);
		return;
	}

	GLdouble l_rX = 0.0f;
	GLdouble l_rY = 0.0f;

	GLdouble l_hA = (l_X == 0.0f) ? l_Z : hypot(l_X, l_Z);
	GLdouble l_hB;
	if(l_Z == 0.0f)
		l_hB = hypot(l_X, l_Y);
	else
		l_hB = (l_Y == 0.0f) ? l_hA : hypot(l_Y, l_hA);

	l_rX = asin(l_Y / l_hB) * (180 / M_PI);
	l_rY = asin(l_X / l_hA) * (180 / M_PI);

	glRotated(l_rX, 1, 0, 0);
	if(l_Z < 0.0f)
		l_rY += 180.0f;
	else
		l_rY = 360.0f - l_rY;

	glRotated(l_rY, 0, 1, 0);
	glTranslated(-p_EyeX, -p_EyeY, -p_EyeZ);
}

void gl_InitRenderer()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClearDepth(5.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	int i = 0;
	for(i = 0; i < 4; i++) {
		Gfx.LightAmbient[i] = 1.0f;
		Gfx.LightDiffuse[i] = 1.0f;
		Gfx.LightSpecular[i] = 1.0f;
		Gfx.LightPosition[i] = 1.0f;
	}

	glLightfv(GL_LIGHT0, GL_AMBIENT, Gfx.LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Gfx.LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Gfx.LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, Gfx.LightPosition);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void gl_InitExtensions()
{
	OpenGL.IsExtUnsupported = false;

	memset(OpenGL.ExtSupported, 0x00, sizeof(OpenGL.ExtSupported));

	OpenGL.ExtensionList = strdup((const char*)glGetString(GL_EXTENSIONS));
	int i;
	for(i = 0; i < strlen(OpenGL.ExtensionList); i++) {
		if(OpenGL.ExtensionList[i] == ' ') OpenGL.ExtensionList[i] = '\n';
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_texture_mirrored_repeat")) {
		OpenGL.Ext_TexMirroredRepeat = true;
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_texture_mirrored_repeat\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_TexMirroredRepeat = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_texture_mirrored_repeat\n", OpenGL.ExtUnsupported);
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_multitexture")) {
		OpenGL.Ext_MultiTexture = true;
        #ifdef WIN32
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB			= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
        #endif
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_multitexture\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_MultiTexture = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_multitexture\n", OpenGL.ExtUnsupported);
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_fragment_program")) {
		OpenGL.Ext_FragmentProgram = true;
		#ifdef WIN32
		glGenProgramsARB				= (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
		glBindProgramARB				= (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
		glDeleteProgramsARB				= (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");
		glProgramStringARB				= (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
		glProgramEnvParameter4fARB		= (PFNGLPROGRAMENVPARAMETER4FARBPROC) wglGetProcAddress("glProgramEnvParameter4fARB");
		glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) wglGetProcAddress("glProgramLocalParameter4fARB");
        #else
		glGenProgramsARB				= (PFNGLGENPROGRAMSARBPROC) glXGetProcAddressARB((const GLubyte*)"glGenProgramsARB");
		glBindProgramARB				= (PFNGLBINDPROGRAMARBPROC) glXGetProcAddressARB((const GLubyte*)"glBindProgramARB");
		glDeleteProgramsARB				= (PFNGLDELETEPROGRAMSARBPROC) glXGetProcAddressARB((const GLubyte*)"glDeleteProgramsARB");
		glProgramStringARB				= (PFNGLPROGRAMSTRINGARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramStringARB");
		glProgramEnvParameter4fARB		= (PFNGLPROGRAMENVPARAMETER4FARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramEnvParameter4fARB");
		glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramLocalParameter4fARB");
		#endif
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_fragment_program\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_FragmentProgram = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_fragment_program\n", OpenGL.ExtUnsupported);
	}
}

void gl_ResizeScene(int Width, int Height)
{
	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gl_Perspective(60.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void gl_DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gl_LookAt(Camera.X, Camera.Y, Camera.Z, Camera.X + Camera.LX, Camera.Y + Camera.LY, Camera.Z + Camera.LZ);

	glDisable(GL_TEXTURE_2D);
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-12.0f, -0.01f,-1000.0f);
		glVertex3f(-12.0f, -0.01f,   10.0f);
		glVertex3f( 12.0f, -0.01f,   10.0f);
		glVertex3f( 12.0f, -0.01f,-1000.0f);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	glScalef(0.004f, 0.004f, 0.004f);

	int ObjectNo = -1;
	while(ObjectNo++ < Viewer.ObjCount) glCallList(Gfx.GLListCount + ObjectNo);
}

void gl_UpdateStates()
{
	if(Gfx.ChangedModes & CHANGED_GEOMETRYMODE) {
		if(Gfx.GeometryMode & F3DEX_CULL_BOTH) {
			glEnable(GL_CULL_FACE);

			if(Gfx.GeometryMode & F3DEX_CULL_BACK)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		} else {
			glDisable(GL_CULL_FACE);
		}

		if((Gfx.GeometryMode & F3DEX_SHADING_SMOOTH) || !(Gfx.GeometryMode & G_TEXTURE_GEN_LINEAR)) {
			glShadeModel(GL_SMOOTH);
		} else {
			glShadeModel(GL_FLAT);
		}

		if(Gfx.GeometryMode & G_LIGHTING) {
			glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);
		} else {
			glDisable(GL_LIGHTING);
			glDisable(GL_NORMALIZE);
		}

		Gfx.ChangedModes &= ~CHANGED_GEOMETRYMODE;
	}
/*
	if(Gfx.GeometryMode & G_ZBUFFER)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
*/
	if(Gfx.ChangedModes & CHANGED_RENDERMODE) {
/*		if(Gfx.OtherModeL & Z_CMP)
			glDepthFunc(GL_LEQUAL);
		else
			glDepthFunc(GL_ALWAYS);
*/
/*		if((Gfx.OtherModeL & Z_UPD) && !(Gfx.OtherModeL & ZMODE_INTER && Gfx.OtherModeL & ZMODE_XLU))
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
*/
		if(Gfx.OtherModeL & ZMODE_DEC) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-3.0f, -3.0f);
		} else {
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if((Gfx.ChangedModes & CHANGED_ALPHACOMPARE) || (Gfx.ChangedModes & CHANGED_RENDERMODE)) {
		if(!(Gfx.OtherModeL & ALPHA_CVG_SEL)) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc((Gfx.BlendColor.A > 0.0f) ? GL_GEQUAL : GL_GREATER, Gfx.BlendColor.A);
		} else if(Gfx.OtherModeL & CVG_X_ALPHA) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.2f);
		} else
			glDisable(GL_ALPHA_TEST);
	}

	if(Gfx.ChangedModes & CHANGED_RENDERMODE) {
		if((Gfx.OtherModeL & FORCE_BL) && !(Gfx.OtherModeL & ALPHA_CVG_SEL)) {
			glEnable(GL_BLEND);

			switch(Gfx.OtherModeL >> 16) {
				case 0x0448: // Add
				case 0x055A:
					glBlendFunc(GL_ONE, GL_ONE);
					break;
				case 0x0C08: // 1080 Sky
				case 0x0F0A: // Used LOTS of places
					glBlendFunc(GL_ONE, GL_ZERO);
					break;
				case 0xC810: // Blends fog
				case 0xC811: // Blends fog
				case 0x0C18: // Standard interpolated blend
				case 0x0C19: // Used for antialiasing
				case 0x0050: // Standard interpolated blend
				case 0x0055: // Used for antialiasing
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case 0x0FA5: // Seems to be doing just blend color - maybe combiner can be used for this?
				case 0x5055: // Used in Paper Mario intro, I'm not sure if this is right...
					glBlendFunc(GL_ZERO, GL_ONE);
					break;

				default:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
			}
		} else {
			glDisable(GL_BLEND);
		}
	}
}

void gl_ClearRenderer(bool Full)
{
	if(Full) {
		if(glIsList(Gfx.GLListCount)) glDeleteLists(Gfx.GLListCount, Viewer.ObjCount);
	}

	if(Gfx.GLTextureID[0]) glDeleteTextures(Gfx.GLTextureCount, Gfx.GLTextureID);
	Gfx.GLTextureCount = 0;

	glGenTextures(2048, &Gfx.GLTextureID[0]);
}

int gl_FinishScene()
{
	#ifdef WIN32
	SwapBuffers(hDC); return EXIT_SUCCESS;
	#else
	glXSwapBuffers(dpy, win); return EXIT_SUCCESS;
	#endif
}
