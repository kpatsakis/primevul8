plugin_init (GstPlugin * plugin)
{
  /* can't initialize this via a struct as caps can't be statically initialized */

  GST_DEBUG_CATEGORY_INIT (type_find_debug, "typefindfunctions",
      GST_DEBUG_FG_GREEN | GST_DEBUG_BG_RED, "generic type find functions");

  /* note: asx/wax/wmx are XML files, asf doesn't handle them */
  /* must use strings, macros don't accept initializers */
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-ms-asf", GST_RANK_SECONDARY,
      "asf,wm,wma,wmv",
      "\060\046\262\165\216\146\317\021\246\331\000\252\000\142\316\154", 16,
      GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/x-musepack", GST_RANK_PRIMARY,
      musepack_type_find, "mpc,mpp,mp+", MUSEPACK_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-au", GST_RANK_MARGINAL,
      au_type_find, "au,snd", AU_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_RIFF (plugin, "video/x-msvideo", GST_RANK_PRIMARY,
      "avi", "AVI ");
  TYPE_FIND_REGISTER_RIFF (plugin, "audio/qcelp", GST_RANK_PRIMARY,
      "qcp", "QLCM");
  TYPE_FIND_REGISTER_RIFF (plugin, "video/x-cdxa", GST_RANK_PRIMARY,
      "dat", "CDXA");
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-vcd", GST_RANK_PRIMARY,
      "dat", "\000\377\377\377\377\377\377\377\377\377\377\000", 12,
      GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-imelody", GST_RANK_PRIMARY,
      "imy,ime,imelody", "BEGIN:IMELODY", 13, GST_TYPE_FIND_MAXIMUM);
#if 0
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-smoke", GST_RANK_PRIMARY,
      NULL, "\x80smoke\x00\x01\x00", 6, GST_TYPE_FIND_MAXIMUM);
#endif
  TYPE_FIND_REGISTER (plugin, "audio/midi", GST_RANK_PRIMARY, mid_type_find,
      "mid,midi", MID_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_RIFF (plugin, "audio/riff-midi", GST_RANK_PRIMARY,
      "mid,midi", "RMID");
  TYPE_FIND_REGISTER (plugin, "audio/mobile-xmf", GST_RANK_PRIMARY,
      mxmf_type_find, "mxmf", MXMF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-fli", GST_RANK_MARGINAL, flx_type_find,
      "flc,fli", FLX_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-id3v2", GST_RANK_PRIMARY + 103,
      id3v2_type_find, "mp3,mp2,mp1,mpga,ogg,flac,tta", ID3_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-id3v1", GST_RANK_PRIMARY + 101,
      id3v1_type_find, "mp3,mp2,mp1,mpga,ogg,flac,tta", ID3_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-apetag", GST_RANK_PRIMARY + 102,
      apetag_type_find, "mp3,ape,mpc,wv", APETAG_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-ttafile", GST_RANK_PRIMARY,
      tta_type_find, "tta", TTA_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-mod", GST_RANK_SECONDARY, mod_type_find,
      "669,amf,ams,dbm,digi,dmf,dsm,gdm,far,imf,it,j2b,mdl,med,mod,mt2,mtm,"
      "okt,psm,ptm,sam,s3m,stm,stx,ult,umx,xm", MOD_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/mpeg", GST_RANK_PRIMARY, mp3_type_find,
      "mp3,mp2,mp1,mpga", MP3_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-ac3", GST_RANK_PRIMARY, ac3_type_find,
      "ac3,eac3", AC3_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-dts", GST_RANK_SECONDARY, dts_type_find,
      "dts", DTS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-gsm", GST_RANK_PRIMARY, NULL, "gsm",
      GSM_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/mpeg-sys", GST_RANK_PRIMARY,
      mpeg_sys_type_find, "mpe,mpeg,mpg", MPEG_SYS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/mpegts", GST_RANK_PRIMARY,
      mpeg_ts_type_find, "ts,mts", MPEGTS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/ogg", GST_RANK_PRIMARY,
      ogganx_type_find, "ogg,oga,ogv,ogm,ogx,spx,anx,axa,axv", OGG_CAPS,
      NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/mpeg-elementary", GST_RANK_MARGINAL,
      mpeg_video_stream_type_find, "mpv,mpeg,mpg", MPEG_VIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/mpeg4", GST_RANK_PRIMARY,
      mpeg4_video_type_find, "m4v", MPEG_VIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-h263", GST_RANK_SECONDARY,
      h263_video_type_find, "h263,263", H263_VIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-h264", GST_RANK_PRIMARY,
      h264_video_type_find, "h264,x264,264", H264_VIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-h265", GST_RANK_PRIMARY,
      h265_video_type_find, "h265,x265,265", H265_VIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-nuv", GST_RANK_SECONDARY, nuv_type_find,
      "nuv", NUV_CAPS, NULL, NULL);

  /* ISO formats */
  TYPE_FIND_REGISTER (plugin, "audio/x-m4a", GST_RANK_PRIMARY, m4a_type_find,
      "m4a", M4A_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-3gp", GST_RANK_PRIMARY,
      q3gp_type_find, "3gp", Q3GP_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/quicktime", GST_RANK_PRIMARY,
      qt_type_find, "mov,mp4", QT_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/x-quicktime", GST_RANK_SECONDARY,
      qtif_type_find, "qif,qtif,qti", QTIF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/jp2", GST_RANK_PRIMARY,
      jp2_type_find, "jp2", JP2_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/mj2", GST_RANK_PRIMARY,
      jp2_type_find, "mj2", MJ2_CAPS, NULL, NULL);

  TYPE_FIND_REGISTER (plugin, "text/html", GST_RANK_SECONDARY, html_type_find,
      "htm,html", HTML_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/vnd.rn-realmedia",
      GST_RANK_SECONDARY, "ra,ram,rm,rmvb", ".RMF", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-pn-realaudio",
      GST_RANK_SECONDARY, "ra,ram,rm,rmvb", ".ra\375", 4,
      GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "application/x-shockwave-flash",
      GST_RANK_SECONDARY, swf_type_find, "swf,swfl", SWF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/dash+xml",
      GST_RANK_PRIMARY, dash_mpd_type_find, "mpd,MPD", DASH_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/vnd.ms-sstr+xml",
      GST_RANK_PRIMARY, mss_manifest_type_find, NULL, MSS_MANIFEST_CAPS, NULL,
      NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-flv", GST_RANK_SECONDARY,
      "flv", "FLV", 3, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "text/plain", GST_RANK_MARGINAL, utf8_type_find,
      "txt", UTF8_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "text/utf-16", GST_RANK_MARGINAL, utf16_type_find,
      "txt", UTF16_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "text/utf-32", GST_RANK_MARGINAL, utf32_type_find,
      "txt", UTF32_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "text/uri-list", GST_RANK_MARGINAL, uri_type_find,
      "ram", URI_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/itc", GST_RANK_SECONDARY,
      itc_type_find, "itc", ITC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-hls", GST_RANK_MARGINAL,
      hls_type_find, "m3u8", HLS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/sdp", GST_RANK_SECONDARY,
      sdp_type_find, "sdp", SDP_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/smil", GST_RANK_SECONDARY,
      smil_type_find, "smil", SMIL_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/ttml+xml", GST_RANK_SECONDARY,
      ttml_xml_type_find, "ttml+xml", TTML_XML_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/xml", GST_RANK_MARGINAL,
      xml_type_find, "xml", GENERIC_XML_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_RIFF (plugin, "audio/x-wav", GST_RANK_PRIMARY, "wav",
      "WAVE");
  TYPE_FIND_REGISTER (plugin, "audio/x-aiff", GST_RANK_SECONDARY,
      aiff_type_find, "aiff,aif,aifc", AIFF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-svx", GST_RANK_SECONDARY, svx_type_find,
      "iff,svx", SVX_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-paris", GST_RANK_SECONDARY,
      paris_type_find, "paf", PARIS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-nist", GST_RANK_SECONDARY,
      "nist", "NIST", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-voc", GST_RANK_SECONDARY,
      "voc", "Creative", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/x-sds", GST_RANK_SECONDARY, sds_type_find,
      "sds", SDS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-ircam", GST_RANK_SECONDARY,
      ircam_type_find, "sf", IRCAM_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-w64", GST_RANK_SECONDARY,
      "w64", "riff", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-rf64", GST_RANK_PRIMARY,
      "rf64", "RF64", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/x-shorten", GST_RANK_SECONDARY,
      shn_type_find, "shn", SHN_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ape", GST_RANK_SECONDARY,
      ape_type_find, "ape", APE_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/jpeg", GST_RANK_PRIMARY + 15,
      jpeg_type_find, "jpg,jpe,jpeg", JPEG_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/gif", GST_RANK_PRIMARY, "gif",
      "GIF8", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/png", GST_RANK_PRIMARY + 14,
      "png", "\211PNG\015\012\032\012", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "image/bmp", GST_RANK_PRIMARY, bmp_type_find,
      "bmp", BMP_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/tiff", GST_RANK_PRIMARY, tiff_type_find,
      "tif,tiff", TIFF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_RIFF (plugin, "image/webp", GST_RANK_PRIMARY,
      "webp", "WEBP");
  TYPE_FIND_REGISTER (plugin, "image/x-exr", GST_RANK_PRIMARY, exr_type_find,
      "exr", EXR_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/x-portable-pixmap", GST_RANK_SECONDARY,
      pnm_type_find, "pnm,ppm,pgm,pbm", PNM_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-matroska", GST_RANK_PRIMARY,
      matroska_type_find, "mkv,mka,mk3d,webm", MATROSKA_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/mxf", GST_RANK_PRIMARY,
      mxf_type_find, "mxf", MXF_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-mve", GST_RANK_SECONDARY,
      "mve", "Interplay MVE File\032\000\032\000\000\001\063\021", 26,
      GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "video/x-dv", GST_RANK_SECONDARY, dv_type_find,
      "dv,dif", DV_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-amr-nb-sh", GST_RANK_PRIMARY,
      "amr", "#!AMR", 5, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-amr-wb-sh", GST_RANK_PRIMARY,
      "amr", "#!AMR-WB", 7, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/iLBC-sh", GST_RANK_PRIMARY, ilbc_type_find,
      "ilbc", ILBC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-sbc", GST_RANK_MARGINAL, sbc_type_find,
      "sbc", SBC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-sid", GST_RANK_MARGINAL,
      "sid", "PSID", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/x-xcf", GST_RANK_SECONDARY,
      "xcf", "gimp xcf", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-mng", GST_RANK_SECONDARY,
      "mng", "\212MNG\015\012\032\012", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/x-jng", GST_RANK_SECONDARY,
      "jng", "\213JNG\015\012\032\012", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/x-xpixmap", GST_RANK_SECONDARY,
      "xpm", "/* XPM */", 9, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/x-sun-raster",
      GST_RANK_SECONDARY, "ras", "\131\246\152\225", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-bzip",
      GST_RANK_SECONDARY, "bz2", "BZh", 3, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-gzip",
      GST_RANK_SECONDARY, "gz", "\037\213", 2, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/zip", GST_RANK_SECONDARY,
      "zip", "PK\003\004", 4, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-compress",
      GST_RANK_SECONDARY, "Z", "\037\235", 2, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER (plugin, "subtitle/x-kate", GST_RANK_MARGINAL,
      kate_type_find, NULL, NULL, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-subtitle-vtt", GST_RANK_MARGINAL,
      webvtt_type_find, "vtt", WEBVTT_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-flac", GST_RANK_PRIMARY, flac_type_find,
      "flac", FLAC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-vorbis", GST_RANK_PRIMARY,
      vorbis_type_find, NULL, VORBIS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-theora", GST_RANK_PRIMARY,
      theora_type_find, NULL, THEORA_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ogm-video", GST_RANK_PRIMARY,
      ogmvideo_type_find, NULL, OGMVIDEO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ogm-audio", GST_RANK_PRIMARY,
      ogmaudio_type_find, NULL, OGMAUDIO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ogm-text", GST_RANK_PRIMARY,
      ogmtext_type_find, NULL, OGMTEXT_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-speex", GST_RANK_PRIMARY,
      speex_type_find, NULL, SPEEX_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-celt", GST_RANK_PRIMARY, celt_type_find,
      NULL, CELT_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ogg-skeleton", GST_RANK_PRIMARY,
      oggskel_type_find, NULL, OGG_SKELETON_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "text/x-cmml", GST_RANK_PRIMARY, cmml_type_find,
      NULL, CMML_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-executable",
      GST_RANK_MARGINAL, NULL, "\177ELF", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/aac", GST_RANK_SECONDARY, aac_type_find,
      "aac,adts,adif,loas", AAC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-spc", GST_RANK_SECONDARY,
      "spc", "SNES-SPC700 Sound File Data", 27, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "audio/x-wavpack", GST_RANK_SECONDARY,
      wavpack_type_find, "wv,wvp", WAVPACK_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "audio/x-wavpack-correction", GST_RANK_SECONDARY,
      wavpack_type_find, "wvc", WAVPACK_CORRECTION_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-caf", GST_RANK_SECONDARY,
      "caf", "caff\000\001", 6, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER (plugin, "application/postscript", GST_RANK_SECONDARY,
      postscript_type_find, "ps", POSTSCRIPT_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/svg+xml", GST_RANK_SECONDARY,
      svg_type_find, "svg", SVG_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/x-rar",
      GST_RANK_SECONDARY, "rar", "Rar!", 4, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER (plugin, "application/x-tar", GST_RANK_SECONDARY,
      tar_type_find, "tar", TAR_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ar", GST_RANK_SECONDARY,
      ar_type_find, "a", AR_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-ms-dos-executable",
      GST_RANK_SECONDARY, msdos_type_find, "dll,exe,ocx,sys,scr,msstyles,cpl",
      MSDOS_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/x-dirac", GST_RANK_PRIMARY,
      dirac_type_find, NULL, DIRAC_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "multipart/x-mixed-replace", GST_RANK_SECONDARY,
      multipart_type_find, NULL, MULTIPART_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-mmsh", GST_RANK_SECONDARY,
      mmsh_type_find, NULL, MMSH_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "video/vivo", GST_RANK_SECONDARY, vivo_type_find,
      "viv", VIVO_CAPS, NULL, NULL);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-nsf", GST_RANK_SECONDARY,
      "nsf", "NESM\x1a", 5, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-gym", GST_RANK_SECONDARY,
      "gym", "GYMX", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-ay", GST_RANK_SECONDARY, "ay",
      "ZXAYEMUL", 8, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-gbs", GST_RANK_SECONDARY,
      "gbs", "GBS\x01", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-vgm", GST_RANK_SECONDARY,
      "vgm", "Vgm\x20", 4, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-sap", GST_RANK_SECONDARY,
      "sap", "SAP\x0d\x0a" "AUTHOR\x20", 12, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "video/x-ivf", GST_RANK_SECONDARY,
      "ivf", "DKIF", 4, GST_TYPE_FIND_NEARLY_CERTAIN);
  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-kss", GST_RANK_SECONDARY,
      "kss", "KSSX\0", 5, GST_TYPE_FIND_MAXIMUM);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/pdf", GST_RANK_SECONDARY,
      "pdf", "%PDF-", 5, GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/msword",
      GST_RANK_SECONDARY, "doc", "\320\317\021\340\241\261\032\341", 8,
      GST_TYPE_FIND_LIKELY);
  /* Mac OS X .DS_Store files tend to be taken for video/mpeg */
  TYPE_FIND_REGISTER_START_WITH (plugin, "application/octet-stream",
      GST_RANK_SECONDARY, "DS_Store", "\000\000\000\001Bud1", 8,
      GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER_START_WITH (plugin, "image/vnd.adobe.photoshop",
      GST_RANK_SECONDARY, "psd", "8BPS\000\001\000\000\000\000", 10,
      GST_TYPE_FIND_LIKELY);
  TYPE_FIND_REGISTER (plugin, "image/vnd.wap.wbmp", GST_RANK_MARGINAL,
      wbmp_typefind, NULL, NULL, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/x-yuv4mpeg", GST_RANK_SECONDARY,
      y4m_typefind, NULL, NULL, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "image/x-icon", GST_RANK_MARGINAL,
      windows_icon_typefind, NULL, NULL, NULL, NULL);

#ifdef USE_GIO
  TYPE_FIND_REGISTER (plugin, "xdgmime-base", GST_RANK_MARGINAL,
      xdgmime_typefind, NULL, NULL, NULL, NULL);
#endif

  TYPE_FIND_REGISTER (plugin, "image/x-degas", GST_RANK_MARGINAL,
      degas_type_find, NULL, NULL, NULL, NULL);
  TYPE_FIND_REGISTER (plugin, "application/octet-stream", GST_RANK_MARGINAL,
      dvdiso_type_find, NULL, NULL, NULL, NULL);

  TYPE_FIND_REGISTER (plugin, "application/x-ssa", GST_RANK_SECONDARY,
      ssa_type_find, "ssa,ass", NULL, NULL, NULL);

  TYPE_FIND_REGISTER (plugin, "video/x-pva", GST_RANK_SECONDARY,
      pva_type_find, "pva", PVA_CAPS, NULL, NULL);

  TYPE_FIND_REGISTER_START_WITH (plugin, "audio/x-xi", GST_RANK_SECONDARY,
      "xi", "Extended Instrument: ", 21, GST_TYPE_FIND_MAXIMUM);

  TYPE_FIND_REGISTER (plugin, "audio/audible", GST_RANK_MARGINAL,
      aa_type_find, "aa,aax", AA_CAPS, NULL, NULL);

  return TRUE;
}