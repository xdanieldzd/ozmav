/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	actors.c - misc. actor-related functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Viewer_RenderAllActors()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	int StartMap = 0, EndMap = 0;
	if(ROM_CurrentMap == -1) {
		StartMap = 0;
		EndMap = SceneHeader[SceneHeader_Current].Map_Count;
	} else {
		StartMap = ROM_CurrentMap;
		EndMap = ROM_CurrentMap + 1;
	}

	float ColorR = 0.0f; float ColorG = 0.0f; float ColorB = 0.0f;

	int i;
	for(i = StartMap; i < EndMap; i++) {
		if(Renderer_EnableMapActors) {
			if(MapHeader[i][MapHeader_Current].Actor_Count > 0) {
				while(!(ActorInfo_CurrentCount[i] == MapHeader[i][MapHeader_Current].Actor_Count)) {
					ColorR = 0.0f; ColorG = 1.0f; ColorB = 0.0f;

//					if(ActorInfo_CurrentCount[i] == 0) { ColorR = 1.0f; ColorG = 1.0f; ColorB = 1.0f; }

					Viewer_RenderActor(Actors[i][ActorInfo_CurrentCount[i]].Number,
						Actors[i][ActorInfo_CurrentCount[i]].X_Position, Actors[i][ActorInfo_CurrentCount[i]].Y_Position, Actors[i][ActorInfo_CurrentCount[i]].Z_Position,
						Actors[i][ActorInfo_CurrentCount[i]].X_Rotation, Actors[i][ActorInfo_CurrentCount[i]].Y_Rotation, Actors[i][ActorInfo_CurrentCount[i]].Z_Rotation,
						ColorR, ColorG, ColorB);
					ActorInfo_CurrentCount[i]++;
				}
			}
		}

		if(Renderer_EnableSceneActors) {
			if(SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
				while(!(ScActorInfo_CurrentCount == SceneHeader[SceneHeader_Current].ScActor_Count)) {
					ColorR = 0.0f; ColorG = 0.0f; ColorB = 1.0f;

					Viewer_RenderActor(ScActors[ScActorInfo_CurrentCount].Number,
						ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position,
						ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation,
						ColorR, ColorG, ColorB);
					ScActorInfo_CurrentCount++;
				}
			}
		}

		if(Renderer_EnableDoors) {
			if(SceneHeader[SceneHeader_Current].Door_Count > 0) {
				while(!(DoorInfo_CurrentCount == SceneHeader[SceneHeader_Current].Door_Count)) {
					ColorR = 1.0f; ColorG = 0.0f; ColorB = 0.0f;

					Viewer_RenderActor(Doors[DoorInfo_CurrentCount].Number,
						Doors[DoorInfo_CurrentCount].X_Position, Doors[DoorInfo_CurrentCount].Y_Position, Doors[DoorInfo_CurrentCount].Z_Position,
						0, Doors[DoorInfo_CurrentCount].Y_Rotation, 0,
						ColorR, ColorG, ColorB);
					DoorInfo_CurrentCount++;
				}
			}
		}
	}

//	if(GLExtension_VertFragProgram) glEnable(GL_FRAGMENT_PROGRAM_ARB);

	return 0;
}

int Viewer_RenderActor(int ID, GLshort X, GLshort Y, GLshort Z, signed int X_Rot, signed int Y_Rot, signed int Z_Rot, float Color_Red, float Color_Green, float Color_Blue)
{
	glPushMatrix();

	glTranslated(X, Y, Z);
	glRotatef((X_Rot / 180), 1, 0, 0);
	glRotatef((Y_Rot / 180), 0, 1, 0);
	glRotatef((Z_Rot / 180), 0, 0, 1);

	switch (ActorTable[ID].Valid) {
/*		case true: {
			glScalef(0.05, 0.05, 0.05);
			glCallList(Renderer_GLDisplayList + Renderer_GLDisplayList_Total + ID);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			break;
		}
*/
		default: {
			if(Renderer_EnableFog) { glEnable(GL_FOG); } else { glDisable(GL_FOG); }

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);

			glLineWidth(2.5f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBegin(GL_QUADS);
				glColor3f(1.0f, 1.0f, 1.0f);

				glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
				glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6

				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
				glVertex3f(-15.0f, 15.0f, 15.0f);   //V8

				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
				glVertex3f(-15.0f, 15.0f, 15.0f);   //V8
				glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

				glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
			glEnd();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_CULL_FACE);

			glBegin(GL_QUADS);
				glColor3f(Color_Red, Color_Green, Color_Blue);

				glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
				glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6

				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
				glVertex3f(-15.0f, 15.0f, 15.0f);   //V8

				glVertex3f(-15.0f, 15.0f,-15.0f);   //V6
				glVertex3f(-15.0f, 15.0f, 15.0f);   //V8
				glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
				glVertex3f( 15.0f, 15.0f,-15.0f);   //V4

				glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
				glVertex3f(-15.0f,-15.0f,-15.0f);   //V5
				glVertex3f( 15.0f,-15.0f,-15.0f);   //V3
				glVertex3f( 15.0f,-15.0f, 15.0f);   //V1

				//front
				glColor3f(1.0f, 1.0f, 1.0f);

				glVertex3f(-15.0f, 15.0f, 15.0f);   //V8
				glVertex3f(-15.0f,-15.0f, 15.0f);   //V7
				glVertex3f( 15.0f,-15.0f, 15.0f);   //V1
				glVertex3f( 15.0f, 15.0f, 15.0f);   //V2
			glEnd();

			glEnable(GL_CULL_FACE);

			glLineWidth(1.0f);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			break;
		}
	}

	glPopMatrix();

	return 0;
}

/*	------------------------------------------------------------ */
