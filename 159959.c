  int get_diag_rbg(int x, int y, int hc)
  {
    float dlurd = calc_dist(
        nraw[nr_offset(y - 1, x - 1)][1] * nraw[nr_offset(y + 1, x + 1)][1],
        nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1]);
    float druld = calc_dist(
        nraw[nr_offset(y - 1, x + 1)][1] * nraw[nr_offset(y + 1, x - 1)][1],
        nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1]);
    float e = calc_dist(dlurd, druld);
    char d =
        druld < dlurd ? (e > T() ? RULDSH : RULD) : (e > T() ? LURDSH : LURD);
    return d;
  }