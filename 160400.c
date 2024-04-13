  static inline float scale_under(float ec, float base)
  {
    float s = base * .6;
    float o = base - ec;
    return base - sqrt(s * (o + s)) + s;
  }