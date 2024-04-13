void LibRaw::getOlympus_CameraType2()
{

  if (OlyID != 0x0ULL)
    return;

  int i = 0;

  fread(imOly.CameraType2, 6, 1, ifp);
  imOly.CameraType2[5] = 0;
  while ((i < 6) && imOly.CameraType2[i])
  {
    OlyID = OlyID << 8 | imOly.CameraType2[i];
    if (i < 5 && isspace(imOly.CameraType2[i + 1]))
      imOly.CameraType2[i + 1] = '\0';
    i++;
  }
  unique_id = OlyID;
  setOlympusBodyFeatures(OlyID);
  return;
}