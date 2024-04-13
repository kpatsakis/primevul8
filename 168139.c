networkstatus_compute_bw_weights_v10(smartlist_t *chunks, int64_t G,
                                     int64_t M, int64_t E, int64_t D,
                                     int64_t T, int64_t weight_scale)
{
  bw_weights_error_t berr = 0;
  int64_t Wgg = -1, Wgd = -1;
  int64_t Wmg = -1, Wme = -1, Wmd = -1;
  int64_t Wed = -1, Wee = -1;
  const char *casename;

  if (G <= 0 || M <= 0 || E <= 0 || D <= 0) {
    log_warn(LD_DIR, "Consensus with empty bandwidth: "
                     "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT
                     " D="I64_FORMAT" T="I64_FORMAT,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T));
    return 0;
  }

  /*
   * Computed from cases in 3.8.3 of dir-spec.txt
   *
   * 1. Neither are scarce
   * 2. Both Guard and Exit are scarce
   *    a. R+D <= S
   *    b. R+D > S
   * 3. One of Guard or Exit is scarce
   *    a. S+D < T/3
   *    b. S+D >= T/3
   */
  if (3*E >= T && 3*G >= T) { // E >= T/3 && G >= T/3
    /* Case 1: Neither are scarce.  */
    casename = "Case 1 (Wgd=Wmd=Wed)";
    Wgd = weight_scale/3;
    Wed = weight_scale/3;
    Wmd = weight_scale/3;
    Wee = (weight_scale*(E+G+M))/(3*E);
    Wme = weight_scale - Wee;
    Wmg = (weight_scale*(2*G-E-M))/(3*G);
    Wgg = weight_scale - Wmg;

    berr = networkstatus_check_weights(Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed,
                                       weight_scale, G, M, E, D, T, 10, 1);

    if (berr) {
      log_warn(LD_DIR,
             "Bw Weights error %d for %s v10. G="I64_FORMAT" M="I64_FORMAT
             " E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT
             " Wmd=%d Wme=%d Wmg=%d Wed=%d Wee=%d"
             " Wgd=%d Wgg=%d Wme=%d Wmg=%d weight_scale=%d",
             berr, casename,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
             (int)Wmd, (int)Wme, (int)Wmg, (int)Wed, (int)Wee,
             (int)Wgd, (int)Wgg, (int)Wme, (int)Wmg, (int)weight_scale);
      return 0;
    }
  } else if (3*E < T && 3*G < T) { // E < T/3 && G < T/3
    int64_t R = MIN(E, G);
    int64_t S = MAX(E, G);
    /*
     * Case 2: Both Guards and Exits are scarce
     * Balance D between E and G, depending upon
     * D capacity and scarcity.
     */
    if (R+D < S) { // Subcase a
      Wgg = weight_scale;
      Wee = weight_scale;
      Wmg = 0;
      Wme = 0;
      Wmd = 0;
      if (E < G) {
        casename = "Case 2a (E scarce)";
        Wed = weight_scale;
        Wgd = 0;
      } else { /* E >= G */
        casename = "Case 2a (G scarce)";
        Wed = 0;
        Wgd = weight_scale;
      }
    } else { // Subcase b: R+D >= S
      casename = "Case 2b1 (Wgg=1, Wmd=Wgd)";
      Wee = (weight_scale*(E - G + M))/E;
      Wed = (weight_scale*(D - 2*E + 4*G - 2*M))/(3*D);
      Wme = (weight_scale*(G-M))/E;
      Wmg = 0;
      Wgg = weight_scale;
      Wmd = (weight_scale - Wed)/2;
      Wgd = (weight_scale - Wed)/2;

      berr = networkstatus_check_weights(Wgg, Wgd, Wmg, Wme, Wmd, Wee, Wed,
                                       weight_scale, G, M, E, D, T, 10, 1);

      if (berr) {
        casename = "Case 2b2 (Wgg=1, Wee=1)";
        Wgg = weight_scale;
        Wee = weight_scale;
        Wed = (weight_scale*(D - 2*E + G + M))/(3*D);
        Wmd = (weight_scale*(D - 2*M + G + E))/(3*D);
        Wme = 0;
        Wmg = 0;

        if (Wmd < 0) { // Can happen if M > T/3
          casename = "Case 2b3 (Wmd=0)";
          Wmd = 0;
          log_warn(LD_DIR,
                   "Too much Middle bandwidth on the network to calculate "
                   "balanced bandwidth-weights. Consider increasing the "
                   "number of Guard nodes by lowering the requirements.");
        }
        Wgd = weight_scale - Wed - Wmd;
        berr = networkstatus_check_weights(Wgg, Wgd, Wmg, Wme, Wmd, Wee,
                  Wed, weight_scale, G, M, E, D, T, 10, 1);
      }
      if (berr != BW_WEIGHTS_NO_ERROR &&
              berr != BW_WEIGHTS_BALANCE_MID_ERROR) {
        log_warn(LD_DIR,
             "Bw Weights error %d for %s v10. G="I64_FORMAT" M="I64_FORMAT
             " E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT
             " Wmd=%d Wme=%d Wmg=%d Wed=%d Wee=%d"
             " Wgd=%d Wgg=%d Wme=%d Wmg=%d weight_scale=%d",
             berr, casename,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
             (int)Wmd, (int)Wme, (int)Wmg, (int)Wed, (int)Wee,
             (int)Wgd, (int)Wgg, (int)Wme, (int)Wmg, (int)weight_scale);
        return 0;
      }
    }
  } else { // if (E < T/3 || G < T/3) {
    int64_t S = MIN(E, G);
    // Case 3: Exactly one of Guard or Exit is scarce
    if (!(3*E < T || 3*G < T) || !(3*G >= T || 3*E >= T)) {
      log_warn(LD_BUG,
           "Bw-Weights Case 3 v10 but with G="I64_FORMAT" M="
           I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT,
               I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
               I64_PRINTF_ARG(D), I64_PRINTF_ARG(T));
    }

    if (3*(S+D) < T) { // Subcase a: S+D < T/3
      if (G < E) {
        casename = "Case 3a (G scarce)";
        Wgg = Wgd = weight_scale;
        Wmd = Wed = Wmg = 0;
        // Minor subcase, if E is more scarce than M,
        // keep its bandwidth in place.
        if (E < M) Wme = 0;
        else Wme = (weight_scale*(E-M))/(2*E);
        Wee = weight_scale-Wme;
      } else { // G >= E
        casename = "Case 3a (E scarce)";
        Wee = Wed = weight_scale;
        Wmd = Wgd = Wme = 0;
        // Minor subcase, if G is more scarce than M,
        // keep its bandwidth in place.
        if (G < M) Wmg = 0;
        else Wmg = (weight_scale*(G-M))/(2*G);
        Wgg = weight_scale-Wmg;
      }
    } else { // Subcase b: S+D >= T/3
      // D != 0 because S+D >= T/3
      if (G < E) {
        casename = "Case 3bg (G scarce, Wgg=1, Wmd == Wed)";
        Wgg = weight_scale;
        Wgd = (weight_scale*(D - 2*G + E + M))/(3*D);
        Wmg = 0;
        Wee = (weight_scale*(E+M))/(2*E);
        Wme = weight_scale - Wee;
        Wmd = (weight_scale - Wgd)/2;
        Wed = (weight_scale - Wgd)/2;

        berr = networkstatus_check_weights(Wgg, Wgd, Wmg, Wme, Wmd, Wee,
                    Wed, weight_scale, G, M, E, D, T, 10, 1);
      } else { // G >= E
        casename = "Case 3be (E scarce, Wee=1, Wmd == Wgd)";
        Wee = weight_scale;
        Wed = (weight_scale*(D - 2*E + G + M))/(3*D);
        Wme = 0;
        Wgg = (weight_scale*(G+M))/(2*G);
        Wmg = weight_scale - Wgg;
        Wmd = (weight_scale - Wed)/2;
        Wgd = (weight_scale - Wed)/2;

        berr = networkstatus_check_weights(Wgg, Wgd, Wmg, Wme, Wmd, Wee,
                      Wed, weight_scale, G, M, E, D, T, 10, 1);
      }
      if (berr) {
        log_warn(LD_DIR,
             "Bw Weights error %d for %s v10. G="I64_FORMAT" M="I64_FORMAT
             " E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT
             " Wmd=%d Wme=%d Wmg=%d Wed=%d Wee=%d"
             " Wgd=%d Wgg=%d Wme=%d Wmg=%d weight_scale=%d",
             berr, casename,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
             (int)Wmd, (int)Wme, (int)Wmg, (int)Wed, (int)Wee,
             (int)Wgd, (int)Wgg, (int)Wme, (int)Wmg, (int)weight_scale);
        return 0;
      }
    }
  }

  /* We cast down the weights to 32 bit ints on the assumption that
   * weight_scale is ~= 10000. We need to ensure a rogue authority
   * doesn't break this assumption to rig our weights */
  tor_assert(0 < weight_scale && weight_scale <= INT32_MAX);

  /*
   * Provide Wgm=Wgg, Wmm=1, Wem=Wee, Weg=Wed. May later determine
   * that middle nodes need different bandwidth weights for dirport traffic,
   * or that weird exit policies need special weight, or that bridges
   * need special weight.
   *
   * NOTE: This list is sorted.
   */
  smartlist_add_asprintf(chunks,
     "bandwidth-weights Wbd=%d Wbe=%d Wbg=%d Wbm=%d "
     "Wdb=%d "
     "Web=%d Wed=%d Wee=%d Weg=%d Wem=%d "
     "Wgb=%d Wgd=%d Wgg=%d Wgm=%d "
     "Wmb=%d Wmd=%d Wme=%d Wmg=%d Wmm=%d\n",
     (int)Wmd, (int)Wme, (int)Wmg, (int)weight_scale,
     (int)weight_scale,
     (int)weight_scale, (int)Wed, (int)Wee, (int)Wed, (int)Wee,
     (int)weight_scale, (int)Wgd, (int)Wgg, (int)Wgg,
     (int)weight_scale, (int)Wmd, (int)Wme, (int)Wmg, (int)weight_scale);

  log_notice(LD_CIRC, "Computed bandwidth weights for %s with v10: "
             "G="I64_FORMAT" M="I64_FORMAT" E="I64_FORMAT" D="I64_FORMAT
             " T="I64_FORMAT,
             casename,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T));
  return 1;
}