/*----------------------------- FILE PROLOGUE -------------------------------*/
/******************************************************************************
 * COPYRIGHT:
 *   2023
 *   SAVIC
 *   All Rights Reserved
 *
 * FILE NAME:
 *   MAP_Math.c
 *
 * FILE DESCRIPTION:
 *   This file defines vector and matrix operations
 *
 * DEFINED FUNCTION:
 *   Mat3d_Mul_Vec3d
 *   Mat4f_Identity
 *   Mat4f_Mul_Mat4f
 *   Mat4f_Mul_Vec4f
 *   Mat4f_Zero
 *   Rotate_By_Vec3d
 *   Vec3d_Cross
 *   Vec3d_Norm
 *   Vec3d_Sub
 *
 * DESIGN NOTES:
 *   None
 *
 * Create Date:
 *   2023-3-9
 *
 *****************************************************************************/
/*----------------------------- FILE INCLUSION ------------------------------*/
#include "MAP_Math.h"
#include <math.h>
#include <stddef.h>

/*---------------------------- MACRO DEFINITIONS ----------------------------*/

/*---------------------------- TYPE DECLARATIONS ----------------------------*/

/*-------------------------- VARIABLE DEFINITIONS ---------------------------*/

/*-------------------------- FUNCTION DECLARATIONS --------------------------*/

/*-------------------------- FUNCTION DEFINITIONS ---------------------------*/

/******************************************************************************
 * FUNCTION NAME:
 *   Mat4f_Zero
 *
 * DESCRIPTION:
 *   Zero the matrix
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     None
 *
 *   OUTPUT:
 *     None
 *
 *   INPUT/OUTPUT:
 *     m: the matrix which need to be zeroed
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Mat4f_Zero(Mat4f *m)
{
    m->col[0].x = 0;
    m->col[0].y = 0;
    m->col[0].z = 0;
    m->col[0].w = 0;
    m->col[1].x = 0;
    m->col[1].y = 0;
    m->col[1].z = 0;
    m->col[1].w = 0;
    m->col[2].x = 0;
    m->col[2].y = 0;
    m->col[2].z = 0;
    m->col[2].w = 0;
    m->col[3].x = 0;
    m->col[3].y = 0;
    m->col[3].z = 0;
    m->col[3].w = 0;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Mat4f_Identity
 *
 * DESCRIPTION:
 *   Identify the matrix
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     None
 *
 *   OUTPUT:
 *     None
 *
 *   INPUT/OUTPUT:
 *     m: the matrix which need to be identified
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Mat4f_Identity(Mat4f *m)
{
    m->col[0].x = 1.0;
    m->col[0].y = 0.0;
    m->col[0].z = 0.0;
    m->col[0].w = 0.0;
    m->col[1].x = 0.0;
    m->col[1].y = 1.0;
    m->col[1].z = 0.0;
    m->col[1].w = 0.0;
    m->col[2].x = 0.0;
    m->col[2].y = 0.0;
    m->col[2].z = 1.0;
    m->col[2].w = 0.0;
    m->col[3].x = 0.0;
    m->col[3].y = 0.0;
    m->col[3].z = 0.0;
    m->col[3].w = 1.0;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Mat4f_Mul_Mat4f
 *
 * DESCRIPTION:
 *   4x4 matrix multiplication
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     l: left matrix
 *     r: right matrix
 *
 *   OUTPUT:
 *     out: output matrix
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Mat4f_Mul_Mat4f(const Mat4f *l, const Mat4f *r, Mat4f *out)
{
    out->col[0].x = l->col[0].x * r->col[0].x + l->col[1].x * r->col[0].y +
                    l->col[2].x * r->col[0].z + l->col[3].x * r->col[0].w;
    out->col[0].y = l->col[0].y * r->col[0].x + l->col[1].y * r->col[0].y +
                    l->col[2].y * r->col[0].z + l->col[3].y * r->col[0].w;
    out->col[0].z = l->col[0].z * r->col[0].x + l->col[1].z * r->col[0].y +
                    l->col[2].z * r->col[0].z + l->col[3].z * r->col[0].w;
    out->col[0].w = l->col[0].w * r->col[0].x + l->col[1].w * r->col[0].y +
                    l->col[2].w * r->col[0].z + l->col[3].w * r->col[0].w;

    out->col[1].x = l->col[0].x * r->col[1].x + l->col[1].x * r->col[1].y +
                    l->col[2].x * r->col[1].z + l->col[3].x * r->col[1].w;
    out->col[1].y = l->col[0].y * r->col[1].x + l->col[1].y * r->col[1].y +
                    l->col[2].y * r->col[1].z + l->col[3].y * r->col[1].w;
    out->col[1].z = l->col[0].z * r->col[1].x + l->col[1].z * r->col[1].y +
                    l->col[2].z * r->col[1].z + l->col[3].z * r->col[1].w;
    out->col[1].w = l->col[0].w * r->col[1].x + l->col[1].w * r->col[1].y +
                    l->col[2].w * r->col[1].z + l->col[3].w * r->col[1].w;

    out->col[2].x = l->col[0].x * r->col[2].x + l->col[1].x * r->col[2].y +
                    l->col[2].x * r->col[2].z + l->col[3].x * r->col[2].w;
    out->col[2].y = l->col[0].y * r->col[2].x + l->col[1].y * r->col[2].y +
                    l->col[2].y * r->col[2].z + l->col[3].y * r->col[2].w;
    out->col[2].z = l->col[0].z * r->col[2].x + l->col[1].z * r->col[2].y +
                    l->col[2].z * r->col[2].z + l->col[3].z * r->col[2].w;
    out->col[2].w = l->col[0].w * r->col[2].x + l->col[1].w * r->col[2].y +
                    l->col[2].w * r->col[2].z + l->col[3].w * r->col[2].w;

    out->col[3].x = l->col[0].x * r->col[3].x + l->col[1].x * r->col[3].y +
                    l->col[2].x * r->col[3].z + l->col[3].x * r->col[3].w;
    out->col[3].y = l->col[0].y * r->col[3].x + l->col[1].y * r->col[3].y +
                    l->col[2].y * r->col[3].z + l->col[3].y * r->col[3].w;
    out->col[3].z = l->col[0].z * r->col[3].x + l->col[1].z * r->col[3].y +
                    l->col[2].z * r->col[3].z + l->col[3].z * r->col[3].w;
    out->col[3].w = l->col[0].w * r->col[3].x + l->col[1].w * r->col[3].y +
                    l->col[2].w * r->col[3].z + l->col[3].w * r->col[3].w;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Mat4f_Mul_Vec4f
 *
 * DESCRIPTION:
 *   matrix and vector multiplication
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     l: left matrix
 *     r: right vector
 *
 *   OUTPUT:
 *     out: output vector
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Mat4f_Mul_Vec4f(const Mat4f *l, const Vec4f *r, Vec4f *out)
{
    out->x = l->col[0].x * r->x + l->col[1].x * r->y + l->col[2].x * r->z + l->col[3].x * r->w;
    out->y = l->col[0].y * r->x + l->col[1].y * r->y + l->col[2].y * r->z + l->col[3].y * r->w;
    out->z = l->col[0].z * r->x + l->col[1].z * r->y + l->col[2].z * r->z + l->col[3].z * r->w;
    out->w = l->col[0].w * r->x + l->col[1].w * r->y + l->col[2].w * r->z + l->col[3].w * r->w;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Vec3d_Sub
 *
 * DESCRIPTION:
 *   subtract vector r from l
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     l: minuend vector
 *     r: subtrahend vector
 *
 *   OUTPUT:
 *     out: output vector
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Vec3d_Sub(const Vec3d *l, const Vec3d *r, Vec3d *out)
{
    out->x = l->x - r->x;
    out->y = l->y - r->y;
    out->z = l->z - r->z;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Vec3d_Cross
 *
 * DESCRIPTION:
 *   cross product of two vectors
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     l: left vector
 *     r: right vector
 *
 *   OUTPUT:
 *     out: output vector
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Vec3d_Cross(const Vec3d *l, const Vec3d *r, Vec3d *out)
{
    double a1 = l->x;
    double a2 = l->y;
    double a3 = l->z;
    double b1 = r->x;
    double b2 = r->y;
    double b3 = r->z;
    out->x    = a2 * b3 - a3 * b2;
    out->y    = a3 * b1 - a1 * b3;
    out->z    = a1 * b2 - a2 * b1;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Vec3d_Norm
 *
 * DESCRIPTION:
 *   Normalize double type vector
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     None
 *
 *   OUTPUT:
 *     None
 *
 *   INPUT/OUTPUT:
 *     out: pointer to the input and output vector
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Vec3d_Norm(Vec3d *out)
{
    double x   = out->x;
    double y   = out->y;
    double z   = out->z;
    double len = sqrt(x * x + y * y + z * z);
    out->x     = x / len;
    out->y     = y / len;
    out->z     = z / len;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Mat3d_Mul_Vec3d
 *
 * DESCRIPTION:
 *   matrix and vector multiplication
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     l: left matrix
 *     r: right vector
 *     
 *
 *   OUTPUT:
 *     out: output vector
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Mat3d_Mul_Vec3d(const Mat3d *l, const Vec3d *r, Vec3d *out)
{
    out->x = l->col[0].x * r->x + l->col[1].x * r->y + l->col[2].x * r->z;
    out->y = l->col[0].y * r->x + l->col[1].y * r->y + l->col[2].y * r->z;
    out->z = l->col[0].z * r->x + l->col[1].z * r->y + l->col[2].z * r->z;
}


/******************************************************************************
 * FUNCTION NAME:
 *   Rotate_By_Vec3d
 *
 * DESCRIPTION:
 *   vector rotate
 *
 * INTERFACE:
 *   GLOBAL DATA:
 *     None
 *
 *   INPUT:
 *     rotv: axis along which to do the rotation
 *     angle: rotate angle in degree
 *     in: input vector to rotate
 *     
 *
 *   OUTPUT:
 *     out: output vector
 *
 *   INPUT/OUTPUT:
 *     None
 *
 * RETURN CODE:
 *   None
 *
 * NOTES:
 *
 *****************************************************************************/
void Rotate_By_Vec3d(const Vec3d *rotv, double angle, const Vec3d *in, Vec3d *out)
{
    angle *= M_DEG2RAD;
    Vec3d norm = *rotv;
    Vec3d_Norm(&norm);
    double ux = norm.x;
    double uy = norm.y;
    double uz = norm.z;
    double cs = cos(angle);
    double ss = sin(angle);

    Mat3d rotM;
    rotM.col[0].x = cs + ux * ux * (1 - cs);
    rotM.col[0].y = uy * ux * (1 - cs) + uz * ss;
    rotM.col[0].z = uz * ux * (1 - cs) - uy * ss;
    rotM.col[1].x = ux * uy * (1 - cs) - uz * ss;
    rotM.col[1].y = cs + uy * uy * (1 - cs);
    rotM.col[1].z = uz * uy * (1 - cs) + ux * ss;
    rotM.col[2].x = ux * uz * (1 - cs) + uy * ss;
    rotM.col[2].y = uy * uz * (1 - cs) - ux * ss;
    rotM.col[2].z = cs + uz * uz * (1 - cs);

    Mat3d_Mul_Vec3d(&rotM, in, out);
}