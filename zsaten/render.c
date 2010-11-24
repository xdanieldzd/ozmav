#include "globals.h"

void gl_CreateViewerDLists()
{
	vProgram.actorAxisMarkerDL = glGenLists(1);
	vProgram.sceneAxisMarkerDL = glGenLists(1);

	glNewList(vProgram.actorAxisMarkerDL, GL_COMPILE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
		glLineWidth(2);

		glBegin(GL_LINES);
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f(100.0f, 0.1f, 0.1f);
			glVertex3f(-100.0f, 0.1f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f(0.1f, 100.0f, 0.1f);
			glVertex3f(0.1f, -100.0f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(0.1f, 0.1f, 100.0f);
			glVertex3f(0.1f, 0.1f, -100.0f);
			glVertex3f(0.1f, 0.1f, 100.0f);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glLineWidth(1);
	glEndList();

	glNewList(vProgram.sceneAxisMarkerDL, GL_COMPILE);
		glEnable(GL_BLEND);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
		glLineWidth(4);

		glBegin(GL_LINES);
			glColor4f(0.0f, 0.0f, 1.0f, 0.2f);
			glVertex3f(1000.0f, 0.1f, 0.1f);
			glVertex3f(-1000.0f, 0.1f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
			glVertex3f(0.1f, 1000.0f, 0.1f);
			glVertex3f(0.1f, -1000.0f, 0.1f);
		glEnd();
		glBegin(GL_LINES);
			glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
			glVertex3f(0.1f, 0.1f, 1000.0f);
			glVertex3f(0.1f, 0.1f, -1000.0f);
			glVertex3f(0.1f, 0.1f, 1000.0f);
		glEnd();

		glEnable(GL_ALPHA_TEST);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);

		glDisable(GL_BLEND);
		glLineWidth(1);
	glEndList();
}

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

	vCamera.rotX = l_rX;
	vCamera.rotY = l_rY;
}

void gl_SetupScene2D(int Width, int Height)
{
	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Width, Height, 0, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);

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

void gl_DrawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl_SetupScene3D(vProgram.windowWidth, vProgram.windowHeight);
	gl_LookAt(	vCamera.X, vCamera.Y, vCamera.Z,
				vCamera.X + vCamera.LX, vCamera.Y + vCamera.LY, vCamera.Z + vCamera.LZ);
	glScalef(0.005f, 0.005f, 0.005f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPushMatrix();
	glRotatef(vCamera.actorRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(vCamera.actorRotY, 0.0f, 1.0f, 0.0f);

	if((vCurrentActor.boneSetupTotal >= 0)/* && (vCurrentActor.animTotal >= 0)*/) {
		if(!vProgram.showBones && vProgram.enableHUD) glCallList(vProgram.actorAxisMarkerDL);
		if(vCurrentActor.hack == LINK)
			drawLink(
				vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupCurrent],
				vCurrentActor.offsetAnims[vCurrentActor.animCurrent],
				vCurrentActor.actorScale,
				0, 0, 0, /* trans */
				0, 0, 0, /* rot */
				vCurrentActor.linkUseDetailModel, /* close or far model */
				vCurrentActor.animFrames[vCurrentActor.animCurrent]
			);
		else if(vCurrentActor.hack == OBJECT_HUMAN)
			drawOldBones(
				vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupCurrent],
				vCurrentActor.offsetAnims[vCurrentActor.animCurrent],
				vCurrentActor.actorScale,
				0, 0, 0, /* trans */
				0, 0, 0  /* rot */
			);
		else
			drawBones(
				vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupCurrent],
				vCurrentActor.offsetAnims[vCurrentActor.animCurrent],
				vCurrentActor.actorScale,
				0, 0, 0, /* trans */
				0, 0, 0  /* rot */
			);
	} else if(RDP_CheckAddressValidity(vCurrentActor.offsetDList)) {
		if(vProgram.enableHUD) glCallList(vProgram.actorAxisMarkerDL);
		RDP_ClearStructures(false);

		glPushMatrix();
		glScalef(vCurrentActor.actorScale, vCurrentActor.actorScale, vCurrentActor.actorScale);
		RDP_ParseDisplayList(vCurrentActor.offsetDList, true);
		glPopMatrix();
	}
	glPopMatrix();

	if(!vProgram.enableHUD) return;

	if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glCallList(vProgram.sceneAxisMarkerDL);

	gl_SetupScene2D(vProgram.windowWidth, vProgram.windowHeight);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	char statMsg[256];

	hud_Print(0, 0, 0, 0, "");

	if(vCurrentActor.useActorOvl) {
		sprintf(statMsg, "Actor 0x%04X / Object 0x%04X:\n- Actor name: %s\n- Object name: %s\n- Alt. Object name: %s",
			vCurrentActor.actorNumber, vActors[vCurrentActor.actorNumber].ObjectNumber,
			(strcmp(vActors[vCurrentActor.actorNumber].ActorName, "\0") ? vActors[vCurrentActor.actorNumber].ActorName : "unknown"),
			(strcmp(vObjects[vActors[vCurrentActor.actorNumber].ObjectNumber].ObjectName, "\0") ? vObjects[vActors[vCurrentActor.actorNumber].ObjectNumber].ObjectName : "unknown"),
			(strcmp(vObjects[vActors[vCurrentActor.actorNumber].AltObjectNumber].ObjectName, "\0") ? vObjects[vActors[vCurrentActor.actorNumber].AltObjectNumber].ObjectName : "unknown"));
	} else if(vCurrentActor.useExtAnim) {
		sprintf(statMsg, "Object name: %s\nAnimation file: %s", vCurrentActor.oName, vCurrentActor.eaName);
	} else {
		sprintf(statMsg, "Object name: %s", vCurrentActor.oName);
	}

	hud_Print(0, 0, -1, -1, statMsg);

	if((vCurrentActor.boneSetupTotal >= 0) && (vCurrentActor.animTotal >= 0) && (vActors[vCurrentActor.actorNumber].isValid) ) {
		sprintf(statMsg,
			"Using bone structure %02i of %02i\n"
			" - Offset: 0x%08X\n"
			"Showing animation %03i of %03i\n"
			" - Offset: 0x%08X\n"
			"Current animation frame: %02i/%02i\n"
			"Target FPS: %2.0f\n"
			"%cBone structure: %s\n"
			"%cAnimation is %s",
			vCurrentActor.boneSetupCurrent + 1, vCurrentActor.boneSetupTotal + 1,
			vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupCurrent],			vCurrentActor.animCurrent + 1, vCurrentActor.animTotal + 1,			vCurrentActor.offsetAnims[vCurrentActor.animCurrent],
			vCurrentActor.frameCurrent + 1, vCurrentActor.frameTotal + 1,
			vProgram.targetFPS,
			(vProgram.showBones ? '\x90' : '\x91'), (vProgram.showBones ? "shown" : "hidden"),
			(vProgram.animPlay ? '\x90' : '\x91'), (vProgram.animPlay ? "running..." : "stopped.")
		);
		hud_Print(-1, 0, -1, -1, statMsg);
	}

	#if 0
	sprintf(statMsg, "Camera:\nX: %.2f, Y: %.2f, Z: %.2f\nLX: %.2f, LY: %.2f, LZ: %.2f\nAngle: X: %.2f, Y: %.2f\nActor Rot: X: %.2f, Y: %.2f",
		vCamera.X, vCamera.Y, vCamera.Z, vCamera.LX, vCamera.LY, vCamera.LZ, vCamera.angleX, vCamera.angleY, vCamera.actorRotX, vCamera.actorRotY);
	hud_Print(-1, -1, -1, -1, statMsg);
	#endif

	#if 0
	//font autowidth test
	hud_Print(0, 80, -1, -1, " !\"#$%&'()*+,-./\n0123456789:;<=>?\n@ABCDEFGHIJKLMNO\nPQRSTUVWXYZ[\\]^_\n`abcdefghijklmno\npqrstuvwxyz{|}~");
	#endif
}

int gl_FinishScene()
{
	#ifdef WIN32
	SwapBuffers(hDC); return EXIT_SUCCESS;
	#else
	glXSwapBuffers(dpy, win); return EXIT_SUCCESS;
	#endif
}
