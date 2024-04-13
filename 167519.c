poppler_ps_file_class_init (PopplerPSFileClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->finalize = poppler_ps_file_finalize;
}