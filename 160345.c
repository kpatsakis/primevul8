void LibRaw::remove_caseSubstr(char *string, char *subStr) // replace a substring with an equal length of spaces
{
  char *found;
  while ((found = strcasestr(string,subStr))) {
    if (!found) return;
    int fill_len = strlen(subStr);
    int p = found - string;
    for (int i=p; i<p+fill_len; i++) {
      string[i] = 32;
    }
  }
  trimSpaces (string);
}