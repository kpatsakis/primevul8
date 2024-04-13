weight3(int x)
{
    if (x < 0.1)
	return 0.1;
    if (x < LOG_MIN)
	return (double)x;
    else
	return LOG_MIN * (log((double)x / LOG_MIN) + 1.);
}