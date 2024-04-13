mod_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;
  GstTypeFindProbability probability;
  const char *mod_type = NULL;

  /* MOD */
  if ((data = gst_type_find_peek (tf, 1080, 4)) != NULL) {
    /* Protracker and variants */
    if ((memcmp (data, "M.K.", 4) == 0) ||
        (memcmp (data, "M!K!", 4) == 0) ||
        (memcmp (data, "M&K!", 4) == 0) || (memcmp (data, "N.T.", 4) == 0) ||
        /* Star Tracker */
        (memcmp (data, "FLT", 3) == 0 && isdigit (data[3])) ||
        (memcmp (data, "EXO", 3) == 0 && isdigit (data[3])) ||
        /* Oktalyzer (Amiga) */
        (memcmp (data, "OKTA", 4) == 0) || (memcmp (data, "OCTA", 4) == 0) ||
        /* Oktalyser (Atari) */
        (memcmp (data, "CD81", 4) == 0) ||
        /* Taketracker */
        (memcmp (data, "TDZ", 3) == 0 && isdigit (data[3])) ||
        /* Fasttracker */
        (memcmp (data + 1, "CHN", 3) == 0 && isdigit (data[0])) ||
        /* Fasttracker or Taketracker */
        (memcmp (data + 2, "CH", 2) == 0 && isdigit (data[0])
            && isdigit (data[1])) || (memcmp (data + 2, "CN", 2) == 0
            && isdigit (data[0]) && isdigit (data[1]))) {
      mod_type = "mod";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* J2B (Jazz Jackrabbit 2) */
  if ((data = gst_type_find_peek (tf, 0, 8)) != NULL) {
    if ((memcmp (data, "MUSE\xDE\xAD", 4) == 0) &&
        ((memcmp (data + 6, "\xBE\xEF", 2) == 0) ||
            (memcmp (data + 6, "\xBA\xBE", 2) == 0))) {
      mod_type = "j2b";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* AMS (Velvet Studio) */
  if ((data = gst_type_find_peek (tf, 0, 7)) != NULL) {
    if (memcmp (data, "AMShdr\x1A", 7) == 0) {
      mod_type = "velvet-ams";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* AMS (Extreme Tracker) */
  if ((data = gst_type_find_peek (tf, 0, 9)) != NULL) {
    if ((memcmp (data, "Extreme", 7) == 0) && (data[8] == 1)) {
      mod_type = "extreme-ams";
      probability = GST_TYPE_FIND_LIKELY;
      goto suggest_audio_mod_caps;
    }
  }
  /* ULT (Ultratracker) */
  if ((data = gst_type_find_peek (tf, 0, 14)) != NULL) {
    if (memcmp (data, "MAS_UTrack_V00", 14) == 0) {
      mod_type = "ult";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* DIGI (DigiBooster) */
  if ((data = gst_type_find_peek (tf, 0, 20)) != NULL) {
    if (memcmp (data, "DIGI Booster module\0", 20) == 0) {
      mod_type = "digi";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* PTM (PolyTracker) */
  if ((data = gst_type_find_peek (tf, 0x2C, 4)) != NULL) {
    if (memcmp (data, "PTMF", 4) == 0) {
      mod_type = "ptm";
      probability = GST_TYPE_FIND_LIKELY;
      goto suggest_audio_mod_caps;
    }
  }
  /* XM */
  if ((data = gst_type_find_peek (tf, 0, 38)) != NULL) {
    if ((memcmp (data, "Extended Module: ", 17) == 0) && (data[37] == 0x1A)) {
      mod_type = "xm";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* OKT */
  if (data || (data = gst_type_find_peek (tf, 0, 8)) != NULL) {
    if (memcmp (data, "OKTASONG", 8) == 0) {
      mod_type = "okt";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* Various formats with a 4-byte magic ID at the beginning of the file */
  if (data || (data = gst_type_find_peek (tf, 0, 4)) != NULL) {
    /* PSM (Protracker Studio PSM) */
    if (memcmp (data, "PSM", 3) == 0) {
      unsigned char fbyte = data[3];
      if ((fbyte == ' ') || (fbyte == 254)) {
        mod_type = "psm";
        probability = GST_TYPE_FIND_MAXIMUM;
        goto suggest_audio_mod_caps;
      }
    }
    /* 669 */
    if ((memcmp (data, "if", 2) == 0) || (memcmp (data, "JN", 2) == 0)) {
      mod_type = "669";
      probability = GST_TYPE_FIND_LIKELY;
      goto suggest_audio_mod_caps;
    }
    /* AMF */
    if ((memcmp (data, "AMF", 3) == 0) && (data[3] > 10) && (data[3] < 14)) {
      mod_type = "dsmi-amf";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* IT */
    if (memcmp (data, "IMPM", 4) == 0) {
      mod_type = "it";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* DBM (DigiBooster Pro) */
    if (memcmp (data, "DBM0", 4) == 0) {
      mod_type = "dbm";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* MDL (DigiTrakker) */
    if (memcmp (data, "DMDL", 4) == 0) {
      mod_type = "mdl";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* MT2 (MadTracker 2.0) */
    if (memcmp (data, "MT20", 4) == 0) {
      mod_type = "mt2";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* DMF (X-Tracker) */
    if (memcmp (data, "DDMF", 4) == 0) {
      mod_type = "dmf";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* MED */
    if ((memcmp (data, "MMD0", 4) == 0) || (memcmp (data, "MMD1", 4) == 0)) {
      mod_type = "med";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* MTM */
    if (memcmp (data, "MTM", 3) == 0) {
      mod_type = "mtm";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* DSM */
    if (memcmp (data, "RIFF", 4) == 0) {
      const guint8 *data2 = gst_type_find_peek (tf, 8, 4);

      if (data2) {
        if (memcmp (data2, "DSMF", 4) == 0) {
          mod_type = "dsm";
          probability = GST_TYPE_FIND_MAXIMUM;
          goto suggest_audio_mod_caps;
        }
      }
    }
    /* FAR (Farandole) */
    if (memcmp (data, "FAR\xFE", 4) == 0) {
      mod_type = "far";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
    /* FAM */
    if (memcmp (data, "FAM\xFE", 4) == 0) {
      const guint8 *data2 = gst_type_find_peek (tf, 44, 3);

      if (data2) {
        if (memcmp (data2, "compare", 3) == 0) {
          mod_type = "fam";
          probability = GST_TYPE_FIND_MAXIMUM;
          goto suggest_audio_mod_caps;
        }
        /* otherwise do not suggest anything */
      } else {
        mod_type = "fam";
        probability = GST_TYPE_FIND_LIKELY;
        goto suggest_audio_mod_caps;
      }
    }
    /* GDM */
    if (memcmp (data, "GDM\xFE", 4) == 0) {
      const guint8 *data2 = gst_type_find_peek (tf, 71, 4);

      if (data2) {
        if (memcmp (data2, "GMFS", 4) == 0) {
          mod_type = "gdm";
          probability = GST_TYPE_FIND_MAXIMUM;
          goto suggest_audio_mod_caps;
        }
        /* otherwise do not suggest anything */
      } else {
        mod_type = "gdm";
        probability = GST_TYPE_FIND_LIKELY;
        goto suggest_audio_mod_caps;
      }
    }
    /* UMX */
    if (memcmp (data, "\xC1\x83\x2A\x9E", 4) == 0) {
      mod_type = "umx";
      probability = GST_TYPE_FIND_POSSIBLE;
      goto suggest_audio_mod_caps;
    }
  }
  /* FAR (Farandole) (secondary detection) */
  if ((data = gst_type_find_peek (tf, 44, 3)) != NULL) {
    if (memcmp (data, "\x0D\x0A\x1A", 3) == 0) {
      mod_type = "far";
      probability = GST_TYPE_FIND_POSSIBLE;
      goto suggest_audio_mod_caps;
    }
  }
  /* IMF */
  if ((data = gst_type_find_peek (tf, 60, 4)) != NULL) {
    if (memcmp (data, "IM10", 4) == 0) {
      mod_type = "imf";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* S3M */
  if ((data = gst_type_find_peek (tf, 44, 4)) != NULL) {
    if (memcmp (data, "SCRM", 4) == 0) {
      mod_type = "s3m";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }
  /* STM */
  if ((data = gst_type_find_peek (tf, 20, 8)) != NULL) {
    if (g_ascii_strncasecmp ((gchar *) data, "!Scream!", 8) == 0 ||
        g_ascii_strncasecmp ((gchar *) data, "BMOD2STM", 8) == 0) {
      const guint8 *id, *stmtype;

      if ((id = gst_type_find_peek (tf, 28, 1)) == NULL)
        return;
      if ((stmtype = gst_type_find_peek (tf, 29, 1)) == NULL)
        return;
      if (*id == 0x1A && *stmtype == 2) {
        mod_type = "stm";
        probability = GST_TYPE_FIND_MAXIMUM;
        goto suggest_audio_mod_caps;
      }
    }
  }
  /* AMF */
  if ((data = gst_type_find_peek (tf, 0, 19)) != NULL) {
    if (memcmp (data, "ASYLUM Music Format", 19) == 0) {
      mod_type = "asylum-amf";
      probability = GST_TYPE_FIND_MAXIMUM;
      goto suggest_audio_mod_caps;
    }
  }

suggest_audio_mod_caps:
  if (mod_type != NULL) {
    GstCaps *caps = gst_caps_new_simple ("audio/x-mod",
        "type", G_TYPE_STRING, mod_type, NULL);

    gst_type_find_suggest (tf, probability, caps);
    gst_caps_unref (caps);
  }
}