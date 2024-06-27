#include <opencv2/opencv.hpp>
#include <vector>
#include <complex>

using namespace cv;
using namespace std;

// Función para calcular el factorial de un número
int factorial(int n) {
	if (n == 0) return 1;
	int result = 1;
	for (int i = 1; i <= n; ++i) {
		result *= i;
	}
	return result;
}

// Función para calcular el coeficiente de binomio
int binomialCoefficient(int n, int k) {
	return factorial(n) / (factorial(k) * factorial(n - k));
}

// Función para calcular el polinomio radial de Zernike
double radialPolynomial(int n, int m, double rho) {
	double result = 0.0;
	for (int s = 0; s <= (n - abs(m)) / 2; ++s) {
		double sign = (s % 2 == 0) ? 1.0 : -1.0;
		result += sign * binomialCoefficient(n - s, s) * binomialCoefficient(n - 2 * s, (n - abs(m)) / 2 - s) * pow(rho, n - 2 * s);
	}
	return result;
}

// Función para calcular los momentos de Zernike
void calcularMomentosZernike(const Mat& imagen, vector<double>& momentosZernike, int maxOrder) {
	// Suponemos que la imagen es binaria y de tipo CV_8UC1
	int rows = imagen.rows;
	int cols = imagen.cols;
	int centerX = rows / 2;
	int centerY = cols / 2;
	double radius = min(rows, cols) / 2.0;

	// Iterar sobre cada píxel de la imagen
	for (int n = 0; n <= maxOrder; ++n) {
		for (int m = -n; m <= n; m += 2) {
			std::complex<double> Znm(0.0, 0.0);

			for (int y = 0; y < rows; ++y) {
				for (int x = 0; x < cols; ++x) {
					if (imagen.at<uchar>(y, x) == 0) continue;

					double rho = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2)) / radius;
					if (rho > 1.0) continue;

					double theta = atan2(y - centerY, x - centerX);
					double Rnm = radialPolynomial(n, m, rho);
					std::complex<double> Vnm = Rnm * exp(-std::complex<double>(0.0, m * theta));

					Znm += Vnm * static_cast<double>(imagen.at<uchar>(y, x)) / 255.0;
				}
			}

			double Anm = abs(Znm) * (n + 1) / CV_PI;
			momentosZernike.push_back(Anm);
		}
	}
}
