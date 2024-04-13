fribidi_get_par_embedding_levels (
  /* input */
  const FriBidiCharType *bidi_types,
  const FriBidiStrIndex len,
  /* input and output */
  FriBidiParType *pbase_dir,
  /* output */
  FriBidiLevel *embedding_levels
)
{
  return fribidi_get_par_embedding_levels_ex (/* input */
                                              bidi_types,
                                              NULL, /* No bracket_types */
                                              len,
                                              /* input and output */
                                              pbase_dir,
                                              /* output */
                                              embedding_levels);
}