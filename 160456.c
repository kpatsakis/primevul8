  void set_dataerror_handler(data_callback func, void *data)
  {
    callbacks.datacb_data = data;
    callbacks.data_cb = func;
  }