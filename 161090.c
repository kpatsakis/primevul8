set_digit(unsigned char *p, size_t s, int value)
{

	while (s--) {
		p[s] = '0' + (value % 10);
		value /= 10;
	}
}