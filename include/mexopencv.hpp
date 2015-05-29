/**
 * @file mexopencv.hpp
 * @brief Global constant definitions
 * @author Kota Yamaguchi
 * @date 2012
 *
 * The header file for a MATLAB MEX-function that uses OpenCV library.
 * The file includes definition of MxArray class that converts between mxArray
 * and a couple of std:: and cv:: data types including cv::Mat.
 */
#ifndef __MEXOPENCV_HPP__
#define __MEXOPENCV_HPP__

#include "MxArray.hpp"

/**************************************************************\
*                       Global constants                       *
\**************************************************************/

/// Border type map for option processing
const ConstMap<std::string,int> BorderType = ConstMap<std::string,int>
    ("Constant",    cv::BORDER_CONSTANT)    // iiiiii|abcdefgh|iiiiiii for some i
    ("Replicate",   cv::BORDER_REPLICATE)   // aaaaaa|abcdefgh|hhhhhhh
    ("Reflect",     cv::BORDER_REFLECT)     // fedcba|abcdefgh|hgfedcb
    ("Wrap",        cv::BORDER_WRAP)        // cdefgh|abcdefgh|abcdefg
    ("Reflect101",  cv::BORDER_REFLECT_101) // gfedcb|abcdefgh|gfedcba
    ("Transparent", cv::BORDER_TRANSPARENT) // uvwxyz|absdefgh|ijklmno
    ("Default",     cv::BORDER_DEFAULT)     // same as "Reflect101"
    ("Isolated",    cv::BORDER_ISOLATED);   // do not look outside of ROI

/// Interpolation type map for option processing
const ConstMap<std::string,int> InterType = ConstMap<std::string,int>
    ("Nearest",  cv::INTER_NEAREST)  // nearest neighbor interpolation
    ("Linear",   cv::INTER_LINEAR)   // bilinear interpolation
    ("Cubic",    cv::INTER_CUBIC)    // bicubic interpolation
    ("Area",     cv::INTER_AREA)     // area-based (or super) interpolation
    ("Lanczos4", cv::INTER_LANCZOS4) // Lanczos interpolation over 8x8 neighborhood
    ("Max",      cv::INTER_MAX);
    //("WarpInverseMap", cv::WARP_INVERSE_MAP);

/// Thresholding type map for option processing
const ConstMap<std::string,int> ThreshType = ConstMap<std::string,int>
    ("Binary",    cv::THRESH_BINARY)
    ("BinaryInv", cv::THRESH_BINARY_INV)
    ("Trunc",     cv::THRESH_TRUNC)
    ("ToZero",    cv::THRESH_TOZERO)
    ("ToZeroInv", cv::THRESH_TOZERO_INV)
    ("Mask",      cv::THRESH_MASK);
    //("Otsu",    cv::THRESH_OTSU);

/// Distance types for Distance Transform and M-estimators
const ConstMap<std::string,int> DistType = ConstMap<std::string,int>
    ("User",   cv::DIST_USER)
    ("L1",     cv::DIST_L1)
    ("L2",     cv::DIST_L2)
    ("C",      cv::DIST_C)
    ("L12",    cv::DIST_L12)
    ("Fair",   cv::DIST_FAIR)
    ("Welsch", cv::DIST_WELSCH)
    ("Huber",  cv::DIST_HUBER);

/// Line type for drawing
const ConstMap<std::string,int> LineType = ConstMap<std::string,int>
    ("4",  cv::LINE_4)
    ("8",  cv::LINE_8)
    ("AA", cv::LINE_AA);

/// Thickness type for drawing
const ConstMap<std::string,int> ThicknessType = ConstMap<std::string,int>
    ("Filled", cv::FILLED);

/// Font faces for drawing
const ConstMap<std::string,int> FontFace = ConstMap<std::string,int>
    ("HersheySimplex",       cv::FONT_HERSHEY_SIMPLEX)
    ("HersheyPlain",         cv::FONT_HERSHEY_PLAIN)
    ("HersheyDuplex",        cv::FONT_HERSHEY_DUPLEX)
    ("HersheyComplex",       cv::FONT_HERSHEY_COMPLEX)
    ("HersheyTriplex",       cv::FONT_HERSHEY_TRIPLEX)
    ("HersheyComplexSmall",  cv::FONT_HERSHEY_COMPLEX_SMALL)
    ("HersheyScriptSimplex", cv::FONT_HERSHEY_SCRIPT_SIMPLEX)
    ("HersheyScriptComplex", cv::FONT_HERSHEY_SCRIPT_COMPLEX);

/// Font styles for drawing
const ConstMap<std::string,int> FontStyle = ConstMap<std::string,int>
    ("Regular", 0)
    ("Italic",  cv::FONT_ITALIC);

/// Norm type map for option processing
const ConstMap<std::string,int> NormType = ConstMap<std::string,int>
    ("Inf",      cv::NORM_INF)
    ("L1",       cv::NORM_L1)
    ("L2",       cv::NORM_L2)
    ("L2Sqr",    cv::NORM_L2SQR)
    ("Hamming",  cv::NORM_HAMMING)
    ("Hamming2", cv::NORM_HAMMING2);

/**************************************************************\
*                       Helper Functions                       *
\**************************************************************/

/// Alias for input/ouput arguments number check
inline void nargchk(bool cond)
{
    if (!cond) {
        mexErrMsgIdAndTxt("mexopencv:error", "Wrong number of arguments");
    }
}

/**************************************************************\
*           Conversion Functions: MxArray to vector            *
\**************************************************************/

/** Convert an MxArray to std::vector<cv::Point_<T>>
 *
 * @param arr MxArray object. In one of the following forms:
 * - a cell-array of 2D points (2-element vectors) of length \c N,
 *   e.g: <tt>{[x,y], [x,y], ...}</tt>
 * - a numeric matrix of size \c Nx2, \c Nx1x2, or \c 1xNx2 in the form:
 *   <tt>[x,y; x,y; ...]</tt> or <tt>cat(3, [x,y], [x,y], ...)</tt>
 * @return vector of 2D points of size \c N
 *
 * Example:
 * @code
 * MxArray cellArray(prhs[0]);
 * vector<Point2d> vp = MxArrayToVectorPoint<double>(cellArray);
 * @endcode
 */
template <typename T>
std::vector<cv::Point_<T> > MxArrayToVectorPoint(const MxArray& arr)
{
    std::vector<cv::Point_<T> > vp;
    if (arr.isNumeric()) {
        if (arr.numel() == 2)
            vp.push_back(arr.toPoint_<T>());
        else
            arr.toMat(cv::DataType<T>::depth).reshape(2, 0).copyTo(vp);
    }
    else if (arr.isCell()) {
        /*
        std::vector<MxArray> va(arr.toVector<MxArray>());
        vp.reserve(va.size());
        for (std::vector<MxArray>::const_iterator it = va.begin(); it != va.end(); ++it)
            vp.push_back(it->toPoint_<T>());
        */
        vp = arr.toVector(
            std::const_mem_fun_ref_t<cv::Point_<T>, MxArray>(
                &MxArray::toPoint_<T>));
    }
    else
        mexErrMsgIdAndTxt("mexopencv:error",
            "Unable to convert MxArray to std::vector<cv::Point_<T>>");
    return vp;
}

/** Convert an MxArray to std::vector<cv::Point3_<T>>
 *
 * @param arr MxArray object. In one of the following forms:
 * - a cell-array of 3D points (3-element vectors) of length \c N,
 *   e.g: <tt>{[x,y,z], [x,y,z], ...}</tt>
 * - a numeric matrix of size \c Nx3, \c Nx1x3, or \c 1xNx3 in the form:
 *   <tt>[x,y,z; x,y,z; ...]</tt> or <tt>cat(3, [x,y,z], [x,y,z], ...)</tt>
 * @return vector of 3D points of size \c N
 *
 * Example:
 * @code
 * MxArray cellArray(prhs[0]);
 * vector<Point3f> vp = MxArrayToVectorPoint3<float>(cellArray);
 * @endcode
 */
template <typename T>
std::vector<cv::Point3_<T> > MxArrayToVectorPoint3(const MxArray& arr)
{
    std::vector<cv::Point3_<T> > vp;
    if (arr.isNumeric()) {
        if (arr.numel() == 3)
            vp.push_back(arr.toPoint3_<T>());
        else
            arr.toMat(cv::DataType<T>::depth).reshape(3, 0).copyTo(vp);
    }
    else if (arr.isCell()) {
        /*
        std::vector<MxArray> va(arr.toVector<MxArray>());
        vp.reserve(va.size());
        for (std::vector<MxArray>::const_iterator it = va.begin(); it != va.end(); ++it)
            vp.push_back(it->toPoint3_<T>());
        */
        vp = arr.toVector(
            std::const_mem_fun_ref_t<cv::Point3_<T>, MxArray>(
                &MxArray::toPoint3_<T>));
    }
    else
        mexErrMsgIdAndTxt("mexopencv:error",
            "Unable to convert MxArray to std::vector<cv::Point3_<T>>");
    return vp;
}

/**************************************************************\
*      Conversion Functions: MxArray to vector of vectors      *
\**************************************************************/

/** Convert an MxArray to std::vector<std::vector<cv::Point_<T>>>
 *
 * @param arr MxArray object. In one of the following forms:
 * - a cell-array of cell-arrays of 2D points (2-element vectors),
 *   e.g: <tt>{{[x,y], [x,y], ..}, {[x,y], [x,y], ..}, ...}</tt>
 * - a cell-array of numeric matrices of size \c Mx2, \c Mx1x2, or \c 1xMx2,
 *   e.g: <tt>{[x,y; x,y; ...], [x,y; x,y; ...], ...}</tt> or
 *   <tt>{cat(3, [x,y], [x,y], ...), cat(3, [x,y], [x,y], ...), ...}</tt>
 * @return vector of vectors of 2D points
 *
 * Example:
 * @code
 * MxArray cellArray(prhs[0]);
 * vector<vector<Point2d>> vvp = MxArrayToVectorVectorPoint<double>(cellArray);
 * @endcode
 */
template <typename T>
std::vector<std::vector<cv::Point_<T> > > MxArrayToVectorVectorPoint(const MxArray& arr)
{
    std::vector<MxArray> vva(arr.toVector<MxArray>());
    std::vector<std::vector<cv::Point_<T> > > vvp;
    vvp.reserve(vva.size());
    for (std::vector<MxArray>::const_iterator it = vva.begin(); it != vva.end(); ++it) {
        /*
        std::vector<MxArray> va(it->toVector<MxArray());
        std::vector<cv::Point_<T> > vp;
        for (std::vector<MxArray>::const_iterator jt = va.begin(); jt != va.end(); ++jt) {
            vp.push_back(jt->toPoint_<T>());
        }
        vvp.push_back(vp);
        */
        vvp.push_back(MxArrayToVectorPoint<T>(*it));
    }
    return vvp;
}

/** Convert an MxArray to std::vector<std::vector<cv::Point3_<T>>>
 *
 * @param arr MxArray object. In one of the following forms:
 * - a cell-array of cell-arrays of 3D points (3-element vectors),
 *   e.g: <tt>{{[x,y,z], [x,y,z], ..}, {[x,y,z], [x,y,z], ..}, ...}</tt>
 * - a cell-array of numeric matrices of size \c Mx3, \c Mx1x3, or \c 1xMx3,
 *   e.g: <tt>{[x,y,z; x,y,z; ...], [x,y,z; x,y,z; ...], ...}</tt> or
 *   <tt>{cat(3, [x,y,z], [x,y,z], ...), cat(3, [x,y,z], [x,y,z], ...), ...}</tt>
 * @return vector of vectors of 3D points
 *
 * Example:
 * @code
 * MxArray cellArray(prhs[0]);
 * vector<vector<Point3d>> vvp = MxArrayToVectorVectorPoint3<double>(cellArray);
 * @endcode
 */
template <typename T>
std::vector<std::vector<cv::Point3_<T> > > MxArrayToVectorVectorPoint3(const MxArray& arr)
{
    std::vector<MxArray> vva(arr.toVector<MxArray>());
    std::vector<std::vector<cv::Point3_<T> > > vvp;
    vvp.reserve(vva.size());
    for (std::vector<MxArray>::const_iterator it = vva.begin(); it != vva.end(); ++it) {
        /*
        std::vector<MxArray> va(it->toVector<MxArray());
        std::vector<cv::Point3_<T> > vp;
        for (std::vector<MxArray>::const_iterator jt = va.begin(); jt != va.end(); ++jt) {
            vp.push_back(jt->toPoint3_<T>());
        }
        vvp.push_back(vp);
        */
        vvp.push_back(MxArrayToVectorPoint3<T>(*it));
    }
    return vvp;
}

#endif
