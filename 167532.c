poppler_document_get_property (GObject    *object,
			       guint       prop_id,
			       GValue     *value,
			       GParamSpec *pspec)
{
  PopplerDocument *document = POPPLER_DOCUMENT (object);
  guint version;

  switch (prop_id)
    {
    case PROP_TITLE:
      g_value_take_string (value, poppler_document_get_title (document));
      break;
    case PROP_FORMAT:
      g_value_take_string (value, poppler_document_get_pdf_version_string (document));
      break;
    case PROP_FORMAT_MAJOR:
      poppler_document_get_pdf_version (document, &version, nullptr);
      g_value_set_uint (value, version);
      break;
    case PROP_FORMAT_MINOR:
      poppler_document_get_pdf_version (document, nullptr, &version);
      g_value_set_uint (value, version);
      break;
    case PROP_AUTHOR:
      g_value_take_string (value, poppler_document_get_author (document));
      break;
    case PROP_SUBJECT:
      g_value_take_string (value, poppler_document_get_subject (document));
      break;
    case PROP_KEYWORDS:
      g_value_take_string (value, poppler_document_get_keywords (document));
      break;
    case PROP_CREATOR:
      g_value_take_string (value, poppler_document_get_creator (document));
      break;
    case PROP_PRODUCER:
      g_value_take_string (value, poppler_document_get_producer (document));
      break;
    case PROP_CREATION_DATE:
      g_value_set_int (value, poppler_document_get_creation_date (document));
      break;
    case PROP_MOD_DATE:
      g_value_set_int (value, poppler_document_get_modification_date (document));
      break;
    case PROP_LINEARIZED:
      g_value_set_boolean (value, poppler_document_is_linearized (document));
      break;
    case PROP_PAGE_LAYOUT:
      g_value_set_enum (value, poppler_document_get_page_layout (document));
      break;
    case PROP_PAGE_MODE:
      g_value_set_enum (value, poppler_document_get_page_mode (document));
      break;
    case PROP_VIEWER_PREFERENCES:
      /* FIXME: write... */
      g_value_set_flags (value, POPPLER_VIEWER_PREFERENCES_UNSET);
      break;
    case PROP_PERMISSIONS:
      g_value_set_flags (value, poppler_document_get_permissions (document));
      break;
    case PROP_METADATA:
      g_value_take_string (value, poppler_document_get_metadata (document));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}