debug_logging_activate(uschar *tag_name, uschar *opts)
{
int fd = -1;

if (debug_file)
  {
  debug_printf("DEBUGGING ACTIVATED FROM WITHIN CONFIG.\n"
      "DEBUG: Tag=\"%s\" opts=\"%s\"\n", tag_name, opts ? opts : US"");
  return;
  }

if (tag_name != NULL && (Ustrchr(tag_name, '/') != NULL))
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "debug tag may not contain a '/' in: %s",
      tag_name);
  return;
  }

debug_selector = D_default;
if (opts)
  decode_bits(&debug_selector, 1, debug_notall, opts,
      debug_options, debug_options_count, US"debug", DEBUG_FROM_CONFIG);

/* When activating from a transport process we may never have logged at all
resulting in certain setup not having been done.  Hack this for now so we
do not segfault; note that nondefault log locations will not work */

if (!*file_path) set_file_path();

open_log(&fd, lt_debug, tag_name);

if (fd != -1)
  debug_file = fdopen(fd, "w");
else
  log_write(0, LOG_MAIN|LOG_PANIC, "unable to open debug log");
}