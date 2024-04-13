void AAHD::make_ahd_rb_hv(int i)
{
  int iwidth = libraw.imgdata.sizes.iwidth;
  int js = libraw.COLOR(i, 0) & 1;
  int kc = libraw.COLOR(i, js);
  js ^= 1; // начальная координата зелёного
  int hvdir[2] = {Pe, Ps};
  // интерполяция вертикальных вертикально и горизонтальных горизонтально
  for (int j = js; j < iwidth; j += 2)
  {
    int x = j + nr_margin;
    int y = i + nr_margin;
    int moff = nr_offset(y, x);
    for (int d = 0; d < 2; ++d)
    {
      ushort3 *cnr;
      cnr = &rgb_ahd[d][moff];
      int c = kc ^ (d << 1); // цвет соответсвенного направления, для
                             // горизонтального c = kc, для вертикального c=kc^2
      int h1 = cnr[-hvdir[d]][c] - cnr[-hvdir[d]][1];
      int h2 = cnr[+hvdir[d]][c] - cnr[+hvdir[d]][1];
      int h0 = (h1 + h2) / 2;
      int eg = cnr[0][1] + h0;
      //			int min = MIN(cnr[-hvdir[d]][c], cnr[+hvdir[d]][c]);
      //			int max = MAX(cnr[-hvdir[d]][c], cnr[+hvdir[d]][c]);
      //			min -= min / OverFraction;
      //			max += max / OverFraction;
      //			if (eg < min)
      //				eg = min - sqrt(min - eg);
      //			else if (eg > max)
      //				eg = max + sqrt(eg - max);
      if (eg > channel_maximum[c])
        eg = channel_maximum[c];
      else if (eg < channel_minimum[c])
        eg = channel_minimum[c];
      cnr[0][c] = eg;
    }
  }
}