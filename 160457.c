void LibRaw::dcb_copy_to_buffer(float (*image2)[3])
{
  int indx;

  for (indx = 0; indx < height * width; indx++)
  {
    image2[indx][0] = image[indx][0]; // R
    image2[indx][2] = image[indx][2]; // B
  }
}