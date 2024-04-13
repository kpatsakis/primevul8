short LibRaw::KodakIllumMatrix(unsigned type, float *romm_camIllum)
{
  int c, j, romm_camTemp[9], romm_camScale[3];
  if (type == 10)
  {
    for (j = 0; j < 9; j++)
      ((float *)romm_camIllum)[j] = getreal(type);
    return 1;
  }
  else if (type == 9)
  {
    FORC3
    {
      romm_camScale[c] = 0;
      for (j = 0; j < 3; j++)
      {
        romm_camTemp[c * 3 + j] = get4();
        romm_camScale[c] += romm_camTemp[c * 3 + j];
      }
    }
    if ((romm_camScale[0] > 0x1fff) && (romm_camScale[1] > 0x1fff) &&
        (romm_camScale[2] > 0x1fff))
    {
      FORC3 for (j = 0; j < 3; j++)((float *)romm_camIllum)[c * 3 + j] =
          ((float)romm_camTemp[c * 3 + j]) / ((float)romm_camScale[c]);
      return 1;
    }
  }
  return 0;
}