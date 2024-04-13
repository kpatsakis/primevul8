exim_tvcmp(struct timeval *t1, struct timeval *t2)
{
if (t1->tv_sec > t2->tv_sec) return +1;
if (t1->tv_sec < t2->tv_sec) return -1;
if (t1->tv_usec > t2->tv_usec) return +1;
if (t1->tv_usec < t2->tv_usec) return -1;
return 0;
}