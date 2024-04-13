void CLASS Canon_CameraSettings()
{
  fseek(ifp, 10, SEEK_CUR);
  imgdata.shootinginfo.DriveMode = get2();
  get2();
  imgdata.shootinginfo.FocusMode = get2();
  fseek(ifp, 18, SEEK_CUR);
  imgdata.shootinginfo.MeteringMode = get2();
  get2();
  imgdata.shootinginfo.AFPoint = get2();
  imgdata.shootinginfo.ExposureMode = get2();
  get2();
  imgdata.lens.makernotes.LensID = get2();
  imgdata.lens.makernotes.MaxFocal = get2();
  imgdata.lens.makernotes.MinFocal = get2();
  imgdata.lens.makernotes.CanonFocalUnits = get2();
  if (imgdata.lens.makernotes.CanonFocalUnits > 1) {
    imgdata.lens.makernotes.MaxFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
    imgdata.lens.makernotes.MinFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
  }
  imgdata.lens.makernotes.MaxAp = _CanonConvertAperture(get2());
  imgdata.lens.makernotes.MinAp = _CanonConvertAperture(get2());
  fseek(ifp, 12, SEEK_CUR);
  imgdata.shootinginfo.ImageStabilization = get2();
}