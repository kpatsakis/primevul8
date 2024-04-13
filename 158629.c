millisleep(int msec)
{
struct itimerval itval;
itval.it_interval.tv_sec = 0;
itval.it_interval.tv_usec = 0;
itval.it_value.tv_sec = msec/1000;
itval.it_value.tv_usec = (msec % 1000) * 1000;
milliwait(&itval);
}