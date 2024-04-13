same_hosts(host_item *one, host_item *two)
{
while (one && two)
  {
  if (Ustrcmp(one->name, two->name) != 0)
    {
    int mx = one->mx;
    host_item *end_one = one;
    host_item *end_two = two;

    /* Batch up only if there was no MX and the list was not randomized */

    if (mx == MX_NONE) return FALSE;

    /* Find the ends of the shortest sequence of identical MX values */

    while (  end_one->next && end_one->next->mx == mx
          && end_two->next && end_two->next->mx == mx)
      {
      end_one = end_one->next;
      end_two = end_two->next;
      }

    /* If there aren't any duplicates, there's no match. */

    if (end_one == one) return FALSE;

    /* For each host in the 'one' sequence, check that it appears in the 'two'
    sequence, returning FALSE if not. */

    for (;;)
      {
      host_item *hi;
      for (hi = two; hi != end_two->next; hi = hi->next)
        if (Ustrcmp(one->name, hi->name) == 0) break;
      if (hi == end_two->next) return FALSE;
      if (one == end_one) break;
      one = one->next;
      }

    /* All the hosts in the 'one' sequence were found in the 'two' sequence.
    Ensure both are pointing at the last host, and carry on as for equality. */

    two = end_two;
    }

  /* if the names matched but ports do not, mismatch */
  else if (one->port != two->port)
    return FALSE;

  /* Hosts matched */

  one = one->next;
  two = two->next;
  }

/* True if both are NULL */

return (one == two);
}