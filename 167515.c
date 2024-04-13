poppler_ps_file_finalize (GObject *object)
{
        PopplerPSFile *ps_file = POPPLER_PS_FILE (object);

        delete ps_file->out;
        g_object_unref (ps_file->document);
        g_free (ps_file->filename);

        G_OBJECT_CLASS (poppler_ps_file_parent_class)->finalize (object);
}