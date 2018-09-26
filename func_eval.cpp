#include "func_eval.h"

int InvertMatrix(int n, double* a, double* x)
{
    int i;
    int j;
    int k;
    double tmp1;
    double tmp2;
    
    // Create adjugate matrix
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            x[i * n + j] = (double)(i == j);
	
    // Reflection method's main body
    for (i = 0; i < n; ++i){
        tmp1 = 0.0;
	
	// tmp1 is a sum of squared column's elements
        for (j = i + 1; j < n; ++j)
            tmp1 += a[j * n + i] * a[j * n + i];
	
        tmp2 = sqrt(tmp1 + a[i * n + i] * a[i * n + i]);
	
	// Check if uninvertible
        if (tmp2 < 1e-100)
            return -1;

        a[i * n + i] -= tmp2;

        tmp1 = sqrt(tmp1 + a[i * n + i] * a[i * n + i]);
	
	// Check if diagonal element is zero
        if (tmp1 < 1e-100){
            a[i * n + i] += tmp2;
            continue;
        }

        tmp1 = 1.0 / tmp1;
	
	// Divide column's elements by tmp1
        for (j = i; j < n; ++j)
            a[j * n + i] *= tmp1;

	// Adding up to zero column's elements
        for (k = i + 1; k < n; ++k){
            tmp1 = 0.0;
            for (j = i; j < n; ++j)
                tmp1 += a[j * n + i] * a[j * n + k];

            tmp1 *= 2.0;
            for (j = i; j < n; ++j)
                a[j * n + k] -= tmp1 * a[j * n + i];
        }

        // Apply to adjugate matrix
        for (k = 0; k < n; ++k){
            tmp1 = 0.0;
            for (j = i; j < n; ++j)
                tmp1 += a[j * n + i] * x[j * n + k];

            tmp1 *= 2.0;
            for (j = i; j < n; ++j)
                x[j * n + k] -= tmp1 * a[j * n + i];
        }

        a[i * n + i] = tmp2;
    }

    // Inverse Gauss method
    for (k = 0; k < n; ++k)
        for (i = n - 1; i >= 0; --i){
            tmp1 = x[i * n + k];
            for (j = i + 1; j < n; ++j)
                tmp1 -= a[i * n + j] * x[j * n + k];
                x[i * n + k] = tmp1 / a[i * n + i];
        }

    return 0;
}