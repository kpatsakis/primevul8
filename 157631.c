print_resolved_levels (
  const FriBidiRun *pp
)
{
  fribidi_assert (pp);

  MSG ("  Res. levels: ");
  for_run_list (pp, pp)
  {
    register FriBidiStrIndex i;
    for (i = RL_LEN (pp); i; i--)
      MSG2 ("%c", fribidi_char_from_level (RL_LEVEL (pp)));
  }
  MSG ("\n");
}