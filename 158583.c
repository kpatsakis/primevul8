get_time_in_ms()
{
struct timeval tmp_time;
unsigned long seconds, microseconds;

gettimeofday(&tmp_time, NULL);
seconds = (unsigned long) tmp_time.tv_sec;
microseconds = (unsigned long) tmp_time.tv_usec;
return seconds*1000 + microseconds/1000;
}