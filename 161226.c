set_date_time(unsigned char *p, time_t t)
{
	struct tm tm;

	get_tmfromtime(&tm, &t);
	set_digit(p, 4, tm.tm_year + 1900);
	set_digit(p+4, 2, tm.tm_mon + 1);
	set_digit(p+6, 2, tm.tm_mday);
	set_digit(p+8, 2, tm.tm_hour);
	set_digit(p+10, 2, tm.tm_min);
	set_digit(p+12, 2, tm.tm_sec);
	set_digit(p+14, 2, 0);
	set_num_712(p+16, (char)(get_gmoffset(&tm)/(60*15)));
}