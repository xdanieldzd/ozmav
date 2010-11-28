void gl_CreateViewerDLists();
void gl_Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
void gl_LookAt(const GLdouble p_EyeX, const GLdouble p_EyeY, const GLdouble p_EyeZ, const GLdouble p_CenterX, const GLdouble p_CenterY, const GLdouble p_CenterZ);
void gl_SetupScene2D(int Width, int Height);
void gl_SetupScene3D(int Width, int Height);
void gl_DrawScene();
int gl_FinishScene();
