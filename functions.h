#ifndef FUNCTIONS_H
#define FUNCTIONS_H

double f_0 (double x);
double f_1 (double x);
double f_2 (double x);
double f_3 (double x);
double f_4 (double x);
double f_5 (double x);
double f_6 (double x);

const char *get_func_name (int k);
double (*get_func (int k)) (double);

#endif // FUNCTIONS_H