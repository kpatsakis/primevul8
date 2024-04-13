  int get_hv_grb(int x, int y, int kc)
  {
    float hv1 = 2 * nraw[nr_offset(y - 1, x)][1] /
                (nraw[nr_offset(y - 2, x)][kc] + nraw[nr_offset(y, x)][kc]);
    float hv2 = 2 * nraw[nr_offset(y + 1, x)][1] /
                (nraw[nr_offset(y + 2, x)][kc] + nraw[nr_offset(y, x)][kc]);
    float kv = calc_dist(hv1, hv2) *
               calc_dist(nraw[nr_offset(y, x)][kc] * nraw[nr_offset(y, x)][kc],
                         (nraw[nr_offset(y - 2, x)][kc] *
                          nraw[nr_offset(y + 2, x)][kc]));
    kv *= kv;
    kv *= kv;
    kv *= kv;
    float dv =
        kv *
        calc_dist(nraw[nr_offset(y - 3, x)][1] * nraw[nr_offset(y + 3, x)][1],
                  nraw[nr_offset(y - 1, x)][1] * nraw[nr_offset(y + 1, x)][1]);
    float hh1 = 2 * nraw[nr_offset(y, x - 1)][1] /
                (nraw[nr_offset(y, x - 2)][kc] + nraw[nr_offset(y, x)][kc]);
    float hh2 = 2 * nraw[nr_offset(y, x + 1)][1] /
                (nraw[nr_offset(y, x + 2)][kc] + nraw[nr_offset(y, x)][kc]);
    float kh = calc_dist(hh1, hh2) *
               calc_dist(nraw[nr_offset(y, x)][kc] * nraw[nr_offset(y, x)][kc],
                         (nraw[nr_offset(y, x - 2)][kc] *
                          nraw[nr_offset(y, x + 2)][kc]));
    kh *= kh;
    kh *= kh;
    kh *= kh;
    float dh =
        kh *
        calc_dist(nraw[nr_offset(y, x - 3)][1] * nraw[nr_offset(y, x + 3)][1],
                  nraw[nr_offset(y, x - 1)][1] * nraw[nr_offset(y, x + 1)][1]);
    float e = calc_dist(dh, dv);
    char d = dh < dv ? (e > Tg() ? HORSH : HOR) : (e > Tg() ? VERSH : VER);
    return d;
  }