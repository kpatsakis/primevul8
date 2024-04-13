blend_mode_name (PSPBlendModes mode)
{
  static const gchar *blend_mode_names[] =
  {
    "NORMAL",
    "DARKEN",
    "LIGHTEN",
    "HUE",
    "SATURATION",
    "COLOR",
    "LUMINOSITY",
    "MULTIPLY",
    "SCREEN",
    "DISSOLVE",
    "OVERLAY",
    "HARD_LIGHT",
    "SOFT_LIGHT",
    "DIFFERENCE",
    "DODGE",
    "BURN",
    "EXCLUSION"
  };
  static gchar *err_name = NULL;

  /* TODO: what about PSP_BLEND_ADJUST? */
  if (mode >= 0 && mode <= PSP_BLEND_EXCLUSION)
    return blend_mode_names[mode];

  g_free (err_name);
  err_name = g_strdup_printf ("unknown layer blend mode %d", mode);

  return err_name;
}