par_reduce(int max, BOOL fallback)
{
while (parcount > max)
  {
  address_item *doneaddr = par_wait();
  if (!doneaddr)
    {
    log_write(0, LOG_MAIN|LOG_PANIC,
      "remote delivery process count got out of step");
    parcount = 0;
    }
  else
    {
    transport_instance * tp = doneaddr->transport;
    if (tp->max_parallel)
      enq_end(string_sprintf("tpt-serialize-%s", tp->name));

    remote_post_process(doneaddr, LOG_MAIN, NULL, fallback);
    }
  }
}