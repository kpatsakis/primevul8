save_image (const gchar  *filename,
            gint32        image_ID,
            gint32        drawable_ID,
            GError      **error)
{
  gint          fd;
  BrushHeader   bh;
  guchar       *buffer;
  GimpDrawable *drawable;
  gint          line;
  gint          x;
  gint          bpp;
  GimpPixelRgn  pixel_rgn;
  GimpRGB       gray, white;

  gimp_rgba_set_uchar (&white, 255, 255, 255, 255);

  switch (gimp_drawable_type (drawable_ID))
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      bpp = 4;
      break;

    case GIMP_GRAY_IMAGE:
    case GIMP_GRAYA_IMAGE:
      bpp = 1;
      break;

    default:
      {
        g_message (_("GIMP brushes are either GRAYSCALE or RGBA"));
        return FALSE;
      }
    }

  fd = g_open (filename, O_CREAT | O_TRUNC | O_WRONLY | _O_BINARY, 0666);

  if (fd == -1)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for writing: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return FALSE;
    }

  gimp_progress_init_printf (_("Saving '%s'"),
                             gimp_filename_to_utf8 (filename));

  drawable = gimp_drawable_get (drawable_ID);

  bh.header_size  = g_htonl (sizeof (BrushHeader) +
                             strlen (info.description) + 1);
  bh.version      = g_htonl (2);
  bh.width        = g_htonl (drawable->width);
  bh.height       = g_htonl (drawable->height);
  bh.bytes        = g_htonl (bpp);
  bh.magic_number = g_htonl (GBRUSH_MAGIC);
  bh.spacing      = g_htonl (info.spacing);

  if (write (fd, &bh, sizeof (BrushHeader)) != sizeof (BrushHeader))
    {
      close (fd);
      return FALSE;
    }

  if (write (fd, info.description, strlen (info.description) + 1) !=
      strlen (info.description) + 1)
    {
      close (fd);
      return FALSE;
    }

  gimp_pixel_rgn_init (&pixel_rgn, drawable,
                       0, 0, drawable->width, drawable->height,
                       FALSE, FALSE);

  buffer = g_new (guchar, drawable->width * MAX (bpp, drawable->bpp));

  for (line = 0; line < drawable->height; line++)
    {
      gimp_pixel_rgn_get_row (&pixel_rgn, buffer, 0, line, drawable->width);

      switch (drawable->bpp)
        {
        case 1:
          /*  invert  */
          for (x = 0; x < drawable->width; x++)
            buffer[x] = 255 - buffer[x];
          break;

        case 2:
          for (x = 0; x < drawable->width; x++)
            {
              /*  apply alpha channel  */
              gimp_rgba_set_uchar (&gray,
                                   buffer[2 * x],
                                   buffer[2 * x],
                                   buffer[2 * x],
                                   buffer[2 * x + 1]);
              gimp_rgb_composite (&gray, &white, GIMP_RGB_COMPOSITE_BEHIND);
              gimp_rgba_get_uchar (&gray, &buffer[x], NULL, NULL, NULL);
              /* invert */
              buffer[x] = 255 - buffer[x];
            }
          break;

        case 3:
          /*  add alpha channel  */
          for (x = drawable->width - 1; x >= 0; x--)
            {
              buffer[x * 4 + 3] = 0xFF;
              buffer[x * 4 + 2] = buffer[x * 3 + 2];
              buffer[x * 4 + 1] = buffer[x * 3 + 1];
              buffer[x * 4 + 0] = buffer[x * 3 + 0];
            }
          break;
        }

      if (write (fd, buffer, drawable->width * bpp) != drawable->width * bpp)
        {
          g_free (buffer);
          close (fd);
          return FALSE;
        }

      gimp_progress_update ((gdouble) line / (gdouble) drawable->height);
    }

  g_free (buffer);

  gimp_drawable_detach (drawable);

  close (fd);

  return TRUE;
}