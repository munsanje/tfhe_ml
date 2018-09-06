#include <vector>
#include "logistic.hpp"
#include "io.hpp"

using namespace std;


ApproxLogRegression::ApproxLogRegression(char* weight_path, char* coefs_path, int dim, TFheGateBootstrappingCloudKeySet* ck, size_t size) {
  vector<vector<double>> weights = readFile(weight_path);
  
}
