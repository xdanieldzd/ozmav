#include "globals.h"

void ca_Reset()
{
	Camera.AngleX = 0.0f, Camera.AngleY = 0.0f;
	Camera.X = 0.0f, Camera.Y = 1.5f, Camera.Z = 5.0f;
	Camera.LX = 0.0f, Camera.LY = 0.0f, Camera.LZ = -1.0f;
}

void ca_Orientation(float Ang, float Ang2)
{
	Camera.LX = sin(Ang);
	Camera.LY = Ang2;
	Camera.LZ = -cos(Ang);
}

void ca_Movement(bool Strafe, float Speed)
{
	if(!Strafe) {
		Camera.X += Camera.LX * 0.025f * Speed;
		Camera.Y += Camera.LY * 0.025f * Speed;
		Camera.Z += Camera.LZ * 0.025f * Speed;
	} else {
		Camera.X += cos(Camera.AngleX) * (0.025f * Speed);
		Camera.Z += sin(Camera.AngleX) * (0.025f * Speed);
	}
}

void ca_MouseMove(int x, int y)
{
	Program.MousePosX = x - Program.MouseCenterX;
	Program.MousePosY = y - Program.MouseCenterY;

	Camera.AngleX = Camera.AngleX + (0.01f * Program.MousePosX);
	Camera.AngleY = Camera.AngleY - (0.01f * Program.MousePosY);

	Program.MouseCenterX = x;
	Program.MouseCenterY = y;
}
