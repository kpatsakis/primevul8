int LibRaw::minolta_z2()
{
  int i, nz;
  char tail[424];

  fseek(ifp, -sizeof tail, SEEK_END);
  fread(tail, 1, sizeof tail, ifp);
  for (nz = i = 0; i < sizeof tail; i++)
    if (tail[i])
      nz++;
  return nz > 20;
}