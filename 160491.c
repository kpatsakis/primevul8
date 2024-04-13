  static inline float scale_over(float ec, float base)
  {
    float s = base * .4;
    float o = ec - base;
    return base + sqrt(s * (o + s)) - s;
  }