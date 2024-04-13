static inline void deshufflePalette(Image *image,PixelInfo* oldColormap)
{
  const size_t
    pages=image->colors/32,  /* Pages per CLUT */
    blocks=4,  /* Blocks per page */
    colors=8;  /* Colors per block */

  int
    page;

  size_t
    i=0;

  (void) memcpy(oldColormap,image->colormap,(size_t)image->colors*
    sizeof(*oldColormap));

  /*
   * Swap the 2nd and 3rd block in each page
   */
  for (page=0; page < (ssize_t) pages; page++)
  {
    memcpy(&(image->colormap[i+1*colors]),&(oldColormap[i+2*colors]),colors*
      sizeof(PixelInfo));
    memcpy(&(image->colormap[i+2*colors]),&(oldColormap[i+1*colors]),colors*
      sizeof(PixelInfo));

    i+=blocks*colors;
  }
}