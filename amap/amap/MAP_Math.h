/*----------------------------- FILE PROLOGUE -------------------------------*/
/******************************************************************************
 * COPYRIGHT: 
 *   2023
 *   SAVIC
 *   All Rights Reserved
 *
 * FILE NAME:
 *   MAP_Math.h
 *
 * DESCRIPTION:
 *   This file defines basic vector and matrix types and operations
 *
 * Create Date:
 *   2023-02-23
 *
******************************************************************************/
#ifndef _MAP_MATH_H_
#define _MAP_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------- FILE INCLUSION ------------------------------*/

/*---------------------------- MACRO DEFINITIONS ----------------------------*/
#if defined(_WIN32)
#    define _USE_MATH_DEFINES /**< for M_PI etc, before #include <math.h> */
#endif
#include <math.h>                /* must be after macro _USE_MATH_DEFINES */
#define M_DEG2RAD (M_PI / 180.0) /**< the multiply factor to convert degree to rad */
#define M_RAD2DEG (180.0 / M_PI) /**< the multiply factor to convert rad to degree */

/*---------------------------- TYPE DECLARATIONS ----------------------------*/
typedef struct Vec3d {
    double x; /**< first component */
    double y; /**< second component */
    double z; /**< third component */
} Vec3d;


typedef struct Vec_2d {
    double x; /**< first component */
    double y; /**< second component */
} Vec2d;


typedef struct Mat3d {
    Vec3d col[3]; /**< column vectors */
} Mat3d;


typedef struct Vec2f {
    float x; /**< first component */
    float y; /**< second component */
} Vec2f;


typedef struct Vec3f {
    float x; /**< first component */
    float y; /**< second component */
    float z; /**< third component */
} Vec3f;


typedef struct Vec4f {
    float x; /**< first component */
    float y; /**< second component */
    float z; /**< third component */
    float w; /**< fourth component */
} Vec4f;


typedef struct Mat3f {
    Vec3f col[3]; /**< column vectors */
} Mat3f;


typedef struct Mat4f {
    Vec4f col[4]; /**< column vectors */
} Mat4f;

/*-------------------------- VARIABLE DECLARATIONS --------------------------*/

/*-------------------------- FUNCTION DECLARATIONS --------------------------*/

/*****************************************************************************/
/* Zero the matrix                                                           */
/*****************************************************************************/
void Mat4f_Zero(Mat4f *m);

/*****************************************************************************/
/* Identify the matrix                                                      **/
/*****************************************************************************/
void Mat4f_Identity(Mat4f *m);

/*****************************************************************************/
/* 4x4 matrix multiplication                                                 */
/*****************************************************************************/
void Mat4f_Mul_Mat4f(const Mat4f *l, const Mat4f *r, Mat4f *out);

/*****************************************************************************/
/* matrix and vector multiplication                                          */
/*****************************************************************************/
void Mat4f_Mul_Vec4f(const Mat4f *l, const Vec4f *r, Vec4f *out);

/*****************************************************************************/
/* subtract vector r from l                                                  */
/*****************************************************************************/
void Vec3d_Sub(const Vec3d *l, const Vec3d *r, Vec3d *out);

/*****************************************************************************/
/* cross product of two vectors                                              */
/*****************************************************************************/
void Vec3d_Cross(const Vec3d *l, const Vec3d *r, Vec3d *out);

/*****************************************************************************/
/* Normalize double type vector                                              */
/*****************************************************************************/
void Vec3d_Norm(Vec3d *out);

/*****************************************************************************/
/* matrix and vector multiplication                                          */
/*****************************************************************************/
void Mat3d_Mul_Vec3d(const Mat3d *l, const Vec3d *r, Vec3d *out);

/*****************************************************************************/
/* vector rotate                                                             */
/*****************************************************************************/
void Rotate_By_Vec3d(const Vec3d *rotv, double angle, const Vec3d *in, Vec3d *out);

#ifdef __cplusplus
}
#endif

#endif /* _MAP_MATH_H_ */
