void LibRaw::x3f_thumb_loader()
{
  x3f_t *x3f = (x3f_t*)_x3f_data;
  if(!x3f) return; // No data pointer set
  x3f_directory_entry_t *DE = x3f_get_thumb_jpeg(x3f);
  if(!DE)
    DE = x3f_get_thumb_plain(x3f);
  if(!DE)
    return;
  if(X3F_OK != x3f_load_data(x3f, DE))
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  imgdata.thumbnail.twidth = ID->columns;
  imgdata.thumbnail.theight = ID->rows;
  imgdata.thumbnail.tcolors = 3;
  if(imgdata.thumbnail.tformat == LIBRAW_THUMBNAIL_JPEG)
    {
      imgdata.thumbnail.thumb = (char*)malloc(ID->data_size);
      merror(imgdata.thumbnail.thumb,"LibRaw::x3f_thumb_loader()");
      memmove(imgdata.thumbnail.thumb,ID->data,ID->data_size);
      imgdata.thumbnail.tlength = ID->data_size;
    }
  else if(imgdata.thumbnail.tformat == LIBRAW_THUMBNAIL_BITMAP)
    {
      imgdata.thumbnail.tlength = ID->columns * ID->rows * 3;
      imgdata.thumbnail.thumb = (char*)malloc(ID->columns * ID->rows * 3);
      merror(imgdata.thumbnail.thumb,"LibRaw::x3f_thumb_loader()");
      char *src0 = (char*)ID->data;
      for(int row = 0; row < ID->rows;row++)
        {
		  int offset = row * ID->row_stride;
		  if (offset + ID->columns * 3 > ID->data_size)
			break;
          char *dest = &imgdata.thumbnail.thumb[row*ID->columns*3];
          char *src = &src0[offset];
          memmove(dest,src,ID->columns*3);
        }
    }
}