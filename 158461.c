test_address(uschar *s, int flags, int *exit_value)
{
int start, end, domain;
uschar *parse_error = NULL;
uschar *address = parse_extract_address(s, &parse_error, &start, &end, &domain,
  FALSE);
if (address == NULL)
  {
  fprintf(stdout, "syntax error: %s\n", parse_error);
  *exit_value = 2;
  }
else
  {
  int rc = verify_address(deliver_make_addr(address,TRUE), stdout, flags, -1,
    -1, -1, NULL, NULL, NULL);
  if (rc == FAIL) *exit_value = 2;
    else if (rc == DEFER && *exit_value == 0) *exit_value = 1;
  }
}