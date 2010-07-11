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

	gl_LookAt(zCamera.X, zCamera.Y, zCamera.Z, zCamera.X + zCamera.LX, zCamera.Y + zCamera.LY, zCamera.Z + zCamera.LZ);

	glScalef(0.005, 0.005, 0.005);

	int Maps = 0, DL = 0, Door = 0, Actor = 0;

	int StartMap = 0; int EndMap = zSHeader[0].MapCount;
	if(zOptions.MapToRender != -1) {
		StartMap = zOptions.MapToRender;
		EndMap = StartMap + 1;
	}

	for(Maps = StartMap; Maps < EndMap; Maps++) {
		for(DL = 0; DL < zGfx.DLCount[Maps]; DL++) {
			glCallList(zGfx.GLListCount[Maps] + DL);
		}

		for(Actor = 0; Actor < zMHeader[0][Maps].ActorCount; Actor++) {
			for(DL = 0; DL < zGfx.ActorDLCount[Maps][Actor]; DL++) {
				glCallList(zGfx.ActorGLListCount[Maps][Actor] + DL);
			}
		}
	}

	for(Door = 0; Door < zSHeader[0].DoorCount; Door++) {
		for(DL = 0; DL < zGfx.DoorDLCount[Door]; DL++) {
			glCallList(zGfx.DoorGLListCount[Door] + DL);
		}
	}
}

void gl_ClearDisplayLists()
{
	int i = 0, j = 0;
	for(i = 0; i < 256; i++) {
		// maps
		if(glIsList(zGfx.GLListCount[i])) glDeleteLists(zGfx.GLListCount[i], zGfx.DLCount[i]);
		zGfx.DLCount[i] = 0;

		// doors
		if(glIsList(zGfx.DoorGLListCount[i])) glDeleteLists(zGfx.DoorGLListCount[i], zGfx.DoorDLCount[i]);
		zGfx.DoorDLCount[i] = 0;

		// actors
		for(j = 0; j < 1024; j++) {
			if(glIsList(zGfx.ActorGLListCount[i][j])) glDeleteLists(zGfx.ActorGLListCount[i][j], zGfx.ActorDLCount[i][j]);
			zGfx.ActorDLCount[i][j] = 0;
		}
	}
}

int gl_FinishScene()
{
	#ifdef WIN32
	SwapBuffers(hDC); return EXIT_SUCCESS;
	#else
	glXSwapBuffers(dpy, win); return EXIT_SUCCESS;
	#endif
}
