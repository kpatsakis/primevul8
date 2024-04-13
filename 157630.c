compact_neutrals (
  FriBidiRun *list
)
{
  fribidi_assert (list);

  if (list->next)
    {
      for_run_list (list, list)
      {
	if (RL_LEVEL (list->prev) == RL_LEVEL (list)
	    &&
	    ((RL_TYPE (list->prev) == RL_TYPE (list)
	      || (FRIBIDI_IS_NEUTRAL (RL_TYPE (list->prev))
		  && FRIBIDI_IS_NEUTRAL (RL_TYPE (list)))))
            && RL_BRACKET_TYPE(list) == FRIBIDI_NO_BRACKET /* Don't join brackets! */
            && RL_BRACKET_TYPE(list->prev) == FRIBIDI_NO_BRACKET
            )
	  list = merge_with_prev (list);
      }
    }
}