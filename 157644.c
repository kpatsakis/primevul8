print_bidi_string (
  /* input */
  const FriBidiCharType *bidi_types,
  const FriBidiStrIndex len
)
{
  register FriBidiStrIndex i;

  fribidi_assert (bidi_types);

  MSG ("  Org. types : ");
  for (i = 0; i < len; i++)
    MSG2 ("%s ", fribidi_get_bidi_type_name (bidi_types[i]));
  MSG ("\n");
}