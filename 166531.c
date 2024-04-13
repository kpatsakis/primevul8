gst_riff_create_audio_template_caps (void)
{
  static const guint16 tags[] = {
    GST_RIFF_WAVE_FORMAT_GSM610,
    GST_RIFF_WAVE_FORMAT_MPEGL3,
    GST_RIFF_WAVE_FORMAT_MPEGL12,
    GST_RIFF_WAVE_FORMAT_PCM,
    GST_RIFF_WAVE_FORMAT_VORBIS1,
    GST_RIFF_WAVE_FORMAT_A52,
    GST_RIFF_WAVE_FORMAT_DTS,
    GST_RIFF_WAVE_FORMAT_AAC,
    GST_RIFF_WAVE_FORMAT_ALAW,
    GST_RIFF_WAVE_FORMAT_MULAW,
    GST_RIFF_WAVE_FORMAT_WMS,
    GST_RIFF_WAVE_FORMAT_ADPCM,
    GST_RIFF_WAVE_FORMAT_DVI_ADPCM,
    GST_RIFF_WAVE_FORMAT_DSP_TRUESPEECH,
    GST_RIFF_WAVE_FORMAT_WMAV1,
    GST_RIFF_WAVE_FORMAT_WMAV2,
    GST_RIFF_WAVE_FORMAT_WMAV3,
    GST_RIFF_WAVE_FORMAT_SONY_ATRAC3,
    GST_RIFF_WAVE_FORMAT_IEEE_FLOAT,
    GST_RIFF_WAVE_FORMAT_VOXWARE_METASOUND,
    GST_RIFF_WAVE_FORMAT_ADPCM_IMA_DK4,
    GST_RIFF_WAVE_FORMAT_ADPCM_IMA_DK3,
    GST_RIFF_WAVE_FORMAT_ADPCM_IMA_WAV,
    GST_RIFF_WAVE_FORMAT_AMR_NB,
    GST_RIFF_WAVE_FORMAT_AMR_WB,
    GST_RIFF_WAVE_FORMAT_SIREN,
    /* FILL ME */
  };
  guint i;
  GstCaps *caps, *one;

  caps = gst_caps_new_empty ();
  for (i = 0; i < G_N_ELEMENTS (tags); i++) {
    one =
        gst_riff_create_audio_caps (tags[i], NULL, NULL, NULL, NULL, NULL,
        NULL);
    if (one)
      gst_caps_append (caps, one);
  }
  one = gst_caps_new_empty_simple ("application/x-ogg-avi");
  gst_caps_append (caps, one);

  return caps;
}