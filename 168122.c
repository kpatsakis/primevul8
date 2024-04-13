networkstatus_check_weights(int64_t Wgg, int64_t Wgd, int64_t Wmg,
                            int64_t Wme, int64_t Wmd, int64_t Wee,
                            int64_t Wed, int64_t scale, int64_t G,
                            int64_t M, int64_t E, int64_t D, int64_t T,
                            int64_t margin, int do_balance) {
  bw_weights_error_t berr = BW_WEIGHTS_NO_ERROR;

  // Wed + Wmd + Wgd == 1
  if (!CHECK_EQ(Wed + Wmd + Wgd, scale, margin)) {
    berr = BW_WEIGHTS_SUMD_ERROR;
    goto out;
  }

  // Wmg + Wgg == 1
  if (!CHECK_EQ(Wmg + Wgg, scale, margin)) {
    berr = BW_WEIGHTS_SUMG_ERROR;
    goto out;
  }

  // Wme + Wee == 1
  if (!CHECK_EQ(Wme + Wee, scale, margin)) {
    berr = BW_WEIGHTS_SUME_ERROR;
    goto out;
  }

  // Verify weights within range 0->1
  if (!RANGE_CHECK(Wgg, Wgd, Wmg, Wme, Wmd, Wed, Wee, scale)) {
    berr = BW_WEIGHTS_RANGE_ERROR;
    goto out;
  }

  if (do_balance) {
    // Wgg*G + Wgd*D == Wee*E + Wed*D, already scaled
    if (!CHECK_EQ(Wgg*G + Wgd*D, Wee*E + Wed*D, (margin*T)/3)) {
      berr = BW_WEIGHTS_BALANCE_EG_ERROR;
      goto out;
    }

    // Wgg*G + Wgd*D == M*scale + Wmd*D + Wme*E + Wmg*G, already scaled
    if (!CHECK_EQ(Wgg*G + Wgd*D, M*scale + Wmd*D + Wme*E + Wmg*G,
                (margin*T)/3)) {
      berr = BW_WEIGHTS_BALANCE_MID_ERROR;
      goto out;
    }
  }

 out:
  if (berr) {
    log_info(LD_DIR,
             "Bw weight mismatch %d. G="I64_FORMAT" M="I64_FORMAT
             " E="I64_FORMAT" D="I64_FORMAT" T="I64_FORMAT
             " Wmd=%d Wme=%d Wmg=%d Wed=%d Wee=%d"
             " Wgd=%d Wgg=%d Wme=%d Wmg=%d",
             berr,
             I64_PRINTF_ARG(G), I64_PRINTF_ARG(M), I64_PRINTF_ARG(E),
             I64_PRINTF_ARG(D), I64_PRINTF_ARG(T),
             (int)Wmd, (int)Wme, (int)Wmg, (int)Wed, (int)Wee,
             (int)Wgd, (int)Wgg, (int)Wme, (int)Wmg);
  }

  return berr;
}