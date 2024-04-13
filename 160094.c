void LibRaw::Canon_CameraSettings(unsigned len)
{
  fseek(ifp, 10, SEEK_CUR);
  imgdata.shootinginfo.DriveMode = get2(); // 5
  get2();
  imgdata.shootinginfo.FocusMode = get2(); // 7
  fseek(ifp, 18, SEEK_CUR);
  imgdata.shootinginfo.MeteringMode = get2(); // 17
  get2();
  imgdata.shootinginfo.AFPoint = get2();      // 19
  imgdata.shootinginfo.ExposureMode = get2(); // 20
  get2();
  ilm.LensID = get2();          // 22
  ilm.MaxFocal = get2();        // 23
  ilm.MinFocal = get2();        // 24
  ilm.FocalUnits = get2(); // 25
  if (ilm.FocalUnits > 1)
  {
    ilm.MaxFocal /= (float)ilm.FocalUnits;
    ilm.MinFocal /= (float)ilm.FocalUnits;
  }
  ilm.MaxAp = _CanonConvertAperture(get2()); // 26
  ilm.MinAp = _CanonConvertAperture(get2()); // 27
  if (len >= 36)
  {
    fseek(ifp, 12, SEEK_CUR);
    imgdata.shootinginfo.ImageStabilization = get2(); // 34
  }
  else
    return;
  if (len >= 48)
  {
    fseek(ifp, 22, SEEK_CUR);
    imCanon.SRAWQuality = get2(); // 46
  }
}