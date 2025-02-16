#pragma once


#define SCREEN_W 800
#define SCREEN_H 450


float TO_RADIAN(float degree);
float TO_DEGREE(float radian);

void DrawCircle3D_XZ(float3 center, float radius, int color, bool fill = false);
void DrawBox3D_XZ(float3 center, float half_w, float half_h, int color, bool fill = false);

