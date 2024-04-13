_poppler_convert_pdf_date_to_gtime (const GooString *date,
				    time_t    *gdate) 
{
  gchar *date_string;
  gboolean retval;

  if (date->hasUnicodeMarker()) {
    date_string = g_convert (date->getCString () + 2,
			     date->getLength () - 2,
			     "UTF-8", "UTF-16BE", nullptr, nullptr, nullptr);		
  } else {
    date_string = g_strndup (date->getCString (), date->getLength ());
  }

  retval = poppler_date_parse (date_string, gdate);
  g_free (date_string);

  return retval;
}