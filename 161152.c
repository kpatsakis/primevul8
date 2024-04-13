set_time_915(unsigned char *p, time_t t)
{
	struct tm tm;

	get_tmfromtime(&tm, &t);
	set_num_711(p+0, tm.tm_year);
	set_num_711(p+1, tm.tm_mon+1);
	set_num_711(p+2, tm.tm_mday);
	set_num_711(p+3, tm.tm_hour);
	set_num_711(p+4, tm.tm_min);
	set_num_711(p+5, tm.tm_sec);
	set_num_712(p+6, (char)(get_gmoffset(&tm)/(60*15)));
}