  void default_data_callback(void *, const char *file, const int offset)
  {
    if (offset < 0)
      fprintf(stderr, "%s: Unexpected end of file\n",
              file ? file : "unknown file");
    else
      fprintf(stderr, "%s: data corrupted at %d\n",
              file ? file : "unknown file", offset);
  }