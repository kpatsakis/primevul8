print_resolved_types (
  const FriBidiRun *pp
)
{
  fribidi_assert (pp);

  MSG ("  Res. types : ");
  for_run_list (pp, pp)
  {
    FriBidiStrIndex i;
    for (i = RL_LEN (pp); i; i--)
      MSG2 ("%s ", fribidi_get_bidi_type_name (pp->type));
  }
  MSG ("\n");
}