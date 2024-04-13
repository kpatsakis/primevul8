  void set_memerror_handler(memory_callback cb, void *data)
  {
    callbacks.memcb_data = data;
    callbacks.mem_cb = cb;
  }