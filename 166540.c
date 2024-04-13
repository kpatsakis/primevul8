save_image (const gchar  *filename,
            gint32        image,
            gint32        layer,
            GError      **error)
{
  FILE          *fp;
  GimpPixelRgn   pixel_rgn;
  GimpDrawable  *drawable;
  GimpImageType  drawable_type;
  guchar        *cmap= NULL;
  guchar        *pixels;
  gint           offset_x, offset_y;
  guint          width, height;
  gint           colors, i;
  guint8         header_buf[128];

  drawable = gimp_drawable_get (layer);
  drawable_type = gimp_drawable_type (layer);
  gimp_drawable_offsets (layer, &offset_x, &offset_y);
  width = drawable->width;
  height = drawable->height;
  gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, width, height, FALSE, FALSE);

  gimp_progress_init_printf (_("Saving '%s'"),
                             gimp_filename_to_utf8 (filename));

  pcx_header.manufacturer = 0x0a;
  pcx_header.version = 5;
  pcx_header.compression = 1;

  switch (drawable_type)
    {
    case GIMP_INDEXED_IMAGE:
      cmap = gimp_image_get_colormap (image, &colors);
      pcx_header.bpp = 8;
      pcx_header.bytesperline = GUINT16_TO_LE (width);
      pcx_header.planes = 1;
      pcx_header.color = GUINT16_TO_LE (1);
      break;

    case GIMP_RGB_IMAGE:
      pcx_header.bpp = 8;
      pcx_header.planes = 3;
      pcx_header.color = GUINT16_TO_LE (1);
      pcx_header.bytesperline = GUINT16_TO_LE (width);
      break;

    case GIMP_GRAY_IMAGE:
      pcx_header.bpp = 8;
      pcx_header.planes = 1;
      pcx_header.color = GUINT16_TO_LE (2);
      pcx_header.bytesperline = GUINT16_TO_LE (width);
      break;

    default:
      g_message (_("Cannot save images with alpha channel."));
      return FALSE;
  }

  if ((fp = g_fopen (filename, "wb")) == NULL)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for writing: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return FALSE;
    }

  pixels = (guchar *) g_malloc (width * height * pcx_header.planes);
  gimp_pixel_rgn_get_rect (&pixel_rgn, pixels, 0, 0, width, height);

  if ((offset_x < 0) || (offset_x > (1<<16)))
    {
      g_message (_("Invalid X offset: %d"), offset_x);
      return FALSE;
    }

  if ((offset_y < 0) || (offset_y > (1<<16)))
    {
      g_message (_("Invalid Y offset: %d"), offset_y);
      return FALSE;
    }

  if (offset_x + width - 1 > (1<<16))
    {
      g_message (_("Right border out of bounds (must be < %d): %d"), (1<<16),
                 offset_x + width - 1);
      return FALSE;
    }

  if (offset_y + height - 1 > (1<<16))
    {
      g_message (_("Bottom border out of bounds (must be < %d): %d"), (1<<16),
                 offset_y + height - 1);
      return FALSE;
    }

  pcx_header.x1 = GUINT16_TO_LE ((guint16)offset_x);
  pcx_header.y1 = GUINT16_TO_LE ((guint16)offset_y);
  pcx_header.x2 = GUINT16_TO_LE ((guint16)(offset_x + width - 1));
  pcx_header.y2 = GUINT16_TO_LE ((guint16)(offset_y + height - 1));

  pcx_header.hdpi = GUINT16_TO_LE (300);
  pcx_header.vdpi = GUINT16_TO_LE (300);
  pcx_header.reserved = 0;

  pcx_header_to_buffer (header_buf);

  fwrite (header_buf, 128, 1, fp);

  switch (drawable_type)
    {
    case GIMP_INDEXED_IMAGE:
      save_8 (fp, width, height, pixels);
      fputc (0x0c, fp);
      fwrite (cmap, colors, 3, fp);
      for (i = colors; i < 256; i++)
        {
          fputc (0, fp);
          fputc (0, fp);
          fputc (0, fp);
        }
      break;

    case GIMP_RGB_IMAGE:
      save_24 (fp, width, height, pixels);
      break;

    case GIMP_GRAY_IMAGE:
      save_8 (fp, width, height, pixels);
      fputc (0x0c, fp);
      for (i = 0; i < 256; i++)
        {
          fputc ((guchar) i, fp);
          fputc ((guchar) i, fp);
          fputc ((guchar) i, fp);
        }
      break;

    default:
      return FALSE;
    }

  gimp_drawable_detach (drawable);
  g_free (pixels);

  if (fclose (fp) != 0)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Writing to file '%s' failed: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return FALSE;
    }

  return TRUE;
}