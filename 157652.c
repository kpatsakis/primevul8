fribidi_remove_bidi_marks (
  FriBidiChar *str,
  const FriBidiStrIndex len,
  FriBidiStrIndex *positions_to_this,
  FriBidiStrIndex *position_from_this_list,
  FriBidiLevel *embedding_levels
)
{
  register FriBidiStrIndex i, j = 0;
  fribidi_boolean private_from_this = false;
  fribidi_boolean status = false;

  if UNLIKELY
    (len == 0)
    {
      status = true;
      goto out;
    }

  DBG ("in fribidi_remove_bidi_marks");

  fribidi_assert (str);

  /* If to_this is not NULL, we must have from_this as well. If it is
     not given by the caller, we have to make a private instance of it. */
  if (positions_to_this && !position_from_this_list)
    {
      position_from_this_list = fribidi_malloc (sizeof
						(position_from_this_list[0]) *
						len);
      if UNLIKELY
	(!position_from_this_list) goto out;
      private_from_this = true;
      for (i = 0; i < len; i++)
	position_from_this_list[positions_to_this[i]] = i;
    }

  for (i = 0; i < len; i++)
    if (!FRIBIDI_IS_EXPLICIT_OR_BN (fribidi_get_bidi_type (str[i]))
        && !FRIBIDI_IS_ISOLATE (fribidi_get_bidi_type (str[i]))
	&& str[i] != FRIBIDI_CHAR_LRM && str[i] != FRIBIDI_CHAR_RLM)
      {
	str[j] = str[i];
	if (embedding_levels)
	  embedding_levels[j] = embedding_levels[i];
	if (position_from_this_list)
	  position_from_this_list[j] = position_from_this_list[i];
	j++;
      }

  /* Convert the from_this list to to_this */
  if (positions_to_this)
    {
      for (i = 0; i < len; i++)
	positions_to_this[i] = -1;
      for (i = 0; i < len; i++)
	positions_to_this[position_from_this_list[i]] = i;
    }

  status = true;

out:

  if (private_from_this)
    fribidi_free (position_from_this_list);

  return status ? j : -1;
}