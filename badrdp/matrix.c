#include "globals.h"

void RDP_Matrix_MulMatrices(float Src1[4][4], float Src2[4][4], float Target[4][4])
{
/*	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			Target[i][j] =
				Src1[i][0] * Src2[0][j] +
				Src1[i][1] * Src2[1][j] +
				Src1[i][2] * Src2[2][j] +
				Src1[i][3] * Src2[3][j];
		}
	}*/

	Target[0][0] = Src1[0][0] * Src2[0][0] + Src1[0][1] * Src2[1][0] + Src1[0][2] * Src2[2][0] + Src1[0][3] * Src2[3][0];
	Target[0][1] = Src1[0][0] * Src2[0][1] + Src1[0][1] * Src2[1][1] + Src1[0][2] * Src2[2][1] + Src1[0][3] * Src2[3][1];
	Target[0][2] = Src1[0][0] * Src2[0][2] + Src1[0][1] * Src2[1][2] + Src1[0][2] * Src2[2][2] + Src1[0][3] * Src2[3][2];
	Target[0][3] = Src1[0][0] * Src2[0][3] + Src1[0][1] * Src2[1][3] + Src1[0][2] * Src2[2][3] + Src1[0][3] * Src2[3][3];

	Target[1][0] = Src1[1][0] * Src2[0][0] + Src1[1][1] * Src2[1][0] + Src1[1][2] * Src2[2][0] + Src1[1][3] * Src2[3][0];
	Target[1][1] = Src1[1][0] * Src2[0][1] + Src1[1][1] * Src2[1][1] + Src1[1][2] * Src2[2][1] + Src1[1][3] * Src2[3][1];
	Target[1][2] = Src1[1][0] * Src2[0][2] + Src1[1][1] * Src2[1][2] + Src1[1][2] * Src2[2][2] + Src1[1][3] * Src2[3][2];
	Target[1][3] = Src1[1][0] * Src2[0][3] + Src1[1][1] * Src2[1][3] + Src1[1][2] * Src2[2][3] + Src1[1][3] * Src2[3][3];

	Target[2][0] = Src1[2][0] * Src2[0][0] + Src1[2][1] * Src2[1][0] + Src1[2][2] * Src2[2][0] + Src1[2][3] * Src2[3][0];
	Target[2][1] = Src1[2][0] * Src2[0][1] + Src1[2][1] * Src2[1][1] + Src1[2][2] * Src2[2][1] + Src1[2][3] * Src2[3][1];
	Target[2][2] = Src1[2][0] * Src2[0][2] + Src1[2][1] * Src2[1][2] + Src1[2][2] * Src2[2][2] + Src1[2][3] * Src2[3][2];
	Target[2][3] = Src1[2][0] * Src2[0][3] + Src1[2][1] * Src2[1][3] + Src1[2][2] * Src2[2][3] + Src1[2][3] * Src2[3][3];

	Target[3][0] = Src1[3][0] * Src2[0][0] + Src1[3][1] * Src2[1][0] + Src1[3][2] * Src2[2][0] + Src1[3][3] * Src2[3][0];
	Target[3][1] = Src1[3][0] * Src2[0][1] + Src1[3][1] * Src2[1][1] + Src1[3][2] * Src2[2][1] + Src1[3][3] * Src2[3][1];
	Target[3][2] = Src1[3][0] * Src2[0][2] + Src1[3][1] * Src2[1][2] + Src1[3][2] * Src2[2][2] + Src1[3][3] * Src2[3][2];
	Target[3][3] = Src1[3][0] * Src2[0][3] + Src1[3][1] * Src2[1][3] + Src1[3][2] * Src2[2][3] + Src1[3][3] * Src2[3][3];
}

void RDP_Matrix_ModelviewLoad(float Mat[4][4])
{
	memcpy(Matrix.Model, Mat, 64);

	Gfx.Update |= CHANGED_MULT_MAT;
}

void RDP_Matrix_ModelviewMul(float Mat[4][4])
{
	float MatTemp[4][4];
	memcpy(MatTemp, Matrix.Model, 64);

	RDP_Matrix_MulMatrices(Mat, MatTemp, Matrix.Model);

	Gfx.Update |= CHANGED_MULT_MAT;
}

void RDP_Matrix_ModelviewPush()
{
	if(Matrix.ModelIndex == Matrix.ModelStackSize) return;

	memcpy(Matrix.ModelStack[Matrix.ModelIndex], Matrix.Model, 64);
	Matrix.ModelIndex++;
}

void RDP_Matrix_ModelviewPop(int PopTo)
{
	if(Matrix.ModelIndex > PopTo - 1) {
		Matrix.ModelIndex -= PopTo;
		memcpy(Matrix.Model, Matrix.ModelStack[Matrix.ModelIndex], 64);

		Gfx.Update |= CHANGED_MULT_MAT;
	} else {
		return;
	}
}

void RDP_Matrix_ProjectionLoad(float Mat[4][4])
{
	memcpy(Matrix.Proj, Mat, 64);

	Gfx.Update |= CHANGED_MULT_MAT;
}

void RDP_Matrix_ProjectionMul(float Mat[4][4])
{
	float MatTemp[4][4];
	memcpy(MatTemp, Matrix.Proj, 64);

	RDP_Matrix_MulMatrices(Mat, MatTemp, Matrix.Proj);

	Gfx.Update |= CHANGED_MULT_MAT;
}
