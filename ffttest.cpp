#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>
#include "fft.h"

using std::cout;
using std::endl;
using std::vector;

// Private function prototypes
static void testFft(int n);
static void testConvolution(int n);
static void naiveDft(const vector<double> &inreal, const vector<double> &inimag, vector<double> &outreal, vector<double> &outimag, bool inverse);
static void naiveConvolve(const vector<double> &xreal, const vector<double> &ximag, const vector<double> &yreal, const vector<double> &yimag, vector<double> &outreal, vector<double> &outimag);
static double log10RmsErr(const vector<double> &xreal, const vector<double> &ximag, const vector<double> &yreal, const vector<double> &yimag);
static void randomReals(vector<double> &vec);

static double maxLogError = -INFINITY;

/* Main and test functions */

int main(int argc, char **argv) {
	testFft(16);
	/*srand(time(nullptr));
	
	// Test power-of-2 size FFTs
	for (int i = 0; i <= 12; i++)
		testFft(1 << i);
	
	// Test small size FFTs
	for (int i = 0; i < 30; i++)
		testFft(i);
	
	cout << endl;
	cout << "Max log err = " << std::setprecision(3) << maxLogError << endl;
	cout << "Test " << (maxLogError < -10 ? "passed" : "failed") << endl;
	
	*/return EXIT_SUCCESS;
}


static void testFft(int n) {
	vector <double> inputreal(n,0);
	vector <double> inputimag(n,0);

	inputreal[0] = 1;

	for (int i = 0; i < n; i++) cout << "Input: " << inputreal[i] << endl;
	//randomReals(inputreal);
	//randomReals(inputimag);
	
	/*vector<double> refoutreal(n);
	vector<double> refoutimag(n);
	naiveDft(inputreal, inputimag, refoutreal, refoutimag, false);
	*/
	Fft::transform(inputreal, inputimag);

	for(int i = 0; i < n; i++) cout << inputreal[i]  << "," << inputimag[i] << endl;
	
}


static void testConvolution(int n) {
	vector<double> input0real(n);
	vector<double> input0imag(n);
	vector<double> input1real(n);
	vector<double> input1imag(n);
	randomReals(input0real);
	randomReals(input0imag);
	randomReals(input1real);
	randomReals(input1imag);
	
	vector<double> refoutreal(n);
	vector<double> refoutimag(n);
	naiveConvolve(input0real, input0imag, input1real, input1imag, refoutreal, refoutimag);
	
	vector<double> actualoutreal(n);
	vector<double> actualoutimag(n);
	Fft::convolve(input0real, input0imag, input1real, input1imag, actualoutreal, actualoutimag);
	
	cout << "convsize=" << std::setw(4) << std::setfill(' ') << n << "  "
	     << "logerr=" << std::setw(5) << std::setprecision(3) << std::setiosflags(std::ios::showpoint) << log10RmsErr(refoutreal, refoutimag, actualoutreal, actualoutimag) << endl;
}


/* Naive reference computation functions */

static void naiveDft(const vector<double> &inreal, const vector<double> &inimag, vector<double> &outreal, vector<double> &outimag, bool inverse) {
	int n = inreal.size();
	double coef = (inverse ? 2 : -2) * M_PI;
	for (int k = 0; k < n; k++) {  // For each output element
		double sumreal = 0;
		double sumimag = 0;
		for (int t = 0; t < n; t++) {  // For each input element
			double angle = coef * ((long long)t * k % n) / n;
			sumreal += inreal[t]*cos(angle) - inimag[t]*sin(angle);
			sumimag += inreal[t]*sin(angle) + inimag[t]*cos(angle);
		}
		outreal[k] = sumreal;
		outimag[k] = sumimag;
	}
}


static void naiveConvolve(const vector<double> &xreal, const vector<double> &ximag, const vector<double> &yreal, const vector<double> &yimag, vector<double> &outreal, vector<double> &outimag) {
	int n = xreal.size();
	for (int i = 0; i < n; i++) {
		double sumreal = 0;
		double sumimag = 0;
		for (int j = 0; j < n; j++) {
			int k = (i - j + n) % n;
			sumreal += xreal[k] * yreal[j] - ximag[k] * yimag[j];
			sumimag += xreal[k] * yimag[j] + ximag[k] * yreal[j];
		}
		outreal[i] = sumreal;
		outimag[i] = sumimag;
	}
}

/* Utility functions */

static double log10RmsErr(const vector<double> &xreal, const vector<double> &ximag, const vector<double> &yreal, const vector<double> &yimag) {
	int n = xreal.size();
	double err = 0;
	for (int i = 0; i < n; i++)
		err += (xreal[i] - yreal[i]) * (xreal[i] - yreal[i]) + (ximag[i] - yimag[i]) * (ximag[i] - yimag[i]);
	
	err /= n > 0 ? n : 1;
	err = sqrt(err);  // Now this is a root mean square (RMS) error
	err = err > 0 ? log10(err) : -99.0;
	maxLogError = std::max(err, maxLogError);
	return err;
}


static void randomReals(vector<double> &vec) {
	for (vector<double>::iterator it = vec.begin(); it != vec.end(); ++it)
		*it = (rand() / (RAND_MAX + 1.0)) * 2 - 1;
}