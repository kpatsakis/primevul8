ephy_embed_single_class_init (EphyEmbedSingleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = ephy_embed_single_finalize;
  object_class->get_property = ephy_embed_single_get_property;
  object_class->set_property = ephy_embed_single_set_property;

  /**
   * EphyEmbedSingle::new-window:
   * @single:
   * @parent_embed: the #EphyEmbed requesting the new window, or %NULL
   * @mask: a #EphyEmbedChrome
   *
   * The ::new_window signal is emitted when a new window needs to be opened.
   * For example, when a JavaScript popup window was opened.
   *
   * Return a new #EphyEmbed.
   **/
  g_signal_new ("new-window",
                EPHY_TYPE_EMBED_SINGLE,
                G_SIGNAL_RUN_FIRST | G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (EphyEmbedSingleClass, new_window),
                ephy_signal_accumulator_object, ephy_embed_get_type,
                ephy_marshal_OBJECT__OBJECT_FLAGS,
                GTK_TYPE_WIDGET,
                2,
                GTK_TYPE_WIDGET,
                EPHY_TYPE_WEB_VIEW_CHROME);

  /**
   * EphyEmbedSingle::handle_content:
   * @single:
   * @mime_type: the MIME type of the content
   * @address: the URL to the content
   *
   * The ::handle_content signal is emitted when encountering content of a mime
   * type Epiphany is unable to handle itself.
   *
   * If a connected callback returns %TRUE, the signal will stop propagating. For
   * example, this could be used by a download manager to prevent other
   * ::handle_content listeners from being called.
   **/
  g_signal_new ("handle_content",
                EPHY_TYPE_EMBED_SINGLE,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (EphyEmbedSingleClass, handle_content),
                g_signal_accumulator_true_handled, NULL,
                ephy_marshal_BOOLEAN__STRING_STRING,
                G_TYPE_BOOLEAN,
                2,
                G_TYPE_STRING,
                G_TYPE_STRING);

  /**
   * EphyEmbedSingle::add-sidebar:
   * @single:
   * @url: The url of the sidebar to be added
   * @title: The title of the sidebar to be added
   *
   * The ::add-sidebar signal is emitted when the user clicks a javascript link that
   * requests adding a url to the sidebar.
   **/
  g_signal_new ("add-sidebar",
                EPHY_TYPE_EMBED_SINGLE,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (EphyEmbedSingleClass, add_sidebar),
                g_signal_accumulator_true_handled, NULL,
                ephy_marshal_BOOLEAN__STRING_STRING,
                G_TYPE_BOOLEAN,
                2,
                G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * EphyEmbedSingle::add-search-engine
   * @single:
   * @url: The url of the search engine definition file
   * @icon_url: The url of the icon to use for this engine
   * @title: The title of the search engine to be added
   *
   * The ::add-search-engine signal is emitted when the user clicks a javascript link that
   * requests adding a search engine to the sidebar.
   **/
  g_signal_new ("add-search-engine",
                EPHY_TYPE_EMBED_SINGLE,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (EphyEmbedSingleClass, add_search_engine),
                g_signal_accumulator_true_handled, NULL,
                ephy_marshal_BOOLEAN__STRING_STRING_STRING,
                G_TYPE_BOOLEAN,
                3,
                G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * EphyEmbedSingle::network-status:
   * 
   * Whether the network is on-line.
   */
  g_object_class_install_property
    (object_class,
     PROP_NETWORK_STATUS,
     g_param_spec_boolean ("network-status",
                           "network-status",
                           "network-status",
                           FALSE,
                           G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_type_class_add_private (object_class, sizeof (EphyEmbedSinglePrivate));
}