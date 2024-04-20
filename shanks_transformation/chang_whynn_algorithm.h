/**
 * @file epsilon_algorithm_two.h
 * @brief This file contains the declaration of the second implementation of Epsilon Algorithm class.
 */

#pragma once
#define DEF_UNDEFINED_SUM 0

#include "series_acceleration.h" // Include the series header
#include <vector> // Include the vector library

 /**
 * @brief Epsilon Algorithm MK-2 class template. "Scalar Epsilon Algorithm" 
 //SOME RESULTS CONCERNING THE FUNDAMENTAL NATURE OF WYNN'S VECTOR EPSILON ALGORITHM - same algo + vector form
 //On a Device for Computing the e (S ) Transformation - nothing new, just matrix

 //Construction of new generalizations of Wynn’s
epsilon and rho algorithm by solving finite difference
equations in the transformation order
chang algo!
 * @authors  Kreinin R.G.
 * @tparam T The type of the elements in the series, K The type of enumerating integer, series_templ is the type of series whose convergence we accelerate
 */
template <typename T, typename K, typename series_templ>
class chang_whynn_algorithm : public series_acceleration<T, K, series_templ>
{
public:
	/**
	* @brief Parameterized constructor to initialize the Epsilon Algorithm MK-2.
	* @param series The series class object to be accelerated
	*/
    chang_whynn_algorithm(const series_templ& series);

	/**
	* @brief Fast impimentation of Levin algorithm.
	* Computes the partial sum after the transformation using the Epsilon Algorithm.
	* @param n The number of terms in the partial sum.
	* @param order The order of transformation.
	* @return The partial sum after the transformation.
	*/
	T operator()(const K n, const int order) const;
};

template <typename T, typename K, typename series_templ>
chang_whynn_algorithm<T, K, series_templ>::chang_whynn_algorithm(const series_templ& series) : series_acceleration<T, K, series_templ>(series) {}

template <typename T, typename K, typename series_templ>
T chang_whynn_algorithm<T, K, series_templ>::operator()(const K n, const int order) const
{
    if (n < 0)
        throw std::domain_error("negative integer in the input");
    else if (n == 0)
        return DEF_UNDEFINED_SUM;

    T up, down, coef, coef2;

    int max = 0;

    (n % 2 == 0) ? max += n : max += n - 1;

    std::vector<std::vector<T>> t(2, std::vector<T>(n, 0));
    std::vector<T> f(n, 0);

    for (int i = 0; i < max; ++i)
    {
        t[0][i] = 1.0 / (this->series->operator()(i + 1));
    }

    for (int i = 0; i < max; ++i)
    {
        coef = (this->series->S_n(i + 3) + this->series->S_n(i + 1) - 2 * this->series->S_n(i + 2));
        coef2 = (this->series->S_n(i + 2) + this->series->S_n(i) - 2 * this->series->S_n(i + 1));

        up = this->series->operator()(i + 1) * this->series->operator()(i + 2) * coef;
        down = this->series->operator()(i + 3) * coef2;
        down -= (this->series->operator()(i + 1)) * coef;
        down = 1.0 / down;
        t[1][i] = this->series->S_n(i + 1) - up * down;

        f[i] = coef * coef2 * down; //Можно взять coef^2 для более сочного эффекта
    }

    for (int k = 2; k <= max; ++k) {
        for (int i = 0; i < max - k; ++i) {

            up = 1 - k + k * f[i];
            down = 1.0 / (t[1][i + 1] - t[1][i]);
            t[0][i] = t[0][i + 1] + up * down;
        }
        std::swap(t[0], t[1]);
    }

    const T result = t[0][0];
    if (!std::isfinite(result))
        throw std::overflow_error("division by zero");

    return result;
}


/*
std::vector<std::vector<T>> t(n + 5, std::vector<T>(n + 5, 0));
    std::vector<T> f(n, 0);

    for (int i = 0; i <= n + 4; ++i)
    {
        t[0][i] = this->series->S_n(i);
    }

    for (int i = 0; i < n + 3; ++i)
    {
        t[1][i] = 1.0 / (t[0][i + 1] - t[0][i]);
    }

    T up, down;

    for (int i = 0; i < n; ++i)
    {
        up = (t[0][i + 1] - t[0][i]) * (t[0][i + 2] - t[0][i + 1]) * (t[0][i + 1 + 2] + t[0][i + 1] - 2 * t[0][i + 1 + 1]);
        down = (t[0][i + 3] - t[0][i + 2]) * (t[0][i + 2] + t[0][i] - 2 * t[0][i + 1]);
        down -= (t[0][i + 1] - t[0][i]) * (t[0][i + 2 + 1] + t[0][i + 1] - 2 * t[0][i + 1 + 1]);
        down = 1.0 / down;

        t[2][i] = t[0][i + 1] - up * down;

        up = (t[0][i + 2 + 1] + t[0][i + 1] - 2 * t[0][i + 1 + 1]) * (t[0][i + 1 + 2] + t[0][i + 1] - 2 * t[0][i + 1 + 1]);
        f[i] = up * down;
    }

    for (int k = 2; k < n; ++k) {
        for (int i = 0; i < n - k; ++i) {

            up = 1 - k + k * f[i];
            down = 1.0 / (t[k][i + 1] - t[k][i]);
            t[k + 1][i] = t[k - 1][i + 1] + up * down;
        }
    }

    if (n % 2 == 0)
    {
        return t[n][0];
    }
    return t[n - 1][0];


*/






/*
#include <cmath>
#include <vector>

double ABS(double x) {
    return std::abs(x);
}

double AMAX1(double x, double y) {
    return std::max(x, y);
}

int main() {
    double ABSERR, DELTA1, DELTA2, DELTA3, EMACH, EPRN, EPSINF, EPSTAB,
           ERROR, ERR1, ERR2, ERR3, E0, E1, E2, E3, E1ABS, OFRN, RES,
           RESULT, RESLA, SS, TOL1, TOL2, TOL3, UFRN;
    int I, IB, IB2, IE, IN, K1, K2, K3, LIMEXP, N, NEWELM, NUM;
    std::vector<double> EPSTAB(52);

    // LIMEXP IS THE MAXIMUM NUMBER OF ELEMENTS THE EPSILON
    // TABLE CAN CONTAIN. IF THIS NUMBER IS REACHED, THE UPPER
    // DIAGONAL OF THE EPSILON TABLE IS DELETED.
    // ( EPSTAB IS OF DIMENSION (LIMEXP+2) AT LEAST.)
    LIMEXP = 50;

    EPSTAB[N+2] = EPSTAB[N];
    NEWELM = (N-1)/2;
    EPSTAB[N] = OFRN;
    ABSERR = OFRN;
    NUM = N;
    K1 = N;
    for (I=0; I<NEWELM; I++) {
        K2 = K1 - 1;
        K3 = K1 - 2;
        RES = EPSTAB[K1+2];
        E0 = EPSTAB[K3];
        E1 = EPSTAB[K2];
        E2 = RES;
        E1ABS = ABS(E1);
        DELTA2 = E2 - E1;
        ERR2 = ABS(DELTA2);
        TOL2 = AMAX1(ABS(E2), E1ABS) * EMACH;
        DELTA3 = E1 - E0;
        ERR3 = ABS(DELTA3);
        TOL3 = AMAX1(E1ABS, ABS(E0)) * EMACH;
        if (ERR2 > TOL2 || ERR3 > TOL3) {
            E3 = EPSTAB[K1];
            EPSTAB[K1] = E1;
            DELTA1 = E1 - E3;
            ERR1 = ABS(DELTA1);
            TOL1 = AMAX1(E1ABS, ABS(E3)) * EMACH;
        } else {
            // IF E0, E1 AND E2 ARE EQUAL TO WITHIN MACHINE
            // ACCURACY, CONVERGENCE IS ASSUMED.
            // RESULT = E2
            // ABSERR = ABS(E1-E0)+ABS(E2-E1)
            RESULT = RES;
            ABSERR = ERR2 + ERR3;
            break;
        }
        K1 = K2;
    }
    return 0;
}


#include <cmath>
#include <algorithm>

if (err1 <= tol1 || err2 <= tol2 || err3 <= tol3) goto label20;

double ss = 0.1 / delta1 + 0.1 / delta2 - 0.1 / delta3;
double epsinf = std::abs(ss * e1);

if (epsinf > 0.0001) goto label30;

label20:
int n = i + i - 1;
goto label50;

label30:
double res = e1 + 0.1 / ss;
epstab[k1] = res;
k1 -= 2;
double error = err2 + std::abs(res - e2) + err3;
if (error > abserr) goto label40;
abserr = error;
result = res;

label40:
continue;

label50:
if (n == limexp) n = 2 * (limexp / 2) - 1;
int ib = 1;
if ((num / 2) * 2 == num) ib = 2;
int ie = newelm + 1;
for (int i = 1; i <= ie; i++) {
    int ib2 = ib + 2;
    epstab[ib] = epstab[ib2];
    ib = ib2;
}
if (num == n) goto label80;
int in = num - n + 1;
for (int i = 1; i <= n; i++) {
    epstab[i] = epstab[in];
    in += 1;
}

label80:
abserr = std::abs(result - resla);
resla = result;

label90:
abserr = std::max(abserr, eprn * std::abs(result));
return;
}

void order(int limit, int& last, double& maxerr, double ermax, std::vector<double>& elist, std::vector<int>& iord, int& nrmax, std::vector<int>& ord, int& navail) {
    // Implement the order subroutine in C++
}




#include <cmath>
#include <algorithm>
#include <vector>

double ABSERR, DELTA1, DELTA2, DELTA3, EMACH, EPRN, EPSINF, EPSTAB, ERROR, ERR1, ERR2, ERR3, E0, E1, E2, E3, E1ABS, OFRN, RES, RESULT, RESLA, SS, TOL1, TOL2, TOL3, UFRN;
int I, IB, IB2, IE, IN, K1, K2, K3, LIMEXP, N, NEWELM, NUM;
std::vector<double> EPSTAB(52);

#define DABS(x) std::abs(x)
#define DMAX1(x, y) std::max(x, y)

void main() {
    EPSTAB[N+2] = EPSTAB[N];
    NEWELM = (N-1)/2;
    EPSTAB[N] = OFRN;
    ABSERR = OFRN;
    NUM = N;
    K1 = N;
    for (I=0; I<NEWELM; I++) {
        K2 = K1 - 1;
        K3 = K1 - 2;
        RES = EPSTAB[K1+2];
        E0 = EPSTAB[K3];
        E1 = EPSTAB[K2];
        E2 = RES;
        E1ABS = DABS(E1);
        DELTA2 = E2 - E1;
        ERR2 = DABS(DELTA2);
        TOL2 = DMAX1(DABS(E2),E1ABS)*EMACH;
        DELTA3 = E1 - E0;
        ERR3 = DABS(DELTA3);
        TOL3 = DMAX1(E1ABS,DABS(E0))*EMACH;
        if (ERR2>TOL2 || ERR3>TOL3) {
            E3 = EPSTAB[K1];
            EPSTAB[K1] = E1;
            DELTA1 = E1 - E3;
            ERR1 = DABS(DELTA1);
            TOL1 = DMAX1(E1ABS,DABS(E3))*EMACH;
            if (ERR1>TOL1 && ERR2>TOL2 && ERR3>TOL3) {
                SS = 0.1D+01/DELTA1 + 0.1D+01/DELTA2 - 0.1D+01/DELTA3;
                EPSINF = DABS(SS*E1);
                if (EPSINF>0.1D-03) {
                    RES = E1 + 0.1D+01/SS;
                    EPSTAB[K1] = RES;
                    K1 = K1 - 2;
                    ERROR = ERR2 + DABS(RES-E2) + ERR3;
                    if (ERROR>ABSERR) {
                        ABSERR = ERROR;
                        RESULT = RES;
                    }
                }
            } else {
                N = I + I - 1;
                if (N==LIMEXP) N = 2*(LIMEXP/2) - 1;
                IB = 1;
                if ((NUM/2)*2==NUM) IB = 2;
                IE = NEWELM + 1;
                for (I=0; I<IE; I++) {
                    IB2 = IB + 2;
                    EPSTAB[IB] = EPSTAB[IB2];
                    IB = IB2;
                }
                if (NUM!=N) {
                    IN = NUM - N + 1;
                    for (I=0; I<N; I++) {
                        EPSTAB[I] = EPSTAB[IN];
                        IN = IN + 1;
                    }
                }
                ABSERR = DABS(RESULT-RESLA);
                RESLA = RESULT;
            }
        } else {
            RESULT = RES;
            ABSERR = ERR2 + ERR3;
        }
    }
    ABSERR = DMAX1(ABSERR,EPRN*DABS(RESULT));
}

void ORDER(int LIMIT, int& LAST, double& MAXERR, double& ERMAX, std::vector<double>& ELIST, std::vector<int>& IORD, int& NRMAX, std::vector<int>& ORD, int& NAVAIL) {
    // Implement the ORDER subroutine in C++
}






*/


/*
#include <cmath>
#include <vector>

double ABS(double x) {
    return std::abs(x);
}

double AMAX1(double x, double y) {
    return std::max(x, y);
}

int main() {
    double ABSERR, DELTA1, DELTA2, DELTA3, EMACH, EPRN, EPSINF, EPSTAB,
           ERROR, ERR1, ERR2, ERR3, E0, E1, E2, E3, E1ABS, OFRN, RES,
           RESULT, RESLA, SS, TOL1, TOL2, TOL3, UFRN;
    int I, IB, IB2, IE, IN, K1, K2, K3, LIMEXP, N, NEWELM, NUM;
    std::vector<double> EPSTAB(52);

    // LIMEXP IS THE MAXIMUM NUMBER OF ELEMENTS THE EPSILON
    // TABLE CAN CONTAIN. IF THIS NUMBER IS REACHED, THE UPPER
    // DIAGONAL OF THE EPSILON TABLE IS DELETED.
    // ( EPSTAB IS OF DIMENSION (LIMEXP+2) AT LEAST.)
    LIMEXP = 50;

    EPSTAB[N+2] = EPSTAB[N];
    NEWELM = (N-1)/2;
    EPSTAB[N] = OFRN;
    ABSERR = OFRN;
    NUM = N;
    K1 = N;
    for (I=0; I<NEWELM; I++) {
        K2 = K1 - 1;
        K3 = K1 - 2;
        RES = EPSTAB[K1+2];
        E0 = EPSTAB[K3];
        E1 = EPSTAB[K2];
        E2 = RES;
        E1ABS = ABS(E1);
        DELTA2 = E2 - E1;
        ERR2 = ABS(DELTA2);
        TOL2 = AMAX1(ABS(E2), E1ABS) * EMACH;
        DELTA3 = E1 - E0;
        ERR3 = ABS(DELTA3);
        TOL3 = AMAX1(E1ABS, ABS(E0)) * EMACH;
        if (ERR2 > TOL2 || ERR3 > TOL3) {
            E3 = EPSTAB[K1];
            EPSTAB[K1] = E1;
            DELTA1 = E1 - E3;
            ERR1 = ABS(DELTA1);
            TOL1 = AMAX1(E1ABS, ABS(E3)) * EMACH;
        } else {
            // IF E0, E1 AND E2 ARE EQUAL TO WITHIN MACHINE
            // ACCURACY, CONVERGENCE IS ASSUMED.
            // RESULT = E2
            // ABSERR = ABS(E1-E0)+ABS(E2-E1)
            RESULT = RES;
            ABSERR = ERR2 + ERR3;
            break;
        }
        K1 = K2;
    }
    return 0;
}


#include <cmath>
#include <algorithm>

if (err1 <= tol1 || err2 <= tol2 || err3 <= tol3) goto label20;

double ss = 0.1 / delta1 + 0.1 / delta2 - 0.1 / delta3;
double epsinf = std::abs(ss * e1);

if (epsinf > 0.0001) goto label30;

label20:
int n = i + i - 1;
goto label50;

label30:
double res = e1 + 0.1 / ss;
epstab[k1] = res;
k1 -= 2;
double error = err2 + std::abs(res - e2) + err3;
if (error > abserr) goto label40;
abserr = error;
result = res;

label40:
continue;

label50:
if (n == limexp) n = 2 * (limexp / 2) - 1;
int ib = 1;
if ((num / 2) * 2 == num) ib = 2;
int ie = newelm + 1;
for (int i = 1; i <= ie; i++) {
    int ib2 = ib + 2;
    epstab[ib] = epstab[ib2];
    ib = ib2;
}
if (num == n) goto label80;
int in = num - n + 1;
for (int i = 1; i <= n; i++) {
    epstab[i] = epstab[in];
    in += 1;
}

label80:
abserr = std::abs(result - resla);
resla = result;

label90:
abserr = std::max(abserr, eprn * std::abs(result));
return;
}

void order(int limit, int& last, double& maxerr, double ermax, std::vector<double>& elist, std::vector<int>& iord, int& nrmax, std::vector<int>& ord, int& navail) {
    // Implement the order subroutine in C++
}




#include <cmath>
#include <algorithm>
#include <vector>

double ABSERR, DELTA1, DELTA2, DELTA3, EMACH, EPRN, EPSINF, EPSTAB, ERROR, ERR1, ERR2, ERR3, E0, E1, E2, E3, E1ABS, OFRN, RES, RESULT, RESLA, SS, TOL1, TOL2, TOL3, UFRN;
int I, IB, IB2, IE, IN, K1, K2, K3, LIMEXP, N, NEWELM, NUM;
std::vector<double> EPSTAB(52);

#define DABS(x) std::abs(x)
#define DMAX1(x, y) std::max(x, y)

void main() {
    EPSTAB[N+2] = EPSTAB[N];
    NEWELM = (N-1)/2;
    EPSTAB[N] = OFRN;
    ABSERR = OFRN;
    NUM = N;
    K1 = N;
    for (I=0; I<NEWELM; I++) {
        K2 = K1 - 1;
        K3 = K1 - 2;
        RES = EPSTAB[K1+2];
        E0 = EPSTAB[K3];
        E1 = EPSTAB[K2];
        E2 = RES;
        E1ABS = DABS(E1);
        DELTA2 = E2 - E1;
        ERR2 = DABS(DELTA2);
        TOL2 = DMAX1(DABS(E2),E1ABS)*EMACH;
        DELTA3 = E1 - E0;
        ERR3 = DABS(DELTA3);
        TOL3 = DMAX1(E1ABS,DABS(E0))*EMACH;
        if (ERR2>TOL2 || ERR3>TOL3) {
            E3 = EPSTAB[K1];
            EPSTAB[K1] = E1;
            DELTA1 = E1 - E3;
            ERR1 = DABS(DELTA1);
            TOL1 = DMAX1(E1ABS,DABS(E3))*EMACH;
            if (ERR1>TOL1 && ERR2>TOL2 && ERR3>TOL3) {
                SS = 0.1D+01/DELTA1 + 0.1D+01/DELTA2 - 0.1D+01/DELTA3;
                EPSINF = DABS(SS*E1);
                if (EPSINF>0.1D-03) {
                    RES = E1 + 0.1D+01/SS;
                    EPSTAB[K1] = RES;
                    K1 = K1 - 2;
                    ERROR = ERR2 + DABS(RES-E2) + ERR3;
                    if (ERROR>ABSERR) {
                        ABSERR = ERROR;
                        RESULT = RES;
                    }
                }
            } else {
                N = I + I - 1;
                if (N==LIMEXP) N = 2*(LIMEXP/2) - 1;
                IB = 1;
                if ((NUM/2)*2==NUM) IB = 2;
                IE = NEWELM + 1;
                for (I=0; I<IE; I++) {
                    IB2 = IB + 2;
                    EPSTAB[IB] = EPSTAB[IB2];
                    IB = IB2;
                }
                if (NUM!=N) {
                    IN = NUM - N + 1;
                    for (I=0; I<N; I++) {
                        EPSTAB[I] = EPSTAB[IN];
                        IN = IN + 1;
                    }
                }
                ABSERR = DABS(RESULT-RESLA);
                RESLA = RESULT;
            }
        } else {
            RESULT = RES;
            ABSERR = ERR2 + ERR3;
        }
    }
    ABSERR = DMAX1(ABSERR,EPRN*DABS(RESULT));
}

void ORDER(int LIMIT, int& LAST, double& MAXERR, double& ERMAX, std::vector<double>& ELIST, std::vector<int>& IORD, int& NRMAX, std::vector<int>& ORD, int& NAVAIL) {
    // Implement the ORDER subroutine in C++
}






*/