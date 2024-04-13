unsigned subtract_bl(unsigned int val, int bl)
{
	return val > bl ? val - bl : 0;
}