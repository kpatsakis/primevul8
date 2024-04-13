stream_is_memory_buffer_or_local_file (GInputStream *stream)
{
  return G_IS_MEMORY_INPUT_STREAM(stream) ||
    (G_IS_FILE_INPUT_STREAM(stream) && strcmp(g_type_name_from_instance((GTypeInstance*)stream), "GLocalFileInputStream") == 0);
}