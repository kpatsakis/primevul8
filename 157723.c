gimp_layer_mode_from_psp_blend_mode (PSPBlendModes mode)
{
  switch (mode)
    {
    case PSP_BLEND_NORMAL:
      return GIMP_LAYER_MODE_NORMAL_LEGACY;

    case PSP_BLEND_DARKEN:
      return GIMP_LAYER_MODE_DARKEN_ONLY_LEGACY;

    case PSP_BLEND_LIGHTEN:
      return GIMP_LAYER_MODE_LIGHTEN_ONLY_LEGACY;

    case PSP_BLEND_HUE:
      return GIMP_LAYER_MODE_HSV_HUE_LEGACY;

    case PSP_BLEND_SATURATION:
      return GIMP_LAYER_MODE_HSV_SATURATION_LEGACY;

    case PSP_BLEND_COLOR:
      return GIMP_LAYER_MODE_HSL_COLOR_LEGACY;

    case PSP_BLEND_LUMINOSITY:
      return GIMP_LAYER_MODE_HSV_VALUE_LEGACY;   /* ??? */

    case PSP_BLEND_MULTIPLY:
      return GIMP_LAYER_MODE_MULTIPLY_LEGACY;

    case PSP_BLEND_SCREEN:
      return GIMP_LAYER_MODE_SCREEN_LEGACY;

    case PSP_BLEND_DISSOLVE:
      return GIMP_LAYER_MODE_DISSOLVE;

    case PSP_BLEND_OVERLAY:
      return GIMP_LAYER_MODE_OVERLAY;

    case PSP_BLEND_HARD_LIGHT:
      return GIMP_LAYER_MODE_HARDLIGHT_LEGACY;

    case PSP_BLEND_SOFT_LIGHT:
      return GIMP_LAYER_MODE_SOFTLIGHT_LEGACY;

    case PSP_BLEND_DIFFERENCE:
      return GIMP_LAYER_MODE_DIFFERENCE_LEGACY;

    case PSP_BLEND_DODGE:
      return GIMP_LAYER_MODE_DODGE_LEGACY;

    case PSP_BLEND_BURN:
      return GIMP_LAYER_MODE_BURN_LEGACY;

    case PSP_BLEND_EXCLUSION:
      return -1;                /* ??? */

    case PSP_BLEND_ADJUST:
      return -1;                /* ??? */

    case PSP_BLEND_TRUE_HUE:
      return -1;                /* ??? */

    case PSP_BLEND_TRUE_SATURATION:
      return -1;                /* ??? */

    case PSP_BLEND_TRUE_COLOR:
      return -1;                /* ??? */

    case PSP_BLEND_TRUE_LIGHTNESS:
      return -1;                /* ??? */
    }
  return -1;
}