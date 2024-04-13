poppler_document_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
  PopplerDocument *document = POPPLER_DOCUMENT (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      poppler_document_set_title (document, g_value_get_string (value));
      break;
    case PROP_AUTHOR:
      poppler_document_set_author (document, g_value_get_string (value));
      break;
    case PROP_SUBJECT:
      poppler_document_set_subject (document, g_value_get_string (value));
      break;
    case PROP_KEYWORDS:
      poppler_document_set_keywords (document, g_value_get_string (value));
      break;
    case PROP_CREATOR:
      poppler_document_set_creator (document, g_value_get_string (value));
      break;
    case PROP_PRODUCER:
      poppler_document_set_producer (document, g_value_get_string (value));
      break;
    case PROP_CREATION_DATE:
      poppler_document_set_creation_date (document, g_value_get_int (value));
      break;
    case PROP_MOD_DATE:
      poppler_document_set_modification_date (document, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}