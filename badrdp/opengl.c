#include "globals.h"

void RDP_SetupOpenGL()
{
	RDP_InitGLExtensions();

	RDP_ResetOpenGL();
}

void RDP_ResetOpenGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClearDepth(5.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

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
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void RDP_InitGLExtensions()
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

void RDP_ClearTextures()
{
	if(Gfx.GLTextureID[0]) glDeleteTextures(Gfx.GLTextureCount, Gfx.GLTextureID);
	Gfx.GLTextureCount = 0;

	glGenTextures(CACHE_TEXTURES, &Gfx.GLTextureID[0]);
}

void RDP_UpdateGLStates()
{
	if(Gfx.Update & CHANGED_GEOMETRYMODE) {
		if(Gfx.GeometryMode & G_CULL_BOTH) {
			glEnable(GL_CULL_FACE);

			if(Gfx.GeometryMode & G_CULL_BACK)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		} else {
			glDisable(GL_CULL_FACE);
		}

		if((Gfx.GeometryMode & G_SHADING_SMOOTH) || !(Gfx.GeometryMode & G_TEXTURE_GEN_LINEAR)) {
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

		Gfx.Update &= ~CHANGED_GEOMETRYMODE;
	}
/*
	if(Gfx.GeometryMode & G_ZBUFFER) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
*/
	if(Gfx.Update & CHANGED_RENDERMODE) {
/*		if(Gfx.OtherModeL & Z_CMP) {
			glDepthFunc(GL_LEQUAL);
		} else {
			glDepthFunc(GL_ALWAYS);
		}*/
/*
		if(Gfx.OtherModeL & Z_UPD) {
			glDepthMask(GL_TRUE);
		} else {
			glDepthMask(GL_FALSE);
		}
*/
		if(Gfx.OtherModeL & ZMODE_DEC) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-3.0f, -3.0f);
		} else {
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if((Gfx.Update & CHANGED_ALPHACOMPARE) || (Gfx.Update & CHANGED_RENDERMODE)) {
		if(!(Gfx.OtherModeL & ALPHA_CVG_SEL)) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc((Gfx.BlendColor.A > 0.0f) ? GL_GEQUAL : GL_GREATER, Gfx.BlendColor.A);

		} else if(Gfx.OtherModeL & CVG_X_ALPHA) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.5f);

		} else
			glDisable(GL_ALPHA_TEST);
	}

	if(Gfx.Update & CHANGED_RENDERMODE) {
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

		Gfx.Update &= ~CHANGED_RENDERMODE;
	}
}

bool RDP_OpenGL_ExtFragmentProgram()
{
	return OpenGL.Ext_FragmentProgram;
}
