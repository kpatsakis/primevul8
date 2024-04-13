exim_wait_tick(struct timeval *then_tv, int resolution)
{
struct timeval now_tv;
long int now_true_usec;

(void)gettimeofday(&now_tv, NULL);
now_true_usec = now_tv.tv_usec;
now_tv.tv_usec = (now_true_usec/resolution) * resolution;

if (exim_tvcmp(&now_tv, then_tv) <= 0)
  {
  struct itimerval itval;
  itval.it_interval.tv_sec = 0;
  itval.it_interval.tv_usec = 0;
  itval.it_value.tv_sec = then_tv->tv_sec - now_tv.tv_sec;
  itval.it_value.tv_usec = then_tv->tv_usec + resolution - now_true_usec;

  /* We know that, overall, "now" is less than or equal to "then". Therefore, a
  negative value for the microseconds is possible only in the case when "now"
  is more than a second less than "then". That means that itval.it_value.tv_sec
  is greater than zero. The following correction is therefore safe. */

  if (itval.it_value.tv_usec < 0)
    {
    itval.it_value.tv_usec += 1000000;
    itval.it_value.tv_sec -= 1;
    }

  DEBUG(D_transport|D_receive)
    {
    if (!f.running_in_test_harness)
      {
      debug_printf("tick check: " TIME_T_FMT ".%06lu " TIME_T_FMT ".%06lu\n",
        then_tv->tv_sec, (long) then_tv->tv_usec,
       	now_tv.tv_sec, (long) now_tv.tv_usec);
      debug_printf("waiting " TIME_T_FMT ".%06lu\n",
        itval.it_value.tv_sec, (long) itval.it_value.tv_usec);
      }
    }

  milliwait(&itval);
  }
}