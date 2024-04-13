poppler_ps_file_free (PopplerPSFile *ps_file)
{
	g_return_if_fail (ps_file != nullptr);
        g_object_unref (ps_file);
}