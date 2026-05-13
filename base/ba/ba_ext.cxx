#include "ba.h"
#include <vector>
#include <ceres/ceres.h>
#include <ceres/rotation.h>
#include <iostream>
#include <cmath>

/*----------------------
 Internal structs (C++ only)
----------------------*/
struct BundleAdjustData {
    const std::vector<std::vector<double>>* projs;
    int n_cameras;
    int n_points;
    const double* k;
};

struct ReprojectionError {
    ReprojectionError(double x, double y, int cam, int pt, BundleAdjustData* data)
        : x_(x), y_(y), cam_index_(cam), pt_index_(pt), data_(data) {}

    template <typename T>
    bool operator()(const T* const camera, const T* const point, T* residuals) const {
        T p[3];
        ceres::AngleAxisRotatePoint(camera, point, p);
        p[0] += camera[3]; p[1] += camera[4]; p[2] += camera[5];

        T fx = T(data_->k[0]);
        T fy = T(data_->k[4]);
        T cx = T(data_->k[2]);
        T cy = T(data_->k[5]);

        T xp = p[0]/p[2];
        T yp = p[1]/p[2];

        residuals[0] = fx*xp + cx - T(x_);
        residuals[1] = fy*yp + cy - T(y_);
        return true;
    }

    double x_, y_;
    int cam_index_, pt_index_;
    BundleAdjustData* data_;
};

/*----------------------
 Internal Ceres execution
----------------------*/
void ba_ext_exec_ceres(std::vector<double>& cameras,
                       std::vector<double>& points,
                       const std::vector<std::vector<double>>& projs,
                       const double* k)
{
    BundleAdjustData data{&projs, static_cast<int>(cameras.size()/6),
                          static_cast<int>(points.size()/3), k};

    ceres::Problem problem;

    for(int i=0;i<data.n_cameras;i++){
        for(int j=0;j<data.n_points;j++){
            double obs_x = projs[i][2*j];
            double obs_y = projs[i][2*j+1];

            ceres::CostFunction* cost_function =
                new ceres::AutoDiffCostFunction<ReprojectionError,2,6,3>(
                    new ReprojectionError(obs_x, obs_y, i, j, &data));

            problem.AddResidualBlock(cost_function, nullptr,
                                     &cameras[i*6], &points[j*3]);
        }
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_SCHUR;
    options.minimizer_progress_to_stdout = true;

    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    std::cout << summary.BriefReport() << std::endl;
}

/*----------------------
 Adapter GSL->Ceres
----------------------*/
void ba_ext_exec(gsl_vector *xout,
                 gsl_vector *params,
                 gsl_matrix *projs,
                 gsl_matrix *k)
{
    int n_points  = projs->size1;
    int n_cameras = projs->size2 / 2;

    std::vector<double> cameras(6*n_cameras);
    std::vector<double> points(3*n_points);

    // Extract points
    for(int i=0;i<n_points;i++){
        points[3*i+0] = gsl_vector_get(params, 3*i+0);
        points[3*i+1] = gsl_vector_get(params, 3*i+1);
        points[3*i+2] = gsl_vector_get(params, 3*i+2);
    }

    // Extract cameras
    int offset = 3*n_points;
    for(int i=0;i<n_cameras;i++)
        for(int j=0;j<6;j++)
            cameras[i*6+j] = gsl_vector_get(params, offset + i*6 + j);

    // Convert projections
    std::vector<std::vector<double>> projs_vec(n_cameras, std::vector<double>(2*n_points));
    for(int j=0;j<n_points;j++)
        for(int i=0;i<n_cameras;i++){
            projs_vec[i][2*j]   = gsl_matrix_get(projs,j,2*i);
            projs_vec[i][2*j+1] = gsl_matrix_get(projs,j,2*i+1);
        }

    // Convert K
    double k_arr[9];
    for(int i=0;i<9;i++) k_arr[i] = gsl_matrix_get(k, i/3, i%3);

    // Run Ceres BA
    ba_ext_exec_ceres(cameras, points, projs_vec, k_arr);

    // Copy back to xout
    for(int i=0;i<n_points;i++){
        gsl_vector_set(xout, 3*i+0, points[3*i+0]);
        gsl_vector_set(xout, 3*i+1, points[3*i+1]);
        gsl_vector_set(xout, 3*i+2, points[3*i+2]);
    }

    for(int i=0;i<n_cameras;i++)
        for(int j=0;j<6;j++)
            gsl_vector_set(xout, 3*n_points + i*6 + j, cameras[i*6+j]);
}
