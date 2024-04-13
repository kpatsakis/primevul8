  static inline float calc_dist(int c1, int c2) throw()
  {
    return c1 > c2 ? (float)c1 / c2 : (float)c2 / c1;
  }