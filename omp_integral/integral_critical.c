#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

double integrand(double x) {
  return 4 / (1 + x * x);
}

int main(int argc, char* argv[]) {
  double begin, end;

  int t_count = 1;
  if (argc != 1) {
    t_count = (int) strtol(argv[1],NULL,10);
  }
  omp_set_num_threads(t_count);

  double int_value = 0;
  int iters_count = pow(10, 8);

  begin = omp_get_wtime();
  #pragma omp parallel shared(int_value)
  {
    double local_sum = 0;

    #pragma omp for
    for(int i = 0; i < iters_count - 1; i++) {
      double left = i * 1.0 / iters_count;
      double right = (i + 1) * 1.0 / iters_count;
      local_sum += (integrand(left) + integrand(right)) / 2;
    }
    #pragma omp critical
    {
      int_value += local_sum;
    }
  }
  end = omp_get_wtime();
  printf("The value of the integral is %f\n", int_value / iters_count);
  printf("time: %f\n", end - begin);
  return 0;
}
