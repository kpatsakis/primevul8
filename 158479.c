show_exim_information(enum commandline_info request, FILE *stream)
{
const uschar **pp;

switch(request)
  {
  case CMDINFO_NONE:
    fprintf(stream, "Oops, something went wrong.\n");
    return;
  case CMDINFO_HELP:
    fprintf(stream,
"The -bI: flag takes a string indicating which information to provide.\n"
"If the string is not recognised, you'll get this help (on stderr).\n"
"\n"
"  exim -bI:help    this information\n"
"  exim -bI:dscp    list of known dscp value keywords\n"
"  exim -bI:sieve   list of supported sieve extensions\n"
);
    return;
  case CMDINFO_SIEVE:
    for (pp = exim_sieve_extension_list; *pp; ++pp)
      fprintf(stream, "%s\n", *pp);
    return;
  case CMDINFO_DSCP:
    dscp_list_to_stream(stream);
    return;
  }
}