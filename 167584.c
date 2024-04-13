poppler_attachment_dispose (GObject *obj)
{
  PopplerAttachmentPrivate *priv;

  priv = POPPLER_ATTACHMENT_GET_PRIVATE (obj);
  priv->obj_stream = Object();

  G_OBJECT_CLASS (poppler_attachment_parent_class)->dispose (obj);
}