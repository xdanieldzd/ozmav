#include "globals.h"

__Vect3D ms_GetScreenCoords(float SceneX, float SceneY, float SceneZ)
{
	__Vect3D RetVect;
	GLdouble ModelM[16], ProjM[16], Pos[3];
	int Viewp[4];

	glPushMatrix();
		glLoadIdentity();
		gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);
		gl_LookAt(zCamera.X, zCamera.Y, zCamera.Z, zCamera.X + zCamera.LX, zCamera.Y + zCamera.LY, zCamera.Z + zCamera.LZ);
		glScalef(0.005, 0.005, 0.005);

		glGetDoublev(GL_MODELVIEW_MATRIX, ModelM);
		glGetDoublev(GL_PROJECTION_MATRIX, ProjM);
		glGetIntegerv(GL_VIEWPORT, (GLint*)Viewp);

		gluProject((GLdouble)SceneX, (GLdouble)SceneY, (GLdouble)SceneZ, ModelM, ProjM, (GLint*)Viewp, &Pos[0], &Pos[1], &Pos[2]);

		Pos[1] = (GLdouble)Viewp[3] - (GLdouble)Pos[1] - 1;

		RetVect.X = (signed int)Pos[0];
		RetVect.Y = (signed int)Pos[1];
		RetVect.Z = (signed int)Pos[2];

		gl_SetupScene2D(zProgram.WindowWidth, zProgram.WindowHeight);
	glPopMatrix();

	return RetVect;
}

__Vect3D ms_GetSceneCoords(int MousePosX, int MousePosY)
{
	__Vect3D RetVect;
	float X, Y, Z;
	GLdouble ModelM[16], ProjM[16], Pos[3];
	int Viewp[4];

	glPushMatrix();
		glLoadIdentity();
		gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);
		gl_LookAt(zCamera.X, zCamera.Y, zCamera.Z, zCamera.X + zCamera.LX, zCamera.Y + zCamera.LY, zCamera.Z + zCamera.LZ);
		glScalef(0.005, 0.005, 0.005);

		glGetDoublev(GL_MODELVIEW_MATRIX, ModelM);
		glGetDoublev(GL_PROJECTION_MATRIX, ProjM);
		glGetIntegerv(GL_VIEWPORT, (GLint*)Viewp);

		X = (float)MousePosX;
		Y = (float)Viewp[3] - (float)MousePosY;
		glReadPixels(MousePosX, (int)Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Z);
		gluUnProject((GLdouble)X, (GLdouble)Y, (GLdouble)Z, ModelM, ProjM, (GLint*)Viewp, &Pos[0], &Pos[1], &Pos[2]);

		RetVect.X = (signed int)Pos[0];
		RetVect.Y = (signed int)Pos[1];
		RetVect.Z = (signed int)Pos[2];

		gl_SetupScene2D(zProgram.WindowWidth, zProgram.WindowHeight);
	glPopMatrix();

	return RetVect;
}

int ms_SelectedMapActor()
{
	if((zOptions.MapToRender == -1)/* || (zOptions.EnableActors)*/) {
		dbgprintf(0, MSK_COLORTYPE_WARNING, "Warning: Cannot select Actors in current mode!");
		return -1;
	}

	int SX = zProgram.SceneCoords.X, SY = zProgram.SceneCoords.Y, SZ = zProgram.SceneCoords.Z;

	int i, HitRadius = 20;

	for(i = 0; i < zMHeader[0][zOptions.MapToRender].ActorCount; i++) {
		int X = zMapActor[zOptions.MapToRender][i].Pos.X, Y = zMapActor[zOptions.MapToRender][i].Pos.Y, Z = zMapActor[zOptions.MapToRender][i].Pos.Z;
		if(	(X >= SX - HitRadius) && (X <= SX + HitRadius) &&
			(Y >= SY - HitRadius) && (Y <= SY + HitRadius) &&
			(Z >= SZ - HitRadius) && (Z <= SZ + HitRadius)) {
				return i;
			}
	}

	return zOptions.SelectedActor;
}

static void __gluMultMatricesd(const GLdouble a[16], const GLdouble b[16],
				GLdouble r[16])
{
    int i, j;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    r[i*4+j] =
		a[i*4+0]*b[0*4+j] +
		a[i*4+1]*b[1*4+j] +
		a[i*4+2]*b[2*4+j] +
		a[i*4+3]*b[3*4+j];
	}
    }
}

static void __gluMultMatrixVecd(const GLdouble matrix[16], const GLdouble in[4],
		      GLdouble out[4])
{
    int i;

    for (i=0; i<4; i++) {
	out[i] =
	    in[0] * matrix[0*4+i] +
	    in[1] * matrix[1*4+i] +
	    in[2] * matrix[2*4+i] +
	    in[3] * matrix[3*4+i];
    }
}

/*
** Invert 4x4 matrix.
** Contributed by David Moore (See Mesa bug #6748)
*/
static int __gluInvertMatrixd(const GLdouble m[16], GLdouble invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
             + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
    inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
             - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
    inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
             + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
    inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
             - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
    inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
             - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
    inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
             + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
    inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
             - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
    inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
             + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
    inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
             + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
    inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
             - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
    inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
             + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
    inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
             - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
    inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
             - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
    inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
             + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
    inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
             - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
    inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
             + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

    det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
    if (det == 0)
        return GL_FALSE;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return GL_TRUE;
}

GLint gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
		const GLdouble modelMatrix[16],
		const GLdouble projMatrix[16],
                const GLint viewport[4],
	        GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
    double finalMatrix[16];
    double in[4];
    double out[4];

    __gluMultMatricesd(modelMatrix, projMatrix, finalMatrix);
    if (!__gluInvertMatrixd(finalMatrix, finalMatrix)) return(GL_FALSE);

    in[0]=winx;
    in[1]=winy;
    in[2]=winz;
    in[3]=1.0;

    /* Map x and y from window coordinates */
    in[0] = (in[0] - viewport[0]) / viewport[2];
    in[1] = (in[1] - viewport[1]) / viewport[3];

    /* Map to range -1 to 1 */
    in[0] = in[0] * 2 - 1;
    in[1] = in[1] * 2 - 1;
    in[2] = in[2] * 2 - 1;
    in[3] = 1;

    __gluMultMatrixVecd(finalMatrix, in, out);
    if (out[3] == 0.0) return(GL_FALSE);
    out[0] /= out[3];
    out[1] /= out[3];
    out[2] /= out[3];
    *objx = out[0];
    *objy = out[1];
    *objz = out[2];
    return(GL_TRUE);
}

GLint gluProject(GLdouble objx, GLdouble objy, GLdouble objz,
	      const GLdouble modelMatrix[16],
	      const GLdouble projMatrix[16],
              const GLint viewport[4],
	      GLdouble *winx, GLdouble *winy, GLdouble *winz)
{
    double in[4];
    double out[4];

    in[0]=objx;
    in[1]=objy;
    in[2]=objz;
    in[3]=1.0;
    __gluMultMatrixVecd(modelMatrix, in, out);
    __gluMultMatrixVecd(projMatrix, out, in);
    if (in[3] == 0.0) return(GL_FALSE);
    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];
    /* Map x, y and z to range 0-1 */
    in[0] = in[0] * 0.5 + 0.5;
    in[1] = in[1] * 0.5 + 0.5;
    in[2] = in[2] * 0.5 + 0.5;

    /* Map x,y to viewport */
    in[0] = in[0] * viewport[2] + viewport[0];
    in[1] = in[1] * viewport[3] + viewport[1];

    *winx=in[0];
    *winy=in[1];
    *winz=in[2];
    return(GL_TRUE);
}
