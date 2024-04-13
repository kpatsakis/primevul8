gs_getdefaultlibdevice(gs_memory_t *mem)
{
    const gx_device *const *list;
    int count = gs_lib_device_list(&list, NULL);
    const char *name, *end, *fin;
    int i;

    /* Search the compiled in device list for a known device name */
    /* In the case the lib ctx hasn't been initialised */
    if (mem && mem->gs_lib_ctx && mem->gs_lib_ctx->default_device_list) {
        name = mem->gs_lib_ctx->default_device_list;
        fin = name + strlen(name);
    }
    else {
        name = gs_dev_defaults;
        fin = name + strlen(name);
    }

    /* iterate through each name in the string */
    while (name < fin) {

      /* split a name from any whitespace */
      while ((name < fin) && (*name == ' ' || *name == '\t'))
        name++;
      end = name;
      while ((end < fin) && (*end != ' ') && (*end != '\t'))
        end++;

      /* return any matches */
      for (i = 0; i < count; i++)
        if ((end - name) == strlen(list[i]->dname))
          if (!memcmp(name, list[i]->dname, end - name))
            return gs_getdevice(i);

      /* otherwise, try the next device name */
      name = end;
    }

    /* Fall back to the first device in the list. */
    return gs_getdevice(0);
}