gdAlphaBlend (int dst, int src)
{
  return (((((gdAlphaTransparent - gdTrueColorGetAlpha (src)) *
	     gdTrueColorGetRed (src) / gdAlphaMax) +
	    (gdTrueColorGetAlpha (src) *
	     gdTrueColorGetRed (dst)) / gdAlphaMax) << 16) +
	  ((((gdAlphaTransparent - gdTrueColorGetAlpha (src)) *
	     gdTrueColorGetGreen (src) / gdAlphaMax) +
	    (gdTrueColorGetAlpha (src) *
	     gdTrueColorGetGreen (dst)) / gdAlphaMax) << 8) +
	  (((gdAlphaTransparent - gdTrueColorGetAlpha (src)) *
	    gdTrueColorGetBlue (src) / gdAlphaMax) +
	   (gdTrueColorGetAlpha (src) *
	    gdTrueColorGetBlue (dst)) / gdAlphaMax));
}