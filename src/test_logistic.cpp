/**
Implements tests on the logistic regression algorithm
Scope:

1. Basic correctness of functions defined
  - Polynomial evaluation
  - Forward pass

2. Outputs of model in terms of metrics

Basic approach is to encapsulate each "test" in a function and when function is run, it outputs
whatever metric and also saves it to disk

*/

#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <ctime>
#include <string>

#include "encryption.hpp"
#include "alu.hpp"
#include "matrix.hpp"
#include "logistic.hpp"
#include "io.hpp"
#include "tfhe_params.hpp"
#include "metrics.hpp"
using namespace std;


/* Adds '_epochtime' to input string*/
string toTimestamped(string name) {
  return name + "_" + to_string(time(0));
}

/* Runs model on data and measures precision, recall, accuracy and saves to csv */
template<typename T>
void gradeModel(string weight_path, string coefs_path, string data_path, int dim, size_t precision, const TFheGateBootstrappingSecretKeySet* sk) {
  size_t size = sizeof(T) * 8;

  const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

  vector<vector<double>> data = readFile(data_path);

  size_t N = data.size();  // number of data points

  // encrypt data
  LweSample ***enc_data = new LweSample**[N];
  for(int i = 0; i < N; i++) {
    enc_data[i] = new LweSample*[dim];
    for(int j = 0; j < dim; j++) {
      enc_data[i][j] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
      encrypt<T>(enc_data[i][j], data[i][j], sk);
    }
  }

  // define logistic regression model and load weights and polynomial coefficients
  ApproxLogRegression lr(weight_path, coefs_path, dim, ck, size, precision);
  // run logistic regression model
  LweSample *y = new_gate_bootstrapping_ciphertext_array(size,ck-> params);
  vector<double> predicted_classes;
  vector<double> ground_truth_classes;
  int cls, res;

  double tick,
         agg = 0;
  int num_iters = N;
  for(int i = 0; i < num_iters; i++) {
    printf("Job [%d/%d]\n", i+1, N);
    tick = omp_get_wtime();
    lr.predict(y, enc_data[i]);
    res = decrypt<T>(y, sk);
    double proba = ((float) res ) / 100;
    // cout << "Raw: " << res << endl;
    // cout << "Probability: " << proba << endl;
    cls = proba >= 0.5 ? 1 : 0;  // threshold on 0.5
    agg += omp_get_wtime() - tick;
    predicted_classes.push_back(cls);
    ground_truth_classes.push_back((int) data[i][dim]);
    // printf("Predicted class: %d\n", (int) cls);
    // printf("Ground truth: %d\n", (int) data[i][d]);
  }
  agg /= num_iters;
  printf("Runtime per prediction: %.2fs\n", agg);

  // save raw results to disk as well as metrics
  string pred_csv_path = toTimestamped("logistic_lr_predicted_classes") + ".csv",
         gt_csv_path = toTimestamped("logistic_lr_ground_truth_classes") + ".csv";

  cout << "Saving class predictions to " << pred_csv_path << endl;
  writeFile(predicted_classes, pred_csv_path);
  cout << "Saving ground truth classes to " << gt_csv_path << endl;
  writeFile(ground_truth_classes, gt_csv_path);

  double metric_precision = calculatePrecision(ground_truth_classes, predicted_classes),
         recall = calculateRecall(ground_truth_classes, predicted_classes),
         accuracy = calculateAccuracy(ground_truth_classes, predicted_classes);
  vector<double> metric_vec = {metric_precision, recall, accuracy, agg};
  string header = "precision,recall,accuracy,secs_per_pred";
  string metric_path = toTimestamped("performance_metrics_logistic_regression") + ".csv";
  writeFile(metric_vec, metric_path, ',', header);

  printf("Precision: %.2f\nRecall: %.2f\nAccuracy: %.2f\n", metric_precision, recall, accuracy);
}

/* Just runs all the tests */
int main() {

    omp_set_nested(1);
    omp_set_num_threads(NUM_THREADS);

    string weight_path = "/home/munsanje/Uni/Thesis/code/sign_weights_wiscon_real_linapprox_weights.csv",
           coefs_path = "/home/munsanje/Uni/Thesis/code/linear_logapprox_coefs_neg1to1.csv",
           data_path = "/home/munsanje/Uni/Thesis/data/testset_int8_breast-cancer-wisconsin.data";
    int dim = 9, scale_factor = 10;
    typedef int8_t num_type;
    size_t size = sizeof(num_type) * 8;

    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(MINIMUM_LAMBDA);
    const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);

    gradeModel<num_type>(weight_path, coefs_path, data_path, dim, scale_factor, sk);


}
