#ifndef CPU_AI_LOSSFUNCTIONS_H_
#define CPU_AI_LOSSFUNCTIONS_H_

#include <vector>

float mse(const std::vector<float>& values);

float half_mse(const std::vector<float>& values);

// Mean Absolute Error (L1 Loss)
float mae(const std::vector<float>& values);

// Binary Cross-Entropy Loss
float binaryCrossEntropy(const std::vector<float>& predicted, const std::vector<float>& target);

// Categorical Cross-Entropy Loss
float categoricalCrossEntropy(const std::vector<float>& predicted, const std::vector<float>& target);

// Huber Loss (smooth approximation of MAE)
float huberLoss(const std::vector<float>& predicted, const std::vector<float>& target, float delta = 1.0f);

#endif /* CPU_AI_LOSSFUNCTIONS_H_ */
