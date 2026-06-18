#include "TrustEstimator.h"

#include <algorithm>
#include <cmath>

namespace ov_msckf {

TrustEstimator::TrustEstimator(const TrustEstimatorOptions &options) : _options(options) {}

double TrustEstimator::clip(double v, double lo, double hi) { return std::max(lo, std::min(hi, v)); }

TrustMetrics TrustEstimator::compute(int n_features, int n_inliers, double e_repr, const std::vector<Eigen::Vector2f> &inlier_uvs,
                                     int img_w, int img_h, double tr_pose) const {

  TrustMetrics m;
  m.n_features = n_features;
  m.n_inliers = n_inliers;
  m.e_repr = e_repr;
  m.tr_pose = tr_pose;

  m.f1 = (n_features > 0) ? static_cast<double>(n_inliers) / static_cast<double>(n_features) : 0.0;
  m.f2 = (n_inliers > 0) ? std::exp(-e_repr / 2.0) : 0.0;

  const int gs = std::max(1, _options.grid_size);
  const int cells = gs * gs;
  std::vector<bool> grid(cells, false);
  int filled = 0;
  if (img_w > 0 && img_h > 0) {
    for (const auto &uv : inlier_uvs) {
      if (uv.x() < 0.0f || uv.y() < 0.0f)
        continue;
      int cx = static_cast<int>(uv.x() / static_cast<float>(img_w) * gs);
      int cy = static_cast<int>(uv.y() / static_cast<float>(img_h) * gs);
      cx = std::min(gs - 1, std::max(0, cx));
      cy = std::min(gs - 1, std::max(0, cy));
      const int idx = cy * gs + cx;
      if (!grid[idx]) {
        grid[idx] = true;
        filled++;
      }
    }
  }
  m.f3 = static_cast<double>(filled) / static_cast<double>(cells);

  const double tau = std::max(1e-6, _options.tau);
  const double tr_clamped = std::min(tr_pose, 10.0 * tau);
  m.f4 = std::exp(-tr_clamped / tau);

  m.c = clip(0.25 * (m.f1 + m.f2 + m.f3 + m.f4), 0.05, 1.0);

  const double denom = m.c + _options.eps;
  m.noise_scale = 1.0 / denom;
  if (m.noise_scale > _options.r_max_multiplier) {
    m.noise_scale = _options.r_max_multiplier;
  }

  m.skip_update = m.c < _options.skip_threshold;
  return m;
}

void TrustEstimator::log(double timestamp, const TrustMetrics &metrics) {

  _last = metrics;
  if (_options.log_filepath.empty())
    return;

  if (!_log.is_open()) {
    _log.open(_options.log_filepath, std::ios::out | std::ios::app);
    _log_header_written = false;
  }
  if (!_log.is_open())
    return;

  if (!_log_header_written) {
    _log << "timestamp,f1,f2,f3,f4,c,n_features,n_inliers,e_repr,tr_pose,skip_update,noise_scale\n";
    _log_header_written = true;
  }

  _log << timestamp << "," << metrics.f1 << "," << metrics.f2 << "," << metrics.f3 << "," << metrics.f4 << "," << metrics.c << ","
       << metrics.n_features << "," << metrics.n_inliers << "," << metrics.e_repr << "," << metrics.tr_pose << ","
       << (metrics.skip_update ? 1 : 0) << "," << metrics.noise_scale << "\n";
  _log.flush();
}

} // namespace ov_msckf
