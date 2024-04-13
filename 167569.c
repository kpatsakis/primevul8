poppler_document_set_creation_date (PopplerDocument *document,
                                    time_t creation_date)
{
  g_return_if_fail (POPPLER_IS_DOCUMENT (document));

  GooString *str = creation_date == (time_t)-1 ? nullptr : timeToDateString (&creation_date);
  document->doc->setDocInfoCreatDate (str);
}