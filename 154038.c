floor_at_intervals(int x, int step)
{
    int mo = x % step;
    if (mo > 0)
	x -= mo;
    else if (mo < 0)
	x += step - mo;
    return x;
}