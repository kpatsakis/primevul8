poppler_document_set_modification_date (PopplerDocument *document,
                                        time_t modification_date)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *str = modification_date == (time_t)-1 ? nullptr : timeToDateString (&modification_date);
  document->doc->setDocInfoModDate (str);
}