int LibRaw::unpack_thumb(void)
{
  CHECK_ORDER_LOW(LIBRAW_PROGRESS_IDENTIFY);
  CHECK_ORDER_BIT(LIBRAW_PROGRESS_THUMB_LOAD);

  try {
    if(!libraw_internal_data.internal_data.input)
      return LIBRAW_INPUT_CLOSED;

    if ( !ID.toffset)
      {
        return LIBRAW_NO_THUMBNAIL;
      }
    else if (thumb_load_raw)
      {
        kodak_thumb_loader();
        T.tformat = LIBRAW_THUMBNAIL_BITMAP;
        SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
        return 0;
      }
    else
      {
        ID.input->seek(ID.toffset, SEEK_SET);
        if ( write_thumb == &LibRaw::jpeg_thumb)
          {
            if(T.thumb) free(T.thumb);
            T.thumb = (char *) malloc (T.tlength);
            merror (T.thumb, "jpeg_thumb()");
            ID.input->read (T.thumb, 1, T.tlength);
			T.thumb[0] = 0xff;
			T.thumb[1] = 0xd8;
            T.tcolors = 3;
            T.tformat = LIBRAW_THUMBNAIL_JPEG;
            SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
            return 0;
          }
        else if (write_thumb == &LibRaw::ppm_thumb)
          {
            T.tlength = T.twidth * T.theight*3;
            if(T.thumb) free(T.thumb);

            T.thumb = (char *) malloc (T.tlength);
            merror (T.thumb, "ppm_thumb()");

            ID.input->read(T.thumb, 1, T.tlength);

            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
            return 0;

          }
        else if (write_thumb == &LibRaw::ppm16_thumb)
          {
            T.tlength = T.twidth * T.theight*3;
            ushort *t_thumb = (ushort*)calloc(T.tlength,2);
            ID.input->read(t_thumb,2,T.tlength);
            if ((libraw_internal_data.unpacker_data.order == 0x4949) == (ntohs(0x1234) == 0x1234))
              swab ((char*)t_thumb, (char*)t_thumb, T.tlength*2);

            if(T.thumb) free(T.thumb);
            T.thumb = (char *) malloc (T.tlength);
            merror (T.thumb, "ppm_thumb()");
            for (int i=0; i < T.tlength; i++)
              T.thumb[i] = t_thumb[i] >> 8;
            free(t_thumb);
            T.tformat = LIBRAW_THUMBNAIL_BITMAP;
            SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
            return 0;

          }
        else if (write_thumb == &LibRaw::x3f_thumb_loader)
          {
            x3f_thumb_loader();
            SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
            return 0;
          }
#ifdef LIBRAW_DEMOSAIC_PACK_GPL2
        else if (write_thumb == &LibRaw::foveon_thumb)
          {
            foveon_thumb_loader();
            // may return with error, so format is set in
            // foveon thumb loader itself
            SET_PROC_FLAG(LIBRAW_PROGRESS_THUMB_LOAD);
            return 0;
          }
        // else if -- all other write_thumb cases!
#endif
        else
          {
            return LIBRAW_UNSUPPORTED_THUMBNAIL;
          }
      }
    // last resort
    return LIBRAW_UNSUPPORTED_THUMBNAIL;
  }
  catch ( LibRaw_exceptions err) {
    EXCEPTION_HANDLER(err);
  }

}