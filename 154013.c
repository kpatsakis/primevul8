ceil_at_intervals(int x, int step)
{
    int mo = x % step;
    if (mo > 0)
	x += step - mo;
    else if (mo < 0)
	x -= mo;
    return x;
}