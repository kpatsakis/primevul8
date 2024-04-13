_poppler_document_get_layers (PopplerDocument *document)
{
  if (!document->layers) {
    Catalog *catalog = document->doc->getCatalog ();
    OCGs *ocg = catalog->getOptContentConfig ();

    if (!ocg)
      return nullptr;
    
    document->layers = get_optional_content_items (ocg);
    document->layers_rbgroups = get_optional_content_rbgroups (ocg);
  }

  return document->layers;
}