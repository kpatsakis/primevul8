setup_remove_header(const uschar *hnames)
{
if (*hnames)
  acl_removed_headers = acl_removed_headers
    ? string_sprintf("%s : %s", acl_removed_headers, hnames)
    : string_copy(hnames);
}