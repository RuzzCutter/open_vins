#ifndef OV_MSCKF_TRUST_ESTIMATOR_OPTIONS_H
#define OV_MSCKF_TRUST_ESTIMATOR_OPTIONS_H

#include <string>

#include "utils/print.h"

namespace ov_msckf {

struct TrustEstimatorOptions {

  bool enable = false;
  std::string log_filepath = "";
  double tau = 5.0;
  double skip_threshold = 0.15;
  double eps = 0.05;
  double r_max_multiplier = 100.0;
  int grid_size = 8;

  void print() {
    PRINT_DEBUG("    - trust_enable: %d\n", enable);
    PRINT_DEBUG("    - trust_log_filepath: %s\n", log_filepath.c_str());
    PRINT_DEBUG("    - trust_tau: %.2f\n", tau);
    PRINT_DEBUG("    - trust_skip_threshold: %.2f\n", skip_threshold);
    PRINT_DEBUG("    - trust_eps: %.3f\n", eps);
    PRINT_DEBUG("    - trust_r_max_multiplier: %.1f\n", r_max_multiplier);
    PRINT_DEBUG("    - trust_grid_size: %d\n", grid_size);
  }
};

} // namespace ov_msckf

#endif // OV_MSCKF_TRUST_ESTIMATOR_OPTIONS_H
