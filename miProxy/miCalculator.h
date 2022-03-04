// CALCULATE TP

void calculate(double *T_cur, double T_new, double alpha)
{
		// Calculate the current throughput
		(*T_cur) = alpha * T_new + (1 - alpha) * (*T_cur);
}