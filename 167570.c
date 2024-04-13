poppler_attachment_class_init (PopplerAttachmentClass *klass)
{
  G_OBJECT_CLASS (klass)->dispose = poppler_attachment_dispose;
  G_OBJECT_CLASS (klass)->finalize = poppler_attachment_finalize;
  g_type_class_add_private (klass, sizeof (PopplerAttachmentPrivate));
}