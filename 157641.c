fribidi_log2vis_get_embedding_levels (
  const FriBidiCharType *bidi_types,	/* input list of bidi types as returned by
					   fribidi_get_bidi_types() */
  const FriBidiStrIndex len,	/* input string length of the paragraph */
  FriBidiParType *pbase_dir,	/* requested and resolved paragraph
				 * base direction */
  FriBidiLevel *embedding_levels	/* output list of embedding levels */
)
{
  return fribidi_get_par_embedding_levels_ex (bidi_types, NULL, len, pbase_dir, embedding_levels);
}