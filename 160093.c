  int get_hv_rbg(int x, int y, int hc)
  {
    float hv1 = 2 * nraw[nr_offset(y - 1, x)][hc ^ 2] /
                (nraw[nr_offset(y - 2, x)][1] + nraw[nr_offset(y, x)][1]);
    float hv2 = 2 * nraw[nr_offset(y + 1, x)][hc ^ 2] /
                (nraw[nr_offset(y + 2, x)][1] + nraw[nr_offset(y, x)][1]);
    float kv = calc_dist(hv1, hv2) *
               calc_dist(nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1],
                         (nraw[nr_offset(y - 2, x)][1] *
                          nraw[nr_offset(y + 2, x)][1]));
    kv *= kv;
    kv *= kv;
    kv *= kv;
    float dv = kv * calc_dist(nraw[nr_offset(y - 3, x)][hc ^ 2] *
                                  nraw[nr_offset(y + 3, x)][hc ^ 2],
                              nraw[nr_offset(y - 1, x)][hc ^ 2] *
                                  nraw[nr_offset(y + 1, x)][hc ^ 2]);
    float hh1 = 2 * nraw[nr_offset(y, x - 1)][hc] /
                (nraw[nr_offset(y, x - 2)][1] + nraw[nr_offset(y, x)][1]);
    float hh2 = 2 * nraw[nr_offset(y, x + 1)][hc] /
                (nraw[nr_offset(y, x + 2)][1] + nraw[nr_offset(y, x)][1]);
    float kh = calc_dist(hh1, hh2) *
               calc_dist(nraw[nr_offset(y, x)][1] * nraw[nr_offset(y, x)][1],
                         (nraw[nr_offset(y, x - 2)][1] *
                          nraw[nr_offset(y, x + 2)][1]));
    kh *= kh;
    kh *= kh;
    kh *= kh;
    float dh =
        kh * calc_dist(
                 nraw[nr_offset(y, x - 3)][hc] * nraw[nr_offset(y, x + 3)][hc],
                 nraw[nr_offset(y, x - 1)][hc] * nraw[nr_offset(y, x + 1)][hc]);
    float e = calc_dist(dh, dv);
    char d = dh < dv ? (e > Tg() ? HORSH : HOR) : (e > Tg() ? VERSH : VER);
    return d;
  }