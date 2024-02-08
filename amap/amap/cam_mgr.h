#pragma once

#include "type_define.h"
#include "MAP_Math.h"

class camManager
{
public:
	camManager();
    camManager(sCtrlParam& params, int* vp);
	~camManager();
    void setViewport(int l, int b, int w, int h);
    int* getViewport();
    int updateCamera();
    Mat4f* getViewMat();
    int updateProjMat(sCtrlParam& param);
    Mat4f* getProjMat();
    bool pointInFrumstum(const Vec3d* p, double radius);

private:
    double Get_distance_from_frustum_plane(
        unsigned char r0, unsigned char r1, double sign0, double sign1, const Vec3d* p);
    int            viewport[4];
    Mat4f          view_mat;
    Mat4f          view_mat_plane;
    Mat4f          proj_mat;
    Mat4f          pv_mat;
    sCtrlParam preCtrl;
};