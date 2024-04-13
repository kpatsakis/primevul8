print_types_re (
  const FriBidiRun *pp
)
{
  fribidi_assert (pp);

  MSG ("  Run types  : ");
  for_run_list (pp, pp)
  {
    MSG6 ("%d:%d(%s)[%d,%d] ",
	  pp->pos, pp->len, fribidi_get_bidi_type_name (pp->type), pp->level, pp->isolate_level);
  }
  MSG ("\n");
}