extern __Vect3D ms_GetSceneCoords(int MousePosX, int MousePosY);
extern int ms_SelectedMapActor();
extern GLint gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *objx, GLdouble *objy, GLdouble *objz);
