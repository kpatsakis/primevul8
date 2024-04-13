poppler_ps_file_set_duplex (PopplerPSFile *ps_file, gboolean duplex)
{
        g_return_if_fail (ps_file->out == nullptr);

        ps_file->duplex = duplex;
}