#include "globals.h"

#define GRID_SIZE	10.0f

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

void gl_SetupScene2D(int Width, int Height)
{
	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Width, Height, 0, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);

	RDP_SetOpenGLDimensions(Width, Height);
}

void gl_SetupScene3D(int Width, int Height)
{
	float TempMatrix[4][4];

	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gl_Perspective(60.0f, (GLfloat)Width / (GLfloat)Height, 0.001f, 10000.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, *TempMatrix);
	RDP_Matrix_ProjectionLoad(TempMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, *TempMatrix);
	RDP_Matrix_ModelviewLoad(TempMatrix);

	glEnable(GL_DEPTH_TEST);
}

void gl_CreateSceneDLists()
{
	dbgprintf(0, MSK_COLORTYPE_OKAY, "Now executing Display Lists... please wait.");

	int i;
	for(i = 0; i < zProgram.DListCount + 1; i++) {
		if(RDP_CheckAddressValidity(zProgram.DListAddr[i])) {
			// delete list
			if(glIsList(zProgram.DListGL[i])) glDeleteLists(zProgram.DListGL[i], 1);
			// generate list
			zProgram.DListGL[i] = glGenLists(1);
			// fill list
			glNewList(zProgram.DListGL[i], GL_COMPILE_AND_EXECUTE);
				RDP_SetCycleType(1);
				RDP_ParseDisplayList(zProgram.DListAddr[i], true);
			glEndList();
		}
	}
}

void gl_DrawScene()
{
	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);
	gl_LookAt(zCamera.X, zCamera.Y, zCamera.Z, zCamera.X + zCamera.LX, zCamera.Y + zCamera.LY, zCamera.Z + zCamera.LZ);

	if(zOptions.EnableGrid) {
		if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
		glCallList(zProgram.GLGrid);
	}

	glScalef(zProgram.ScaleFactor, zProgram.ScaleFactor, zProgram.ScaleFactor);

	if(zProgram.DListSel == -1) {
		int i;
		for(i = 0; i < zProgram.DListCount + 1; i++) {
			if(glIsList(zProgram.DListGL[i])) glCallList(zProgram.DListGL[i]);
			//if(RDP_CheckAddressValidity(zProgram.DListAddr[i])) RDP_ParseDisplayList(zProgram.DListAddr[i], true);
		}
	} else {
		if(glIsList(zProgram.DListGL[zProgram.DListSel])) glCallList(zProgram.DListGL[zProgram.DListSel]);
/*		if(RDP_CheckAddressValidity(zProgram.DListAddr[zProgram.DListSel]))
			RDP_ParseDisplayList(zProgram.DListAddr[zProgram.DListSel], true);*/
	}

	gl_DrawHUD();
}

static inline GLfloat gl_GetPointDistance(__Vect3D First, __Vect3D Second)
{
	GLfloat DeltaX = (float)Second.X - (float)First.X;
	GLfloat DeltaY = (float)Second.Y - (float)First.Y;
	GLfloat DeltaZ = (float)Second.Z - (float)First.Z;
	return sqrtf(DeltaX * DeltaX + DeltaY * DeltaY + DeltaZ * DeltaZ);
};

void gl_DrawHUD()
{
	if(!zOptions.EnableHUD) return;

	gl_SetupScene2D(zProgram.WindowWidth, zProgram.WindowHeight);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(RDRAM.IsSet == true) {
		hud_Print(
			0, -1, zProgram.WindowWidth, -1, 1, 1.0f,
			"Display List: ignored");
	} else if(zProgram.DListSel == -1) {
		hud_Print(
			0, -1, zProgram.WindowWidth, -1, 1, 1.0f,
			"Display List: all");
	} else {
		hud_Print(
			0, -1, zProgram.WindowWidth, -1, 1, 1.0f,
			"Display List: %08X",
			zProgram.DListAddr[zProgram.DListSel]);
	}

	if(RDRAM.IsSet == false) {
		hud_Print(
			130, -1, 0, -1, 1, 1.0f,
			"Scale: %4f",
			zProgram.ScaleFactor);
	} else {
		hud_Print(
			130, -1, 0, -1, 1, 1.0f,
			"Scale: ignored");
	}
/*
	hud_Print(
		230, -1, 0, -1, 1, 1.0f,
		"%4f",
		zCamera.CamSpeed);
*/
	hud_Print(
		zProgram.WindowWidth - 45, -1, 0, -1, 1, 1.0f,
		"FPS: %i",
		zProgram.LastFPS);
}

void gl_CreateViewerDLists()
{
	zProgram.GLAxisMarker = glGenLists(1);
	zProgram.GLGrid = glGenLists(1);

	glNewList(zProgram.GLAxisMarker, GL_COMPILE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
		glLineWidth(2);

		glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(420.0f, 0.1f, 0.1f);
			glVertex3f(-420.0f, 0.1f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.1f, 420.0f, 0.1f);
			glVertex3f(0.1f, -420.0f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.1f, 0.1f, 420.0f);
			glVertex3f(0.1f, 0.1f, -420.0f);
			glVertex3f(0.1f, 0.1f, 420.0f);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glLineWidth(1);
	glEndList();

	float i;
	glNewList(zProgram.GLGrid, GL_COMPILE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);

		glColor3f(0.0f, 0.0f, 0.0f);
		for(i = -GRID_SIZE; i <= GRID_SIZE; i += GRID_SIZE / 10.0f) {
			glBegin(GL_LINES);
				glVertex3f(-GRID_SIZE, 0.0f,          i);
				glVertex3f( GRID_SIZE, 0.0f,          i);
				glVertex3f(         i, 0.0f, -GRID_SIZE);
				glVertex3f(         i, 0.0f,  GRID_SIZE);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
	glEndList();
}

int gl_FinishScene()
{
	#ifdef WIN32
	SwapBuffers(hDC); return EXIT_SUCCESS;
	#else
	glXSwapBuffers(dpy, win); return EXIT_SUCCESS;
	#endif
}
