#ifndef OV_MSCKF_TRUST_ESTIMATOR_H
#define OV_MSCKF_TRUST_ESTIMATOR_H

#include <Eigen/Eigen>
#include <fstream>
#include <vector>

#include "TrustEstimatorOptions.h"

namespace ov_msckf {

struct TrustMetrics {
  double f1 = 0.0;
  double f2 = 0.0;
  double f3 = 0.0;
  double f4 = 0.0;
  double c = 1.0;
  int n_features = 0;
  int n_inliers = 0;
  double e_repr = 0.0;
  double tr_pose = 0.0;
  bool skip_update = false;
  double noise_scale = 1.0;
};

class TrustEstimator {

public:
  explicit TrustEstimator(const TrustEstimatorOptions &options);

  TrustMetrics compute(int n_features, int n_inliers, double e_repr, const std::vector<Eigen::Vector2f> &inlier_uvs, int img_w,
                       int img_h, double tr_pose) const;

  void log(double timestamp, const TrustMetrics &metrics);

  const TrustMetrics &last_metrics() const { return _last; }

private:
  static double clip(double v, double lo, double hi);

  TrustEstimatorOptions _options;
  TrustMetrics _last;
  mutable std::ofstream _log;
  mutable bool _log_header_written = false;
};

} // namespace ov_msckf

#endif // OV_MSCKF_TRUST_ESTIMATOR_H
