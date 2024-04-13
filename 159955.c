int LibRaw::canon_600_color(int ratio[2], int mar)
{
  int clipped = 0, target, miss;

  if (flash_used)
  {
    if (ratio[1] < -104)
    {
      ratio[1] = -104;
      clipped = 1;
    }
    if (ratio[1] > 12)
    {
      ratio[1] = 12;
      clipped = 1;
    }
  }
  else
  {
    if (ratio[1] < -264 || ratio[1] > 461)
      return 2;
    if (ratio[1] < -50)
    {
      ratio[1] = -50;
      clipped = 1;
    }
    if (ratio[1] > 307)
    {
      ratio[1] = 307;
      clipped = 1;
    }
  }
  target = flash_used || ratio[1] < 197 ? -38 - (398 * ratio[1] >> 10)
                                        : -123 + (48 * ratio[1] >> 10);
  if (target - mar <= ratio[0] && target + 20 >= ratio[0] && !clipped)
    return 0;
  miss = target - ratio[0];
  if (abs(miss) >= mar * 4)
    return 2;
  if (miss < -20)
    miss = -20;
  if (miss > mar)
    miss = mar;
  ratio[0] = target - miss;
  return 1;
}