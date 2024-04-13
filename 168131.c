update_total_bandwidth_weights(const routerstatus_t *rs,
                               int is_exit, int is_guard,
                               int64_t *G, int64_t *M, int64_t *E, int64_t *D,
                               int64_t *T)
{
  int default_bandwidth = rs->bandwidth_kb;
  int guardfraction_bandwidth = 0;

  if (!rs->has_bandwidth) {
    log_info(LD_BUG, "Missing consensus bandwidth for router %s",
             rs->nickname);
    return;
  }

  /* If this routerstatus represents a guard that we have
   * guardfraction information on, use it to calculate its actual
   * bandwidth. From proposal236:
   *
   *    Similarly, when calculating the bandwidth-weights line as in
   *    section 3.8.3 of dir-spec.txt, directory authorities should treat N
   *    as if fraction F of its bandwidth has the guard flag and (1-F) does
   *    not.  So when computing the totals G,M,E,D, each relay N with guard
   *    visibility fraction F and bandwidth B should be added as follows:
   *
   *    G' = G + F*B, if N does not have the exit flag
   *    M' = M + (1-F)*B, if N does not have the exit flag
   *
   *    or
   *
   *    D' = D + F*B, if N has the exit flag
   *    E' = E + (1-F)*B, if N has the exit flag
   *
   * In this block of code, we prepare the bandwidth values by setting
   * the default_bandwidth to F*B and guardfraction_bandwidth to (1-F)*B.
   */
  if (rs->has_guardfraction) {
    guardfraction_bandwidth_t guardfraction_bw;

    tor_assert(is_guard);

    guard_get_guardfraction_bandwidth(&guardfraction_bw,
                                      rs->bandwidth_kb,
                                      rs->guardfraction_percentage);

    default_bandwidth = guardfraction_bw.guard_bw;
    guardfraction_bandwidth = guardfraction_bw.non_guard_bw;
  }

  /* Now calculate the total bandwidth weights with or without
   * guardfraction. Depending on the flags of the relay, add its
   * bandwidth to the appropriate weight pool. If it's a guard and
   * guardfraction is enabled, add its bandwidth to both pools as
   * indicated by the previous comment.
   */
  *T += default_bandwidth;
  if (is_exit && is_guard) {

    *D += default_bandwidth;
    if (rs->has_guardfraction) {
      *E += guardfraction_bandwidth;
    }

  } else if (is_exit) {

    *E += default_bandwidth;

  } else if (is_guard) {

    *G += default_bandwidth;
    if (rs->has_guardfraction) {
      *M += guardfraction_bandwidth;
    }

  } else {

    *M += default_bandwidth;
  }
}