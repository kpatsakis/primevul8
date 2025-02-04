level_cost(unsigned int n)
{
    static const double log_1_1 = 0.09531017980432493;

	/* Adding 0.1 protects against the case where n==1 */
	return ceil(log(n + 0.1)/log_1_1);
}