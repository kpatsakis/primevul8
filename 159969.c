void LibRaw::dcb_restore_from_buffer(float (*image2)[3])
{
  int indx;

  for (indx = 0; indx < height * width; indx++)
  {
    image[indx][0] = image2[indx][0]; // R
    image[indx][2] = image2[indx][2]; // B
  }
}