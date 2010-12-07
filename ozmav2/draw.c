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

	zCamera.RotX = l_rX;
	zCamera.RotY = l_rY;
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

void gl_DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);
	gl_LookAt(zCamera.X, zCamera.Y, zCamera.Z, zCamera.X + zCamera.LX, zCamera.Y + zCamera.LY, zCamera.Z + zCamera.LZ);

	glScalef(0.005, 0.005, 0.005);

	int StartMap = 0; int EndMap = zSHeader[0].MapCount;
	if(zOptions.MapToRender != -1) {
		StartMap = zOptions.MapToRender;
		EndMap = StartMap + 1;
	}

	int Maps = 0;
	for(Maps = StartMap; Maps < EndMap; Maps++) {
		gl_DrawMap(Maps);
		gl_DrawActors(Maps);
	}
	gl_DrawDoors();

	gl_DrawHUD(StartMap, EndMap);
}

void gl_DrawMap(int MapNo)
{
	int DL = 0;
	for(DL = 0; DL < zGfx.DLCount[MapNo]; DL++) glCallList(zGfx.GLLists[MapNo] + DL);
}

void gl_DrawActors(int MapNo)
{
	GLfloat ActorAmbientHighlight[] = { 1.0, -1.0, -1.0, 1.0 };
	GLfloat AmbientDefault[3];
	glGetLightfv(GL_LIGHT0, GL_AMBIENT, AmbientDefault);

	int Actor = 0;

	for(Actor = 0; Actor < zMHeader[0][MapNo].ActorCount; Actor++) {
		glPushMatrix();

		// pop the matrix and translate/rotate to the actor's position
		glTranslated(zMapActor[MapNo][Actor].Pos.X, zMapActor[MapNo][Actor].Pos.Y, zMapActor[MapNo][Actor].Pos.Z);
		glRotated(zMapActor[MapNo][Actor].Rot.X / 182.0444444, 1, 0, 0);
		glRotated(zMapActor[MapNo][Actor].Rot.Y / 182.0444444, 0, 1, 0);
		glRotated(zMapActor[MapNo][Actor].Rot.Z / 182.0444444, 0, 0, 1);

		if((MapNo == zOptions.SelectedActorMap) && (Actor == zOptions.SelectedActor) &&
			(zOptions.SelectedActorMap > -1) && (zOptions.SelectedActor > -1)) {
				// setup GL lighting to do the red highlighting
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ActorAmbientHighlight);
				// call the actor's GL display list
				glCallList(zMapActor[MapNo][Actor].GLDList);

				// ...so that we can call our axis marker display list...
				if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
				glCallList(zProgram.AxisMarker);
				if(RDP_OpenGL_ExtFragmentProgram()) glEnable(GL_FRAGMENT_PROGRAM_ARB);

				// and reset the lighting
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientDefault);

				gl_DrawActorCube(true);
			} else {
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientDefault);
				glCallList(zMapActor[MapNo][Actor].GLDList);
				gl_DrawActorCube(false);
			}

		glPopMatrix();
	}
}

void gl_DrawDoors()
{
	int Door = 0;

	for(Door = 0; Door < zSHeader[0].DoorCount; Door++) {
		glPushMatrix();

		glTranslated(zDoor[Door].Pos.X, zDoor[Door].Pos.Y, zDoor[Door].Pos.Z);
		glRotated(zDoor[Door].Rot.X / 182.0444444, 1, 0, 0);
		glRotated(zDoor[Door].Rot.Y / 182.0444444, 0, 1, 0);
		glRotated(zDoor[Door].Rot.Z / 182.0444444, 0, 0, 1);
		glCallList(zDoor[Door].GLDList);

		// simulating door visibility from both sides by drawing it once more, this time rotated by 180 deg
		glRotated(180, 0, 1, 0);
		glCallList(zDoor[Door].GLDList);

		glPopMatrix();
	}
}

static inline GLfloat gl_GetPointDistance(__Vect3D First, __Vect3D Second)
{
	GLfloat DeltaX = (float)Second.X - (float)First.X;
	GLfloat DeltaY = (float)Second.Y - (float)First.Y;
	GLfloat DeltaZ = (float)Second.Z - (float)First.Z;
	return sqrtf(DeltaX * DeltaX + DeltaY * DeltaY + DeltaZ * DeltaZ);
};

void gl_DrawHUD(int StartMap, int EndMap)
{
	gl_SetupScene2D(zProgram.WindowWidth, zProgram.WindowHeight);

	__Vect3D SelectedActorSC = { 0, 0, 0 };
	bool ShowActorBox = false;

	int Actor = 0, Map = 0;
	for(Map = StartMap; Map < EndMap; Map++) {
		for(Actor = 0; Actor < zMHeader[0][Map].ActorCount; Actor++) {
			__Vect3D ScreenCoords = ms_GetScreenCoords((float)zMapActor[Map][Actor].Pos.X, (float)zMapActor[Map][Actor].Pos.Y, (float)zMapActor[Map][Actor].Pos.Z);
			if(ScreenCoords.X == -1) ScreenCoords.X = 0;
			if(ScreenCoords.Y == -1) ScreenCoords.Y = 0;

			if(ScreenCoords.Z == 0) {
				if((Map == zOptions.SelectedActorMap) && (Actor == zOptions.SelectedActor) &&
					(zOptions.SelectedActorMap > -1) && (zOptions.SelectedActor > -1)) {
						SelectedActorSC = ScreenCoords;
						ShowActorBox = true;
					} else {
						__Vect3D CamPos = ms_GetSceneCoords(0, 0);
						__Vect3D ActorPos = {
							(float)zMapActor[Map][Actor].Pos.X,
							(float)zMapActor[Map][Actor].Pos.Y,
							(float)zMapActor[Map][Actor].Pos.Z
							};
						GLfloat Visibility = (-(gl_GetPointDistance(CamPos, ActorPos) / 7500.0f) + 1.0f);
						Visibility = (Visibility > 1.0f) ? 1.0f : Visibility;

						if(Visibility > 0.0f) {
							hud_Print(ScreenCoords.X, ScreenCoords.Y, -1, -1, 1, Visibility,
								"#%i: %s\n0x%04X/0x%04X",
								Actor, zActor[zMapActor[Map][Actor].Number].Name, zMapActor[Map][Actor].Number, zMapActor[Map][Actor].Var);
						}
					}
			}
		}
	}

	if(ShowActorBox) {
/*		dbgprintf(0, 0, "%4.2f %4.2f %4.2f -> %4.2f %4.2f %4.2f -> %i %i %i",
			(float)zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X,
			(float)zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Y,
			(float)zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Z,
			zCamera.X, zCamera.Y, zCamera.Z, SelectedActorSC.X, SelectedActorSC.Y, SelectedActorSC.Z);
*/
		__zHUDMenuEntry ActorMenu[] = {
			{ "Name", NULL, -1, -1 },
			{ "", NULL, 0, -1 },
			{ "Settings", NULL, -1, -1 },
			{ "Number", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number, 2, 3 },
			{ "Variable", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Var, 2, 3 },
			{ "Position", NULL, -1, -1 },
			{ "X", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X, 2, 0 },
			{ "Y", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Y, 2, 0 },
			{ "Z", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Z, 2, 0 },
			{ "Rotation", NULL, -1, -1 },
			{ "X", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.X, 2, 0 },
			{ "Y", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.Y, 2, 0 },
			{ "Z", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.Z, 2, 0 }
		};
		sprintf(ActorMenu[1].Name, "%s", zActor[zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number].Name);

		char TempString[256];
		sprintf(TempString, "Map Actor #%i", zOptions.SelectedActor);
		hudMenu_Render(TempString, SelectedActorSC.X, SelectedActorSC.Y, ActorMenu, ArraySize(ActorMenu));
	}
}

void gl_DrawActorCube(bool Selected)
{
	glPushMatrix();

	//overall settings...
	glScalef(10.0, 10.0, 10.0);
	glDisable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//solid inner cube...
	if(Selected) {
		glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
	} else {
		glColor4f(1.0f, 0.7f, 0.0f, 0.7f);
	}
	glCallList(zProgram.CubeDL);
	//wireframe outer cube...
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(zProgram.CubeDL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//reset settings...
	glLineWidth(1);
	glDisable(GL_BLEND);
	if(RDP_OpenGL_ExtFragmentProgram()) glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void gl_ClearDisplayLists()
{
	int i = 0, j = 0;
	for(i = 0; i < 256; i++) {
		// maps
		if(glIsList(zGfx.GLLists[i])) glDeleteLists(zGfx.GLLists[i], zGfx.DLCount[i]);
		zGfx.DLCount[i] = 0;

		// doors
		if(glIsList(zDoor[i].GLDList)) glDeleteLists(zDoor[i].GLDList, 1);

		// actors
		for(j = 0; j < 1024; j++) {
			if(glIsList(zMapActor[i][j].GLDList)) glDeleteLists(zMapActor[i][j].GLDList, 1);
		}
	}
}

void gl_CreateViewerDLists()
{
	zProgram.AxisMarker = glGenLists(1);
	zProgram.CubeDL = glGenLists(1);

	glNewList(zProgram.AxisMarker, GL_COMPILE);
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

	glNewList(zProgram.CubeDL, GL_COMPILE);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			// Front Face
			glVertex3f(-1.0f, -1.0f,  1.0f);
			glVertex3f( 1.0f, -1.0f,  1.0f);
			glVertex3f( 1.0f,  1.0f,  1.0f);
			glVertex3f(-1.0f,  1.0f,  1.0f);
			// Back Face
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f,  1.0f, -1.0f);
			glVertex3f( 1.0f,  1.0f, -1.0f);
			glVertex3f( 1.0f, -1.0f, -1.0f);
			// Top Face
			glVertex3f(-1.0f,  1.0f, -1.0f);
			glVertex3f(-1.0f,  1.0f,  1.0f);
			glVertex3f( 1.0f,  1.0f,  1.0f);
			glVertex3f( 1.0f,  1.0f, -1.0f);
			// Bottom Face
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f( 1.0f, -1.0f, -1.0f);
			glVertex3f( 1.0f, -1.0f,  1.0f);
			glVertex3f(-1.0f, -1.0f,  1.0f);
			// Right Face
			glVertex3f( 1.0f, -1.0f, -1.0f);
			glVertex3f( 1.0f,  1.0f, -1.0f);
			glVertex3f( 1.0f,  1.0f,  1.0f);
			glVertex3f( 1.0f, -1.0f,  1.0f);
			// Left Face
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f,  1.0f);
			glVertex3f(-1.0f,  1.0f,  1.0f);
			glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();
		glEnable(GL_TEXTURE_2D);
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
