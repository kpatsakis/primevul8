format_line_if_present(const char *keyword, const char *opt_value)
{
  if (opt_value) {
    char *result = NULL;
    tor_asprintf(&result, "%s %s\n", keyword, opt_value);
    return result;
  } else {
    return tor_strdup("");
  }
}