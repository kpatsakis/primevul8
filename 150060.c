onig_set_default_syntax(const OnigSyntaxType* syntax)
{
  if (IS_NULL(syntax))
    syntax = ONIG_SYNTAX_RUBY;

  OnigDefaultSyntax = syntax;
  return 0;
}