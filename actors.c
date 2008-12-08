/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	actors.c - misc. actor-related functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

/* VIEWER_RENDERALLACTORS - RENDERS THE CURRENT MAP'S MAP AND/OR SCENE ACTORS */
int Viewer_RenderAllActors()
{
	if((GLExtension_MultiTexture) && (Renderer_EnableCombiner)) {
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
	} else {
		glDisable(GL_TEXTURE_2D);
	}

	glDisable(GL_FOG);

	int i;
	for(i = 0; i < (SceneHeader[SceneHeader_Current].Map_Count); i++) {
		ROM_CurrentMap = i;

		if(Renderer_EnableMapActors) {
			if (MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count > 0) {
				while (!(ActorInfo_CurrentCount[ROM_CurrentMap] == MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count)) {
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					Viewer_RenderActorCube(ActorInfo_CurrentCount[ROM_CurrentMap],
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Position,
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Rotation,
						true);
					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActorCube(ActorInfo_CurrentCount[ROM_CurrentMap],
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Position, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Position,
						Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Rotation, Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Rotation,
						true);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					ActorInfo_CurrentCount[ROM_CurrentMap]++;
				}
			}
		}

		if(Renderer_EnableSceneActors) {
			if (SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
				while (!(ScActorInfo_CurrentCount == SceneHeader[SceneHeader_Current].ScActor_Count)) {
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					Viewer_RenderActorCube(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActorCube(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					ScActorInfo_CurrentCount++;
				}
			}
		}
	}

	return 0;
}

/* VIEWER_RENDERACTORCUBE - RENDERS AN INFAMOUS 'ACTOR CUBE' AT THE SPECIFIED LOCATION ON THE MAP */
int Viewer_RenderActorCube(int ActorToRender, GLshort X, GLshort Y, GLshort Z, signed int X_Rot, signed int Y_Rot, signed int Z_Rot, bool IsMapActor)
{
	glPushMatrix();

	glTranslated(X, Y, Z);
	glRotatef((X_Rot / 180), 1, 0, 0);
	glRotatef((Y_Rot / 180), 0, 1, 0);
	glRotatef((Z_Rot / 180), 0, 0, 1);

	glBegin(GL_QUADS);
		/* CHECK IF ACTOR IS MAP ACTOR OR SCENE ACTOR */
		if(IsMapActor) {
//			if((ActorToRender == ActorInfo_Selected)) {
//				glColor3f(1.0f, 0.0f, 0.0f);
//			} else {
				glColor3f(0.0f, 1.0f, 0.0f);
//			}
		} else {
//			if((ActorToRender == ScActorInfo_Selected)) {
//				glColor3f(1.0f, 1.0f, 0.0f);
//			} else {
				glColor3f(0.0f, 0.0f, 1.0f);
//			}
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

	glPopMatrix();

	return 0;
}

/*	------------------------------------------------------------ */
