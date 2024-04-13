poppler_attachment_finalize (GObject *obj)
{
  PopplerAttachment *attachment;

  attachment = (PopplerAttachment *) obj;

  if (attachment->name)
    g_free (attachment->name);
  attachment->name = nullptr;

  if (attachment->description)
    g_free (attachment->description);
  attachment->description = nullptr;
  
  if (attachment->checksum)
    g_string_free (attachment->checksum, TRUE);
  attachment->checksum = nullptr;
  
  G_OBJECT_CLASS (poppler_attachment_parent_class)->finalize (obj);
}