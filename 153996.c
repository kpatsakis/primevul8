weight(int x)
{

    if (x < COLS)
	return (double)x;
    else
	return COLS * (log((double)x / COLS) + 1.);
}