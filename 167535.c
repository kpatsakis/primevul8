poppler_document_finalize (GObject *object)
{
  PopplerDocument *document = POPPLER_DOCUMENT (object);

  poppler_document_layers_free (document);
  delete document->output_dev;
  delete document->doc;

  G_OBJECT_CLASS (poppler_document_parent_class)->finalize (object);
}