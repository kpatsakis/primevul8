gst_riff_create_video_caps (guint32 codec_fcc,
    gst_riff_strh * strh, gst_riff_strf_vids * strf,
    GstBuffer * strf_data, GstBuffer * strd_data, char **codec_name)
{
  GstCaps *caps = NULL;
  GstBuffer *palette = NULL;

  GST_DEBUG ("video fourcc %" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (codec_fcc));

  switch (codec_fcc) {
    case GST_RIFF_DIB:         /* uncompressed RGB */
    case GST_RIFF_rgb:
    case GST_RIFF_RGB:
    case GST_RIFF_RAW:
    {
      gint bpp = (strf && strf->bit_cnt != 0) ? strf->bit_cnt : 8;

      if (strf) {
        if (bpp == 8) {
          caps = gst_caps_new_simple ("video/x-raw",
              "format", G_TYPE_STRING, "RGB8P", NULL);
        } else if (bpp == 24) {
          caps = gst_caps_new_simple ("video/x-raw",
              "format", G_TYPE_STRING, "BGR", NULL);
        } else if (bpp == 32) {
          caps = gst_caps_new_simple ("video/x-raw",
              "format", G_TYPE_STRING, "BGRx", NULL);
        } else {
          GST_WARNING ("Unhandled DIB RGB depth: %d", bpp);
          return NULL;
        }
      } else {
        /* for template */
        caps =
            gst_caps_from_string ("video/x-raw, format = (string) "
            "{ RGB8P, BGR, BGRx }");
      }

      palette = strf_data;
      strf_data = NULL;
      if (codec_name) {
        if (bpp == 8)
          *codec_name = g_strdup_printf ("Palettized %d-bit RGB", bpp);
        else
          *codec_name = g_strdup_printf ("%d-bit RGB", bpp);
      }
      break;
    }

    case GST_MAKE_FOURCC ('G', 'R', 'E', 'Y'):
    case GST_MAKE_FOURCC ('Y', '8', '0', '0'):
    case GST_MAKE_FOURCC ('Y', '8', ' ', ' '):
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "GRAY8", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed 8-bit monochrome");
      break;

    case GST_MAKE_FOURCC ('r', '2', '1', '0'):
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "r210", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed RGB 10-bit 4:4:4");
      break;

    case GST_RIFF_I420:
    case GST_RIFF_i420:
    case GST_RIFF_IYUV:
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "I420", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed planar YUV 4:2:0");
      break;

    case GST_RIFF_YUY2:
    case GST_RIFF_yuy2:
    case GST_MAKE_FOURCC ('Y', 'U', 'N', 'V'):
    case GST_MAKE_FOURCC ('Y', 'U', 'Y', 'V'):
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "YUY2", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed YUV 4:2:2");
      break;

    case GST_RIFF_YVU9:
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "YVU9", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed YVU 4:1:0");
      break;

    case GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'):
    case GST_MAKE_FOURCC ('2', 'v', 'u', 'y'):
    case GST_MAKE_FOURCC ('H', 'D', 'Y', 'C'):
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "UYVY", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed YUV 4:2:2");
      break;

    case GST_RIFF_YV12:
    case GST_RIFF_yv12:
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "YV12", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed YVU 4:2:2");
      break;
    case GST_MAKE_FOURCC ('v', '2', '1', '0'):
      caps = gst_caps_new_simple ("video/x-raw",
          "format", G_TYPE_STRING, "v210", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Uncompressed packed 10-bit YUV 4:2:2");
      break;

    case GST_RIFF_MJPG:        /* YUY2 MJPEG */
    case GST_RIFF_mJPG:
    case GST_MAKE_FOURCC ('A', 'V', 'R', 'n'):
    case GST_RIFF_IJPG:
    case GST_MAKE_FOURCC ('i', 'j', 'p', 'g'):
    case GST_RIFF_DMB1:
    case GST_RIFF_dmb1:
    case GST_MAKE_FOURCC ('A', 'C', 'D', 'V'):
    case GST_MAKE_FOURCC ('Q', 'I', 'V', 'G'):
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("Motion JPEG");
      break;

    case GST_RIFF_JPEG:        /* generic (mostly RGB) MJPEG */
    case GST_RIFF_jpeg:
    case GST_MAKE_FOURCC ('j', 'p', 'e', 'g'): /* generic (mostly RGB) MJPEG */
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("JPEG Still Image");
      break;

    case GST_MAKE_FOURCC ('P', 'I', 'X', 'L'): /* Miro/Pinnacle fourccs */
    case GST_RIFF_VIXL:        /* Miro/Pinnacle fourccs */
    case GST_RIFF_vixl:
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("Miro/Pinnacle Motion JPEG");
      break;

    case GST_MAKE_FOURCC ('C', 'J', 'P', 'G'):
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("Creative Webcam JPEG");
      break;

    case GST_MAKE_FOURCC ('S', 'L', 'M', 'J'):
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("SL Motion JPEG");
      break;

    case GST_MAKE_FOURCC ('J', 'P', 'G', 'L'):
      caps = gst_caps_new_empty_simple ("image/jpeg");
      if (codec_name)
        *codec_name = g_strdup ("Pegasus Lossless JPEG");
      break;

    case GST_MAKE_FOURCC ('L', 'O', 'C', 'O'):
      caps = gst_caps_new_empty_simple ("video/x-loco");
      if (codec_name)
        *codec_name = g_strdup ("LOCO Lossless");
      break;

    case GST_MAKE_FOURCC ('S', 'P', '5', '3'):
    case GST_MAKE_FOURCC ('S', 'P', '5', '4'):
    case GST_MAKE_FOURCC ('S', 'P', '5', '5'):
    case GST_MAKE_FOURCC ('S', 'P', '5', '6'):
    case GST_MAKE_FOURCC ('S', 'P', '5', '7'):
    case GST_MAKE_FOURCC ('S', 'P', '5', '8'):
      caps = gst_caps_new_empty_simple ("video/sp5x");
      if (codec_name)
        *codec_name = g_strdup ("Sp5x-like JPEG");
      break;

    case GST_MAKE_FOURCC ('Z', 'M', 'B', 'V'):
      caps = gst_caps_new_empty_simple ("video/x-zmbv");
      if (codec_name)
        *codec_name = g_strdup ("Zip Motion Block video");
      break;

    case GST_MAKE_FOURCC ('H', 'F', 'Y', 'U'):
      caps = gst_caps_new_empty_simple ("video/x-huffyuv");
      if (strf) {
        gst_caps_set_simple (caps, "bpp",
            G_TYPE_INT, (int) strf->bit_cnt, NULL);
      }
      if (codec_name)
        *codec_name = g_strdup ("Huffman Lossless Codec");
      break;

    case GST_MAKE_FOURCC ('M', 'P', 'E', 'G'):
    case GST_MAKE_FOURCC ('M', 'P', 'G', 'I'):
    case GST_MAKE_FOURCC ('m', 'p', 'g', '1'):
    case GST_MAKE_FOURCC ('M', 'P', 'G', '1'):
    case GST_MAKE_FOURCC ('P', 'I', 'M', '1'):
    case GST_MAKE_FOURCC (0x01, 0x00, 0x00, 0x10):
      caps = gst_caps_new_simple ("video/mpeg",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "mpegversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("MPEG-1 video");
      break;

    case GST_MAKE_FOURCC ('M', 'P', 'G', '2'):
    case GST_MAKE_FOURCC ('m', 'p', 'g', '2'):
    case GST_MAKE_FOURCC ('P', 'I', 'M', '2'):
    case GST_MAKE_FOURCC ('D', 'V', 'R', ' '):
    case GST_MAKE_FOURCC (0x02, 0x00, 0x00, 0x10):
      caps = gst_caps_new_simple ("video/mpeg",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "mpegversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("MPEG-2 video");
      break;

    case GST_MAKE_FOURCC ('L', 'M', 'P', '2'):
      caps = gst_caps_new_simple ("video/mpeg",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "mpegversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Lead MPEG-2 video");
      break;

    case GST_RIFF_H263:
    case GST_RIFF_h263:
    case GST_RIFF_i263:
    case GST_MAKE_FOURCC ('U', '2', '6', '3'):
    case GST_MAKE_FOURCC ('v', 'i', 'v', '1'):
    case GST_MAKE_FOURCC ('T', '2', '6', '3'):
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "itu", NULL);
      if (codec_name)
        *codec_name = g_strdup ("ITU H.26n");
      break;

    case GST_RIFF_L263:
      /* http://www.leadcodecs.com/Codecs/LEAD-H263.htm */
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "lead", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Lead H.263");
      break;

    case GST_RIFF_M263:
    case GST_RIFF_m263:
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "microsoft", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft H.263");
      break;

    case GST_RIFF_VDOW:
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "vdolive", NULL);
      if (codec_name)
        *codec_name = g_strdup ("VDOLive");
      break;

    case GST_MAKE_FOURCC ('V', 'I', 'V', 'O'):
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "vivo", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Vivo H.263");
      break;

    case GST_RIFF_x263:
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "xirlink", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Xirlink H.263");
      break;

      /* apparently not standard H.263...? */
    case GST_MAKE_FOURCC ('I', '2', '6', '3'):
      caps = gst_caps_new_simple ("video/x-intel-h263",
          "variant", G_TYPE_STRING, "intel", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Intel H.263");
      break;

    case GST_MAKE_FOURCC ('V', 'X', '1', 'K'):
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "lucent", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Lucent VX1000S H.263");
      break;

    case GST_MAKE_FOURCC ('X', '2', '6', '4'):
    case GST_MAKE_FOURCC ('x', '2', '6', '4'):
    case GST_MAKE_FOURCC ('H', '2', '6', '4'):
    case GST_MAKE_FOURCC ('h', '2', '6', '4'):
    case GST_MAKE_FOURCC ('a', 'v', 'c', '1'):
    case GST_MAKE_FOURCC ('A', 'V', 'C', '1'):
      caps = gst_caps_new_simple ("video/x-h264",
          "variant", G_TYPE_STRING, "itu", NULL);
      if (codec_name)
        *codec_name = g_strdup ("ITU H.264");
      break;

    case GST_RIFF_VSSH:
      caps = gst_caps_new_simple ("video/x-h264",
          "variant", G_TYPE_STRING, "videosoft", NULL);
      if (codec_name)
        *codec_name = g_strdup ("VideoSoft H.264");
      break;

    case GST_MAKE_FOURCC ('L', '2', '6', '4'):
      /* http://www.leadcodecs.com/Codecs/LEAD-H264.htm */
      caps = gst_caps_new_simple ("video/x-h264",
          "variant", G_TYPE_STRING, "lead", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Lead H.264");
      break;

    case GST_MAKE_FOURCC ('S', 'E', 'D', 'G'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Samsung MPEG-4");
      break;

    case GST_MAKE_FOURCC ('M', '4', 'C', 'C'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Divio MPEG-4");
      break;

    case GST_RIFF_DIV3:
    case GST_MAKE_FOURCC ('d', 'i', 'v', '3'):
    case GST_MAKE_FOURCC ('D', 'V', 'X', '3'):
    case GST_MAKE_FOURCC ('d', 'v', 'x', '3'):
    case GST_MAKE_FOURCC ('D', 'I', 'V', '4'):
    case GST_MAKE_FOURCC ('d', 'i', 'v', '4'):
    case GST_MAKE_FOURCC ('D', 'I', 'V', '5'):
    case GST_MAKE_FOURCC ('d', 'i', 'v', '5'):
    case GST_MAKE_FOURCC ('D', 'I', 'V', '6'):
    case GST_MAKE_FOURCC ('d', 'i', 'v', '6'):
    case GST_MAKE_FOURCC ('M', 'P', 'G', '3'):
    case GST_MAKE_FOURCC ('m', 'p', 'g', '3'):
    case GST_MAKE_FOURCC ('c', 'o', 'l', '0'):
    case GST_MAKE_FOURCC ('C', 'O', 'L', '0'):
    case GST_MAKE_FOURCC ('c', 'o', 'l', '1'):
    case GST_MAKE_FOURCC ('C', 'O', 'L', '1'):
    case GST_MAKE_FOURCC ('A', 'P', '4', '1'):
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 3, NULL);
      if (codec_name)
        *codec_name = g_strdup ("DivX MS-MPEG-4 Version 3");
      break;

    case GST_MAKE_FOURCC ('d', 'i', 'v', 'x'):
    case GST_MAKE_FOURCC ('D', 'I', 'V', 'X'):
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 4, NULL);
      if (codec_name)
        *codec_name = g_strdup ("DivX MPEG-4 Version 4");
      break;

    case GST_MAKE_FOURCC ('B', 'L', 'Z', '0'):
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 4, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Blizzard DivX");
      break;

    case GST_MAKE_FOURCC ('D', 'X', '5', '0'):
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 5, NULL);
      if (codec_name)
        *codec_name = g_strdup ("DivX MPEG-4 Version 5");
      break;

    case GST_MAKE_FOURCC ('M', 'P', 'G', '4'):
    case GST_MAKE_FOURCC ('M', 'P', '4', '1'):
    case GST_MAKE_FOURCC ('m', 'p', '4', '1'):
      caps = gst_caps_new_simple ("video/x-msmpeg",
          "msmpegversion", G_TYPE_INT, 41, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft MPEG-4 4.1");
      break;

    case GST_MAKE_FOURCC ('m', 'p', '4', '2'):
    case GST_MAKE_FOURCC ('M', 'P', '4', '2'):
      caps = gst_caps_new_simple ("video/x-msmpeg",
          "msmpegversion", G_TYPE_INT, 42, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft MPEG-4 4.2");
      break;

    case GST_MAKE_FOURCC ('m', 'p', '4', '3'):
    case GST_MAKE_FOURCC ('M', 'P', '4', '3'):
      caps = gst_caps_new_simple ("video/x-msmpeg",
          "msmpegversion", G_TYPE_INT, 43, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft MPEG-4 4.3");
      break;

    case GST_MAKE_FOURCC ('M', 'P', '4', 'S'):
    case GST_MAKE_FOURCC ('M', '4', 'S', '2'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft ISO MPEG-4 1.1");
      break;

    case GST_MAKE_FOURCC ('F', 'M', 'P', '4'):
    case GST_MAKE_FOURCC ('U', 'M', 'P', '4'):
    case GST_MAKE_FOURCC ('F', 'F', 'D', 'S'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      if (codec_name)
        *codec_name = g_strdup ("FFmpeg MPEG-4");
      break;

    case GST_MAKE_FOURCC ('3', 'I', 'V', '1'):
    case GST_MAKE_FOURCC ('3', 'I', 'V', '2'):
    case GST_MAKE_FOURCC ('X', 'V', 'I', 'D'):
    case GST_MAKE_FOURCC ('x', 'v', 'i', 'd'):
    case GST_MAKE_FOURCC ('E', 'M', '4', 'A'):
    case GST_MAKE_FOURCC ('E', 'P', 'V', 'H'):
    case GST_MAKE_FOURCC ('F', 'V', 'F', 'W'):
    case GST_MAKE_FOURCC ('I', 'N', 'M', 'C'):
    case GST_MAKE_FOURCC ('D', 'I', 'G', 'I'):
    case GST_MAKE_FOURCC ('D', 'M', '2', 'K'):
    case GST_MAKE_FOURCC ('D', 'C', 'O', 'D'):
    case GST_MAKE_FOURCC ('M', 'V', 'X', 'M'):
    case GST_MAKE_FOURCC ('P', 'M', '4', 'V'):
    case GST_MAKE_FOURCC ('S', 'M', 'P', '4'):
    case GST_MAKE_FOURCC ('D', 'X', 'G', 'M'):
    case GST_MAKE_FOURCC ('V', 'I', 'D', 'M'):
    case GST_MAKE_FOURCC ('M', '4', 'T', '3'):
    case GST_MAKE_FOURCC ('G', 'E', 'O', 'X'):
    case GST_MAKE_FOURCC ('M', 'P', '4', 'V'):
    case GST_MAKE_FOURCC ('m', 'p', '4', 'v'):
    case GST_MAKE_FOURCC ('R', 'M', 'P', '4'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      if (codec_name)
        *codec_name = g_strdup ("MPEG-4");
      break;

    case GST_MAKE_FOURCC ('3', 'i', 'v', 'd'):
    case GST_MAKE_FOURCC ('3', 'I', 'V', 'D'):
      caps = gst_caps_new_simple ("video/x-msmpeg",
          "msmpegversion", G_TYPE_INT, 43, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft MPEG-4 4.3");        /* FIXME? */
      break;

    case GST_MAKE_FOURCC ('D', 'V', 'S', 'D'):
    case GST_MAKE_FOURCC ('d', 'v', 's', 'd'):
    case GST_MAKE_FOURCC ('d', 'v', 'c', ' '):
    case GST_MAKE_FOURCC ('d', 'v', '2', '5'):
      caps = gst_caps_new_simple ("video/x-dv",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "dvversion", G_TYPE_INT, 25, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Generic DV");
      break;

    case GST_MAKE_FOURCC ('C', 'D', 'V', 'C'):
    case GST_MAKE_FOURCC ('c', 'd', 'v', 'c'):
      caps = gst_caps_new_simple ("video/x-dv",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "dvversion", G_TYPE_INT, 25, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Canopus DV");
      break;

    case GST_MAKE_FOURCC ('D', 'V', '5', '0'):
    case GST_MAKE_FOURCC ('d', 'v', '5', '0'):
      caps = gst_caps_new_simple ("video/x-dv",
          "systemstream", G_TYPE_BOOLEAN, FALSE,
          "dvversion", G_TYPE_INT, 50, NULL);
      if (codec_name)
        *codec_name = g_strdup ("DVCPro50 Video");
      break;

    case GST_MAKE_FOURCC ('M', 'S', 'S', '1'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 1, "format", G_TYPE_STRING, "MSS1", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media 7 Screen");
      break;

    case GST_MAKE_FOURCC ('M', 'S', 'S', '2'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 3, "format", G_TYPE_STRING, "MSS2", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media 9 Screen");
      break;

    case GST_MAKE_FOURCC ('W', 'M', 'V', '1'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media 7");
      break;

    case GST_MAKE_FOURCC ('W', 'M', 'V', '2'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media 8");
      break;

    case GST_MAKE_FOURCC ('W', 'M', 'V', '3'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 3, "format", G_TYPE_STRING, "WMV3", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media 9");
      break;

    case GST_MAKE_FOURCC ('W', 'M', 'V', 'A'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 3, "format", G_TYPE_STRING, "WMVA", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media Advanced Profile");
      break;

    case GST_MAKE_FOURCC ('W', 'V', 'C', '1'):
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 3, "format", G_TYPE_STRING, "WVC1", NULL);
      if (codec_name)
        *codec_name = g_strdup ("Microsoft Windows Media VC-1");
      break;

    case GST_RIFF_cvid:
    case GST_RIFF_CVID:
      caps = gst_caps_new_empty_simple ("video/x-cinepak");
      if (codec_name)
        *codec_name = g_strdup ("Cinepak video");
      break;

    case GST_RIFF_FCCH_MSVC:
    case GST_RIFF_FCCH_msvc:
    case GST_RIFF_CRAM:
    case GST_RIFF_cram:
    case GST_RIFF_WHAM:
    case GST_RIFF_wham:
      caps = gst_caps_new_simple ("video/x-msvideocodec",
          "msvideoversion", G_TYPE_INT, 1, NULL);
      if (strf) {
        gst_caps_set_simple (caps, "bpp",
            G_TYPE_INT, (int) strf->bit_cnt, NULL);
      }
      if (codec_name)
        *codec_name = g_strdup ("MS video v1");
      palette = strf_data;
      strf_data = NULL;
      break;

    case GST_RIFF_FCCH_RLE:
    case GST_MAKE_FOURCC ('m', 'r', 'l', 'e'):
    case GST_MAKE_FOURCC (0x1, 0x0, 0x0, 0x0): /* why, why, why? */
    case GST_MAKE_FOURCC (0x2, 0x0, 0x0, 0x0): /* why, why, why? */
      caps = gst_caps_new_simple ("video/x-rle",
          "layout", G_TYPE_STRING, "microsoft", NULL);
      palette = strf_data;
      strf_data = NULL;
      if (strf) {
        gst_caps_set_simple (caps,
            "depth", G_TYPE_INT, (gint) strf->bit_cnt, NULL);
      } else {
        gst_caps_set_simple (caps, "depth", GST_TYPE_INT_RANGE, 1, 64, NULL);
      }
      if (codec_name)
        *codec_name = g_strdup ("Microsoft RLE");
      break;

    case GST_MAKE_FOURCC ('A', 'A', 'S', 'C'):
      caps = gst_caps_new_empty_simple ("video/x-aasc");
      if (codec_name)
        *codec_name = g_strdup ("Autodesk Animator");
      break;

    case GST_MAKE_FOURCC ('X', 'x', 'a', 'n'):
      caps = gst_caps_new_simple ("video/x-xan",
          "wcversion", G_TYPE_INT, 4, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Xan Wing Commander 4");
      break;

    case GST_RIFF_RT21:
    case GST_RIFF_rt21:
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Intel Video 2");
      break;

    case GST_RIFF_IV31:
    case GST_RIFF_IV32:
    case GST_RIFF_iv31:
    case GST_RIFF_iv32:
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 3, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Intel Video 3");
      break;

    case GST_RIFF_IV41:
    case GST_RIFF_iv41:
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 4, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Intel Video 4");
      break;

    case GST_RIFF_IV50:
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 5, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Intel Video 5");
      break;

    case GST_MAKE_FOURCC ('M', 'S', 'Z', 'H'):
      caps = gst_caps_new_empty_simple ("video/x-mszh");
      if (codec_name)
        *codec_name = g_strdup ("Lossless MSZH Video");
      break;

    case GST_MAKE_FOURCC ('Z', 'L', 'I', 'B'):
      caps = gst_caps_new_empty_simple ("video/x-zlib");
      if (codec_name)
        *codec_name = g_strdup ("Lossless zlib video");
      break;

    case GST_MAKE_FOURCC ('C', 'L', 'J', 'R'):
    case GST_MAKE_FOURCC ('c', 'l', 'j', 'r'):
      caps = gst_caps_new_empty_simple ("video/x-cirrus-logic-accupak");
      if (codec_name)
        *codec_name = g_strdup ("Cirrus Logipak AccuPak");
      break;

    case GST_RIFF_CYUV:
    case GST_RIFF_cyuv:
      caps = gst_caps_new_empty_simple ("video/x-compressed-yuv");
      if (codec_name)
        *codec_name = g_strdup ("CYUV Lossless");
      break;

    case GST_MAKE_FOURCC ('D', 'U', 'C', 'K'):
    case GST_MAKE_FOURCC ('P', 'V', 'E', 'Z'):
      caps = gst_caps_new_simple ("video/x-truemotion",
          "trueversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Duck Truemotion1");
      break;

    case GST_MAKE_FOURCC ('T', 'M', '2', '0'):
      caps = gst_caps_new_simple ("video/x-truemotion",
          "trueversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("TrueMotion 2.0");
      break;

    case GST_MAKE_FOURCC ('V', 'P', '3', '0'):
    case GST_MAKE_FOURCC ('v', 'p', '3', '0'):
    case GST_MAKE_FOURCC ('V', 'P', '3', '1'):
    case GST_MAKE_FOURCC ('v', 'p', '3', '1'):
    case GST_MAKE_FOURCC ('V', 'P', '3', ' '):
      caps = gst_caps_new_empty_simple ("video/x-vp3");
      if (codec_name)
        *codec_name = g_strdup ("VP3");
      break;

    case GST_RIFF_ULTI:
    case GST_RIFF_ulti:
      caps = gst_caps_new_empty_simple ("video/x-ultimotion");
      if (codec_name)
        *codec_name = g_strdup ("IBM UltiMotion");
      break;

      /* FIXME 2.0: Rename video/x-camtasia to video/x-tscc,version=1 */
    case GST_MAKE_FOURCC ('T', 'S', 'C', 'C'):
    case GST_MAKE_FOURCC ('t', 's', 'c', 'c'):{
      if (strf) {
        gint depth = (strf->bit_cnt != 0) ? (gint) strf->bit_cnt : 24;

        caps = gst_caps_new_simple ("video/x-camtasia", "depth", G_TYPE_INT,
            depth, NULL);
      } else {
        /* template caps */
        caps = gst_caps_new_empty_simple ("video/x-camtasia");
      }
      if (codec_name)
        *codec_name = g_strdup ("TechSmith Camtasia");
      break;
    }

    case GST_MAKE_FOURCC ('T', 'S', 'C', '2'):
    case GST_MAKE_FOURCC ('t', 's', 'c', '2'):{
      caps =
          gst_caps_new_simple ("video/x-tscc", "tsccversion", G_TYPE_INT, 2,
          NULL);
      if (codec_name)
        *codec_name = g_strdup ("TechSmith Screen Capture 2");
      break;
    }

    case GST_MAKE_FOURCC ('C', 'S', 'C', 'D'):
    {
      if (strf) {
        gint depth = (strf->bit_cnt != 0) ? (gint) strf->bit_cnt : 24;

        caps = gst_caps_new_simple ("video/x-camstudio", "depth", G_TYPE_INT,
            depth, NULL);
      } else {
        /* template caps */
        caps = gst_caps_new_empty_simple ("video/x-camstudio");
      }
      if (codec_name)
        *codec_name = g_strdup ("Camstudio");
      break;
    }

    case GST_MAKE_FOURCC ('V', 'C', 'R', '1'):
      caps = gst_caps_new_simple ("video/x-ati-vcr",
          "vcrversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("ATI VCR 1");
      break;

    case GST_MAKE_FOURCC ('V', 'C', 'R', '2'):
      caps = gst_caps_new_simple ("video/x-ati-vcr",
          "vcrversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("ATI VCR 2");
      break;

    case GST_MAKE_FOURCC ('A', 'S', 'V', '1'):
      caps = gst_caps_new_simple ("video/x-asus",
          "asusversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Asus Video 1");
      break;

    case GST_MAKE_FOURCC ('A', 'S', 'V', '2'):
      caps = gst_caps_new_simple ("video/x-asus",
          "asusversion", G_TYPE_INT, 2, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Asus Video 2");
      break;

    case GST_MAKE_FOURCC ('M', 'P', 'N', 'G'):
    case GST_MAKE_FOURCC ('m', 'p', 'n', 'g'):
    case GST_MAKE_FOURCC ('P', 'N', 'G', ' '):
    case GST_MAKE_FOURCC ('p', 'n', 'g', ' '):
      caps = gst_caps_new_empty_simple ("image/png");
      if (codec_name)
        *codec_name = g_strdup ("PNG image");
      break;

    case GST_MAKE_FOURCC ('F', 'L', 'V', '1'):
      caps = gst_caps_new_simple ("video/x-flash-video",
          "flvversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("Flash Video 1");
      break;

    case GST_MAKE_FOURCC ('V', 'M', 'n', 'c'):
      caps = gst_caps_new_simple ("video/x-vmnc",
          "version", G_TYPE_INT, 1, NULL);
      if (strf && strf->bit_cnt != 0)
        gst_caps_set_simple (caps, "bpp", G_TYPE_INT, strf->bit_cnt, NULL);
      if (codec_name)
        *codec_name = g_strdup ("VMWare NC Video");
      break;

    case GST_MAKE_FOURCC ('d', 'r', 'a', 'c'):
      caps = gst_caps_new_empty_simple ("video/x-dirac");
      if (codec_name)
        *codec_name = g_strdup ("Dirac");
      break;

    case GST_RIFF_rpza:
    case GST_RIFF_azpr:
    case GST_MAKE_FOURCC ('R', 'P', 'Z', 'A'):
      caps = gst_caps_new_empty_simple ("video/x-apple-video");
      if (codec_name)
        *codec_name = g_strdup ("Apple Video (RPZA)");
      break;


    case GST_MAKE_FOURCC ('F', 'F', 'V', '1'):
      caps = gst_caps_new_simple ("video/x-ffv",
          "ffvversion", G_TYPE_INT, 1, NULL);
      if (codec_name)
        *codec_name = g_strdup ("FFmpeg lossless video codec");
      break;

    case GST_MAKE_FOURCC ('K', 'M', 'V', 'C'):
      caps = gst_caps_new_empty_simple ("video/x-kmvc");
      if (codec_name)
        *codec_name = g_strdup ("Karl Morton's video codec");
      break;

    case GST_MAKE_FOURCC ('v', 'p', '5', '0'):
    case GST_MAKE_FOURCC ('V', 'P', '5', '0'):
      caps = gst_caps_new_empty_simple ("video/x-vp5");
      if (codec_name)
        *codec_name = g_strdup ("On2 VP5");
      break;

    case GST_MAKE_FOURCC ('v', 'p', '6', '0'):
    case GST_MAKE_FOURCC ('V', 'P', '6', '0'):
    case GST_MAKE_FOURCC ('v', 'p', '6', '1'):
    case GST_MAKE_FOURCC ('V', 'P', '6', '1'):
    case GST_MAKE_FOURCC ('V', 'p', '6', '2'):
    case GST_MAKE_FOURCC ('V', 'P', '6', '2'):
      caps = gst_caps_new_empty_simple ("video/x-vp6");
      if (codec_name)
        *codec_name = g_strdup ("On2 VP6");
      break;

    case GST_MAKE_FOURCC ('V', 'P', '6', 'F'):
    case GST_MAKE_FOURCC ('v', 'p', '6', 'f'):
    case GST_MAKE_FOURCC ('F', 'L', 'V', '4'):
      caps = gst_caps_new_empty_simple ("video/x-vp6-flash");
      if (codec_name)
        *codec_name = g_strdup ("On2 VP6");
      break;

    case GST_MAKE_FOURCC ('v', 'p', '7', '0'):
    case GST_MAKE_FOURCC ('V', 'P', '7', '0'):
      caps = gst_caps_new_empty_simple ("video/x-vp7");
      if (codec_name)
        *codec_name = g_strdup ("On2 VP7");
      break;

    case GST_MAKE_FOURCC ('V', 'P', '8', '0'):
      caps = gst_caps_new_empty_simple ("video/x-vp8");
      if (codec_name)
        *codec_name = g_strdup ("On2 VP8");
      break;

    case GST_MAKE_FOURCC ('L', 'M', '2', '0'):
      caps = gst_caps_new_empty_simple ("video/x-mimic");
      if (codec_name)
        *codec_name = g_strdup ("Mimic webcam");
      break;

    case GST_MAKE_FOURCC ('T', 'H', 'E', 'O'):
    case GST_MAKE_FOURCC ('t', 'h', 'e', 'o'):
      caps = gst_caps_new_empty_simple ("video/x-theora");
      if (codec_name)
        *codec_name = g_strdup ("Theora video codec");

      break;

    case GST_MAKE_FOURCC ('F', 'P', 'S', '1'):
      caps = gst_caps_new_empty_simple ("video/x-fraps");
      if (codec_name)
        *codec_name = g_strdup ("Fraps video");

      break;

    case GST_MAKE_FOURCC ('D', 'X', 'S', 'B'):
    case GST_MAKE_FOURCC ('D', 'X', 'S', 'A'):
      caps = gst_caps_new_empty_simple ("subpicture/x-xsub");
      if (codec_name)
        *codec_name = g_strdup ("XSUB subpicture stream");

      break;

    default:
      GST_WARNING ("Unknown video fourcc %" GST_FOURCC_FORMAT,
          GST_FOURCC_ARGS (codec_fcc));
      return NULL;
  }

  if (strh != NULL) {
    gst_caps_set_simple (caps, "framerate", GST_TYPE_FRACTION,
        strh->rate, strh->scale, NULL);
  } else {
    gst_caps_set_simple (caps,
        "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, G_MAXINT, 1, NULL);
  }

  if (strf != NULL) {
    /* raw rgb data is stored topdown, but instead of inverting the buffer, */
    /* some tools just negate the height field in the header (e.g. ffmpeg) */
    gst_caps_set_simple (caps,
        "width", G_TYPE_INT, strf->width,
        "height", G_TYPE_INT, ABS ((gint) strf->height), NULL);
  } else {
    gst_caps_set_simple (caps,
        "width", GST_TYPE_INT_RANGE, 1, G_MAXINT,
        "height", GST_TYPE_INT_RANGE, 1, G_MAXINT, NULL);
  }

  /* extradata */
  if (strf_data || strd_data) {
    GstBuffer *codec_data;

    codec_data = strf_data ? strf_data : strd_data;

    gst_caps_set_simple (caps, "codec_data", GST_TYPE_BUFFER, codec_data, NULL);
  }

  /* palette */
  if (palette) {
    GstBuffer *copy;
    guint num_colors;
    gsize size;

    if (strf != NULL)
      num_colors = strf->num_colors;
    else
      num_colors = 256;

    size = gst_buffer_get_size (palette);

    if (size >= (num_colors * 4)) {
      guint8 *pdata;

      /* palette is always at least 256*4 bytes */
      pdata = g_malloc0 (MAX (size, 256 * 4));
      gst_buffer_extract (palette, 0, pdata, size);

      if (G_BYTE_ORDER == G_BIG_ENDIAN) {
        guint8 *p = pdata;
        gint n;

        /* own endianness */
        for (n = 0; n < num_colors; n++) {
          GST_WRITE_UINT32_BE (p, GST_READ_UINT32_LE (p));
          p += sizeof (guint32);
        }
      }

      copy = gst_buffer_new_wrapped (pdata, size);
      gst_caps_set_simple (caps, "palette_data", GST_TYPE_BUFFER, copy, NULL);
      gst_buffer_unref (copy);
    } else {
      GST_WARNING ("Palette smaller than expected: broken file");
    }
  }

  return caps;
}