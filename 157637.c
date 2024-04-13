fribidi_reorder_line (
  /* input */
  FriBidiFlags flags, /* reorder flags */
  const FriBidiCharType *bidi_types,
  const FriBidiStrIndex len,
  const FriBidiStrIndex off,
  const FriBidiParType base_dir,
  /* input and output */
  FriBidiLevel *embedding_levels,
  FriBidiChar *visual_str,
  /* output */
  FriBidiStrIndex *map
)
{
  fribidi_boolean status = false;
  FriBidiLevel max_level = 0;

  if UNLIKELY
    (len == 0)
    {
      status = true;
      goto out;
    }

  DBG ("in fribidi_reorder_line");

  fribidi_assert (bidi_types);
  fribidi_assert (embedding_levels);

  DBG ("reset the embedding levels, 4. whitespace at the end of line");
  {
    register FriBidiStrIndex i;

    /* L1. Reset the embedding levels of some chars:
       4. any sequence of white space characters at the end of the line. */
    for (i = off + len - 1; i >= off &&
	 FRIBIDI_IS_EXPLICIT_OR_BN_OR_WS (bidi_types[i]); i--)
      embedding_levels[i] = FRIBIDI_DIR_TO_LEVEL (base_dir);
  }

  /* 7. Reordering resolved levels */
  {
    register FriBidiLevel level;
    register FriBidiStrIndex i;

    /* Reorder both the outstring and the order array */
    {
      if (FRIBIDI_TEST_BITS (flags, FRIBIDI_FLAG_REORDER_NSM))
	{
	  /* L3. Reorder NSMs. */
	  for (i = off + len - 1; i >= off; i--)
	    if (FRIBIDI_LEVEL_IS_RTL (embedding_levels[i])
		&& bidi_types[i] == FRIBIDI_TYPE_NSM)
	      {
		register FriBidiStrIndex seq_end = i;
		level = embedding_levels[i];

		for (i--; i >= off &&
		     FRIBIDI_IS_EXPLICIT_OR_BN_OR_NSM (bidi_types[i])
		     && embedding_levels[i] == level; i--)
		  ;

		if (i < off || embedding_levels[i] != level)
		  {
		    i++;
		    DBG ("warning: NSM(s) at the beginning of level run");
		  }

		if (visual_str)
		  {
		    bidi_string_reverse (visual_str + i, seq_end - i + 1);
		  }
		if (map)
		  {
		    index_array_reverse (map + i, seq_end - i + 1);
		  }
	      }
	}

      /* Find max_level of the line.  We don't reuse the paragraph
       * max_level, both for a cleaner API, and that the line max_level
       * may be far less than paragraph max_level. */
      for (i = off + len - 1; i >= off; i--)
	if (embedding_levels[i] > max_level)
	  max_level = embedding_levels[i];

      /* L2. Reorder. */
      for (level = max_level; level > 0; level--)
	for (i = off + len - 1; i >= off; i--)
	  if (embedding_levels[i] >= level)
	    {
	      /* Find all stretches that are >= level_idx */
	      register FriBidiStrIndex seq_end = i;
	      for (i--; i >= off && embedding_levels[i] >= level; i--)
		;

	      if (visual_str)
		bidi_string_reverse (visual_str + i + 1, seq_end - i);
	      if (map)
		index_array_reverse (map + i + 1, seq_end - i);
	    }
    }

  }

  status = true;

out:

  return status ? max_level + 1 : 0;
}