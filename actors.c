/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	actors.c - misc. actor-related functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

/* VIEWER_RENDERALLACTORS - RENDERS THE CURRENT MAP'S MAP AND/OR SCENE ACTORS */
int Viewer_RenderAllActors()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	int i;
	for(i = 0; i < (SceneHeader[SceneHeader_Current].Map_Count); i++) {
		ROM_CurrentMap = i;

		if(Renderer_EnableMapActors) {
			if (MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count > 0) {
				while (!(ActorInfo_CurrentCount[ROM_CurrentMap] == MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count)) {
/*					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
*/					Viewer_RenderActor(Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Number,
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Position,
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Rotation,
						true);
/*					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActor(ActorInfo_CurrentCount[ROM_CurrentMap],
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Position,
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Rotation,
						true);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
*/					ActorInfo_CurrentCount[ROM_CurrentMap]++;
				}
			}
		}

		if(Renderer_EnableSceneActors) {
			if (SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
				while (!(ScActorInfo_CurrentCount == SceneHeader[SceneHeader_Current].ScActor_Count)) {
/*					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
*/					Viewer_RenderActor(ScActors[ScActorInfo_CurrentCount].Number, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
/*					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActor(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
*/					ScActorInfo_CurrentCount++;
				}
			}
		}
	}

	return 0;
}

int Viewer_RenderActor(int ID, GLshort X, GLshort Y, GLshort Z, signed int X_Rot, signed int Y_Rot, signed int Z_Rot, bool IsMapActor)
{
	glPushMatrix();

	glTranslated(X, Y, Z);
	glRotatef((X_Rot / 180), 1, 0, 0);
	glRotatef((Y_Rot / 180), 0, 1, 0);
	glRotatef((Z_Rot / 180), 0, 0, 1);

	/*if(ActorTable[ID].Valid) {
		glScalef(0.05, 0.05, 0.05);
		glCallList(Renderer_GLDisplayList + Renderer_GLDisplayList_Total + ID);
	} else {*/
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBegin(GL_QUADS);
			if(IsMapActor) {
				glColor3f(0.0f, 1.0f, 0.0f);
			} else {
				glColor3f(0.0f, 0.0f, 1.0f);
			}

			glVertex3s( 12, 12, 12);   //V2
			glVertex3s( 12,-12, 12);   //V1
			glVertex3s( 12,-12,-12);   //V3
			glVertex3s( 12, 12,-12);   //V4

			glVertex3s( 12, 12,-12);   //V4
			glVertex3s( 12,-12,-12);   //V3
			glVertex3s(-12,-12,-12);   //V5
			glVertex3s(-12, 12,-12);   //V6

			glVertex3s(-12, 12,-12);   //V6
			glVertex3s(-12,-12,-12);   //V5
			glVertex3s(-12,-12, 12);   //V7
			glVertex3s(-12, 12, 12);   //V8

			glVertex3s(-12, 12,-12);   //V6
			glVertex3s(-12, 12, 12);   //V8
			glVertex3s( 12, 12, 12);   //V2
			glVertex3s( 12, 12,-12);   //V4

			glVertex3s(-12,-12, 12);   //V7
			glVertex3s(-12,-12,-12);   //V5
			glVertex3s( 12,-12,-12);   //V3
			glVertex3s( 12,-12, 12);   //V1

			//front
			glColor3f(1.0f, 1.0f, 1.0f);

			glVertex3s(-12, 12, 12);   //V8
			glVertex3s(-12,-12, 12);   //V7
			glVertex3s( 12,-12, 12);   //V1
			glVertex3s( 12, 12, 12);   //V2
		glEnd();

		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHTING);
	//}

	glPopMatrix();

	return 0;
}

/*	------------------------------------------------------------ */
