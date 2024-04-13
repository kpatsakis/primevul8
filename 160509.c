  int get_diag_grb(int x, int y, int kc)
  {
    float hlu =
        nraw[nr_offset(y - 1, x - 1)][1] / nraw[nr_offset(y - 1, x - 1)][kc];
    float hrd =
        nraw[nr_offset(y + 1, x + 1)][1] / nraw[nr_offset(y + 1, x + 1)][kc];
    float hru =
        nraw[nr_offset(y - 1, x + 1)][1] / nraw[nr_offset(y - 1, x + 1)][kc];
    float hld =
        nraw[nr_offset(y + 1, x - 1)][1] / nraw[nr_offset(y + 1, x - 1)][kc];
    float dlurd =
        calc_dist(hlu, hrd) *
        calc_dist(nraw[nr_offset(y - 1, x - 1)][1] *
                      nraw[nr_offset(y + 1, x + 1)][1],
                  nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1]);
    float druld =
        calc_dist(hlu, hrd) *
        calc_dist(nraw[nr_offset(y - 1, x + 1)][1] *
                      nraw[nr_offset(y + 1, x - 1)][1],
                  nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1]);
    float e = calc_dist(dlurd, druld);
    char d =
        druld < dlurd ? (e > T() ? RULDSH : RULD) : (e > T() ? LURDSH : LURD);
    return d;
  }