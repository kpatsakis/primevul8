void LibRaw::rgb_to_lch(double (*image2)[3])
{
  int indx;
  for (indx = 0; indx < height * width; indx++)
  {

    image2[indx][0] = image[indx][0] + image[indx][1] + image[indx][2]; // L
    image2[indx][1] = 1.732050808 * (image[indx][0] - image[indx][1]);  // C
    image2[indx][2] =
        2.0 * image[indx][2] - image[indx][0] - image[indx][1]; // H
  }
}