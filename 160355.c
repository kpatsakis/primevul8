  static float powf_lim(float a, float b, float limup)
  {
    return (b > limup || b < -limup) ? 0.f : powf(a, b);
  }