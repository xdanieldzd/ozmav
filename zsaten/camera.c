#include "globals.h"

void ca_Reset()
{
	vCamera.X = -0.7f, vCamera.Y = 0.3f, vCamera.Z = 0.7f;
	vCamera.LX = 0.7f, vCamera.LY = -0.2f, vCamera.LZ = -0.7f;
	vCamera.angleX = vCamera.LX * 1.1f, vCamera.angleY = vCamera.LY;

	vCamera.actorRotX = 0.0f, vCamera.actorRotY = 0.0f;

	ca_Orientation(vCamera.angleX, vCamera.angleY);
}

void ca_Orientation(float Ang, float Ang2)
{
	vCamera.LX = sin(Ang);
	vCamera.LY = Ang2;
	vCamera.LZ = -cos(Ang);
}

void ca_Movement(bool Strafe, float Speed)
{
	if(!Strafe) {
		vCamera.X += vCamera.LX * 0.025f * Speed;
		vCamera.Y += vCamera.LY * 0.025f * Speed;
		vCamera.Z += vCamera.LZ * 0.025f * Speed;
	} else {
		vCamera.X += cos(vCamera.angleX) * (0.025f * Speed);
		vCamera.Z += sin(vCamera.angleX) * (0.025f * Speed);
	}
}

void ca_MouseMove(int x, int y)
{
	vProgram.mousePosX = x - vProgram.mouseCenterX;
	vProgram.mousePosY = y - vProgram.mouseCenterY;

	vCamera.angleX = vCamera.angleX + (0.01f * vProgram.mousePosX);
	vCamera.angleY = vCamera.angleY - (0.01f * vProgram.mousePosY);

	vProgram.mouseCenterX = x;
	vProgram.mouseCenterY = y;
}
