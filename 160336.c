void LibRaw::getOlympus_SensorTemperature(unsigned len)
{
  if (OlyID != 0x0ULL)
  {
    short temp = get2();
    if ((OlyID == 0x4434303430ULL) || // E-1
        (OlyID == 0x5330303336ULL) || // E-M5
        (len != 1))
      imgdata.makernotes.common.SensorTemperature = (float)temp;
    else if ((temp != -32768) && (temp != 0))
    {
      if (temp > 199)
        imgdata.makernotes.common.SensorTemperature = 86.474958f - 0.120228f * (float)temp;
      else
        imgdata.makernotes.common.SensorTemperature = (float)temp;
    }
  }
  return;
}