poppler_document_get_form_field (PopplerDocument *document,
				 gint             id)
{
  Page *page;
  unsigned pageNum;
  unsigned fieldNum;
  FormPageWidgets *widgets;
  FormWidget *field;

  FormWidget::decodeID (id, &pageNum, &fieldNum);

  page = document->doc->getPage (pageNum);
  if (!page)
    return nullptr;

  widgets = page->getFormWidgets ();
  if (!widgets)
    return nullptr;

  field = widgets->getWidget (fieldNum);
  if (field)
    return _poppler_form_field_new (document, field);

  return nullptr;
}