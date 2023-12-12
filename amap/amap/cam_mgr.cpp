#include "cam_mgr.h"
#include "config.h"


camManager::camManager()
{
	Mat4f_Identity(&this->view_mat);
	Mat4f_Identity(&this->proj_mat);
	Mat4f_Identity(&this->pv_mat);
}

camManager::camManager(sCtrlParam& params,int* vp){
	//this->preCtrl = params;
	Mat4f_Identity(&this->view_mat);
	Mat4f_Identity(&this->proj_mat);
	Mat4f_Identity(&this->pv_mat);
	Mat4f_Identity(&this->view_mat_plane);

	this->viewport[0] = vp[0];
	this->viewport[1] = vp[1];
	this->viewport[2] = vp[2];
	this->viewport[3] = vp[3];
}

camManager::~camManager()
{
}


void camManager::setViewport(int l, int b, int w, int h) 
{
	this->viewport[0] = l;
	this->viewport[1] = b;
	this->viewport[2] = w;
	this->viewport[3] = h;
}

int* camManager::getViewport() 
{
	return this->viewport;
}

int camManager::updateCamera() {
		
	Vec3d fo = {0,0,-1};
	Vec3d ri = {1,0,0};
	Vec3d up = {0,1,0};


	Mat4f_Identity(&view_mat_plane);
	Mat4f_Identity(&view_mat);

	this->view_mat_plane.col[0].x = ri.x;
	this->view_mat_plane.col[1].x = ri.y;
	this->view_mat_plane.col[2].x = ri.z;
	this->view_mat_plane.col[0].y = up.x;
	this->view_mat_plane.col[1].y = up.y;
	this->view_mat_plane.col[2].y = up.z; 
	this->view_mat_plane.col[0].z = -fo.x;
	this->view_mat_plane.col[1].z = -fo.y;
	this->view_mat_plane.col[2].z = -fo.z;

	this->view_mat.col[0].x = ri.x;
	this->view_mat.col[1].x = ri.y;
	this->view_mat.col[2].x = ri.z;
	this->view_mat.col[0].y = up.x;
	this->view_mat.col[1].y = up.y;
	this->view_mat.col[2].y = up.z;
	this->view_mat.col[0].z = -fo.x;
	this->view_mat.col[1].z = -fo.y;
	this->view_mat.col[2].z = -fo.z;

	Mat4f_Mul_Mat4f(&view_mat, &view_mat_plane, &pv_mat);
			
	return 0;
}

Mat4f* camManager::getViewMat() {
	return &this->view_mat;
}

int camManager::updateProjMat(sCtrlParam& param) {
	if (fabs(this->preCtrl.lon - param.lon) > 1e-5
		|| fabs(this->preCtrl.lat - param.lat) > 1e-5
		|| fabs(this->preCtrl.range - param.range) > 1e-5) {
		float l, r, b, t;
		float f = -5.0;
		float n = 5.0;

		l = param.lon - param.range / RING_RADIUS * SCREEN_WIDTH / 2;
		r = param.lon + param.range / RING_RADIUS * SCREEN_WIDTH / 2;
		b = param.lat - param.range / RING_RADIUS * SCREEN_HEIGHT / 2;
		t = param.lat + param.range / RING_RADIUS * SCREEN_HEIGHT / 2;

		Mat4f_Zero(&proj_mat);

		proj_mat.col[0].x = 2 / (r - l);
		proj_mat.col[1].y = 2 / (t - b);
		proj_mat.col[2].z = -2 / (f - n);
		proj_mat.col[3].x = -(r + l) / (r - l);
		proj_mat.col[3].y = -(t + b) / (t - b);
		proj_mat.col[3].z = -(f + n) / (f - n);
		proj_mat.col[3].w = 1;

		preCtrl = param;

		Mat4f_Mul_Mat4f(&view_mat, &view_mat_plane, &pv_mat);
	}
	return 0;
}

Mat4f* camManager::getProjMat() {
	return &this->proj_mat;
}

double camManager::Get_distance_from_frustum_plane(
	unsigned char r0, unsigned char r1, double sign0, double sign1, const Vec3d* p)
{
	const float* pv = &(pv_mat.col[0].x);
	double       a = sign0 * pv[r0] + sign1 * pv[r1];
	double       b = sign0 * pv[r0 + 4] + sign1 * pv[r1 + 4];
	double       c = sign0 * pv[r0 + 8] + sign1 * pv[r1 + 8];
	double       d = sign0 * pv[r0 + 12] + sign1 * pv[r1 + 12];
	double       l = sqrt(a * a + b * b + c * c);
	return (p->x * a + p->y * b + p->z * c + d) / l;
}

bool camManager::pointInFrumstum(const Vec3d* p, double radius) {
	bool ret = true;
	double l_dis = Get_distance_from_frustum_plane(3, 0, -1.0, -1.0, p);
	double r_dis = Get_distance_from_frustum_plane(3, 0, -1.0,  1.0, p);
	double b_dis = Get_distance_from_frustum_plane(3, 1, -1.0, -1.0, p);
	double t_dis = Get_distance_from_frustum_plane(3, 1, -1.0,  1.0, p);
	if (l_dis > radius || r_dis > radius || b_dis > radius || t_dis > radius) {
		return false;
	}
	return ret;
}

