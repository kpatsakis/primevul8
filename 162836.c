gen_date_string (gchar * date_string, guint len)
{
  static const char wkdays[7][4] =
      { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  static const char months[12][4] =
      { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
    "Nov", "Dec"
  };
  struct tm tm;
  time_t t;

  time (&t);

#ifdef HAVE_GMTIME_R
  gmtime_r (&t, &tm);
#else
  tm = *gmtime (&t);
#endif

  g_snprintf (date_string, len, "%s, %02d %s %04d %02d:%02d:%02d GMT",
      wkdays[tm.tm_wday], tm.tm_mday, months[tm.tm_mon], tm.tm_year + 1900,
      tm.tm_hour, tm.tm_min, tm.tm_sec);
}