poppler_document_class_init (PopplerDocumentClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = poppler_document_finalize;
  gobject_class->get_property = poppler_document_get_property;
  gobject_class->set_property = poppler_document_set_property;

  /**
   * PopplerDocument:title:
   *
   * The document's title or %NULL
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_TITLE,
				   g_param_spec_string ("title",
							"Document Title",
							"The title of the document",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:format:
   *
   * The PDF version as string. See also poppler_document_get_pdf_version_string()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_FORMAT,
				   g_param_spec_string ("format",
							"PDF Format",
							"The PDF version of the document",
							nullptr,
							G_PARAM_READABLE));

  /**
   * PopplerDocument:format-major:
   *
   * The PDF major version number. See also poppler_document_get_pdf_version()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_FORMAT_MAJOR,
				   g_param_spec_uint ("format-major",
						      "PDF Format Major",
						      "The PDF major version number of the document",
						      0, G_MAXUINT, 1,
						      G_PARAM_READABLE));

  /**
   * PopplerDocument:format-minor:
   *
   * The PDF minor version number. See also poppler_document_get_pdf_version()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_FORMAT_MINOR,
				   g_param_spec_uint ("format-minor",
						      "PDF Format Minor",
						      "The PDF minor version number of the document",
						      0, G_MAXUINT, 0,
						      G_PARAM_READABLE));

  /**
   * PopplerDocument:author:
   *
   * The author of the document
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_AUTHOR,
				   g_param_spec_string ("author",
							"Author",
							"The author of the document",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:subject:
   *
   * The subject of the document
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_SUBJECT,
				   g_param_spec_string ("subject",
							"Subject",
							"Subjects the document touches",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:keywords:
   *
   * The keywords associated to the document
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_KEYWORDS,
				   g_param_spec_string ("keywords",
							"Keywords",
							"Keywords",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:creator:
   *
   * The creator of the document. See also poppler_document_get_creator()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_CREATOR,
				   g_param_spec_string ("creator",
							"Creator",
							"The software that created the document",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:producer:
   *
   * The producer of the document. See also poppler_document_get_producer()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_PRODUCER,
				   g_param_spec_string ("producer",
							"Producer",
							"The software that converted the document",
							nullptr,
							G_PARAM_READWRITE));

  /**
   * PopplerDocument:creation-date:
   *
   * The date the document was created as seconds since the Epoch, or -1
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_CREATION_DATE,
				   g_param_spec_int ("creation-date",
						     "Creation Date",
						     "The date and time the document was created",
						     -1, G_MAXINT, -1,
						     G_PARAM_READWRITE));

  /**
   * PopplerDocument:mod-date:
   *
   * The date the document was most recently modified as seconds since the Epoch, or -1
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_MOD_DATE,
				   g_param_spec_int ("mod-date",
						     "Modification Date",
						     "The date and time the document was modified",
						     -1, G_MAXINT, -1,
						     G_PARAM_READWRITE));

  /**
   * PopplerDocument:linearized:
   *
   * Whether document is linearized. See also poppler_document_is_linearized()
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_LINEARIZED,
				   g_param_spec_boolean ("linearized",
							 "Fast Web View Enabled",
							 "Is the document optimized for web viewing?",
							 FALSE,
							 G_PARAM_READABLE));

  /**
   * PopplerDocument:page-layout:
   *
   * The page layout that should be used when the document is opened
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_PAGE_LAYOUT,
				   g_param_spec_enum ("page-layout",
						      "Page Layout",
						      "Initial Page Layout",
						      POPPLER_TYPE_PAGE_LAYOUT,
						      POPPLER_PAGE_LAYOUT_UNSET,
						      G_PARAM_READABLE));

  /**
   * PopplerDocument:page-mode:
   *
   * The mode that should be used when the document is opened
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_PAGE_MODE,
				   g_param_spec_enum ("page-mode",
						      "Page Mode",
						      "Page Mode",
						      POPPLER_TYPE_PAGE_MODE,
						      POPPLER_PAGE_MODE_UNSET,
						      G_PARAM_READABLE));

  /**
   * PopplerDocument:viewer-preferences:
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_VIEWER_PREFERENCES,
				   g_param_spec_flags ("viewer-preferences",
						       "Viewer Preferences",
						       "Viewer Preferences",
						       POPPLER_TYPE_VIEWER_PREFERENCES,
						       POPPLER_VIEWER_PREFERENCES_UNSET,
						       G_PARAM_READABLE));

  /**
   * PopplerDocument:permissions:
   *
   * Flags specifying which operations are permitted when the document is opened
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_PERMISSIONS,
				   g_param_spec_flags ("permissions",
						       "Permissions",
						       "Permissions",
						       POPPLER_TYPE_PERMISSIONS,
						       POPPLER_PERMISSIONS_FULL,
						       G_PARAM_READABLE));

  /**
   * PopplerDocument:metadata:
   *
   * Document metadata in XML format, or %NULL
   */
  g_object_class_install_property (G_OBJECT_CLASS (klass),
				   PROP_METADATA,
				   g_param_spec_string ("metadata",
							"XML Metadata",
							"Embedded XML metadata",
							nullptr,
							G_PARAM_READABLE));
}