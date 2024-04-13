poppler_document_get_modification_date (PopplerDocument *document)
{
  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), (time_t)-1);

  GooString *str = document->doc->getDocInfoModDate();
  if (str == nullptr) {
    return (time_t)-1;
  }

  time_t date;
  gboolean success = _poppler_convert_pdf_date_to_gtime (str, &date);
  delete str;

  return (success) ? date : (time_t)-1;
}