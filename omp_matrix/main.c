#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

int check_running(double *x1, double *x2, int n) {
	int i;
	double eps = 1.0 / pow(10, 6);
	double max = -1, diff;
	for (i = 0; i < n; i++) {
		diff = fabs(x1[i] - x2[i]);
		if (diff > max) {
			max = diff;
		}
	}
	if (max < eps) {
		return 0;
	}
	return 1;
}

int debug_print(double *x, double *b, double **a, int n) {
	int i, j;
	printf("X:\n");
	for (i = 0; i < n; i++) {
		printf("%f ", x[i]);
	}
	printf("\nB:\n");
	for (i = 0; i < n; i++) {
		printf("%f ", b[i]);
	}
	printf("\nA:\n");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%f ", a[i][j]);
		}
		printf("\n");
	}
	return 0;
}

double sum(double *x, double *ai, int i, int n) {
	int j;
	double retval = 0.0, local = 0.0;
	for (j = 0; j < n; j++) {
		if (i != j) {
			retval += ai[j] * x[j];
		}
	}
	//printf("Sum retval: %f\n", retval);
	return retval;
}

int process(double *x, double *b, double **a, int n) {
	double *x_new = (double *) malloc(n * sizeof(double));
	int i;
	while (1) {
		#pragma omp  parallel for
		for (i = 0; i < n; i++) {
			x_new[i] = (b[i] - sum(x, a[i], i, n)) / a[i][i];
		}
		if (!check_running(x, x_new, n)) {
			break;
		}
		for (i = 0; i < n; i++) {
			x[i] = x_new[i];
		}
	}
	for (i = 0; i < n; i++) {
		x[i] = x_new[i];
	}
	//debug_print(x, b, a, n);
	free(x_new);
	return 0;
}
int init_x_b_a(double *x, double *b, double **a, int n) {
	int i, j;
	for (i = 0; i < n; i++) {
		x[i] = 0.0;
		b[i] = 2 * n - i;
		a[i] = (double *) malloc(n * sizeof(double));
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i == j) {
				a[i][j] = 2 * n - (n - i - 1);
			} else {
				a[i][j] = 1;
			}
		}
	}
	return 0;
}

int clear(double *x, double *b, double **a, int n) {
	int i;
	free(x);
	free(b);
	for (i = 0; i < n; i++) {
		free(a[i]);
	}
}

int main(int argc, char **argv) {
	double begin, end;
	int t_count = 1, n = 10;
	double *x, *b, **a;

	if (argc > 1) {
		t_count = (int) strtol(argv[1], NULL, 10);
	}
	if (argc > 2) {
		n = (int) strtol(argv[2], NULL, 10);
	}

	x = (double *)  malloc(n * sizeof(double));
	b = (double *)  malloc(n * sizeof(double));
	a = (double **) malloc(n * sizeof(double *));

	init_x_b_a(x, b, a, n);
	omp_set_num_threads(t_count);
	begin = omp_get_wtime();
	process(x, b, a, n);
	end = omp_get_wtime();
	printf("Time: %f\n", end - begin);

	clear(x, b, a, n);

	return 0;
}
