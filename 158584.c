auth_heimdal_gssapi_version_report(FILE *f)
{
  /* No build-time constants available unless we link against libraries at
  build-time and export the result as a string into a header ourselves. */
  fprintf(f, "Library version: Heimdal: Runtime: %s\n"
             " Build Info: %s\n",
          heimdal_version, heimdal_long_version);
}