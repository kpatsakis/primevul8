fribidi_get_par_direction (
  /* input */
  const FriBidiCharType *bidi_types,
  const FriBidiStrIndex len
)
{
  register FriBidiStrIndex i;

  fribidi_assert (bidi_types);

  for (i = 0; i < len; i++)
    if (FRIBIDI_IS_LETTER (bidi_types[i]))
      return FRIBIDI_IS_RTL (bidi_types[i]) ? FRIBIDI_PAR_RTL :
	FRIBIDI_PAR_LTR;

  return FRIBIDI_PAR_ON;
}