void LibRaw::lch_to_rgb(double (*image2)[3])
{
  int indx;
  for (indx = 0; indx < height * width; indx++)
  {

    image[indx][0] = CLIP(image2[indx][0] / 3.0 - image2[indx][2] / 6.0 +
                          image2[indx][1] / 3.464101615);
    image[indx][1] = CLIP(image2[indx][0] / 3.0 - image2[indx][2] / 6.0 -
                          image2[indx][1] / 3.464101615);
    image[indx][2] = CLIP(image2[indx][0] / 3.0 + image2[indx][2] / 3.0);
  }
}