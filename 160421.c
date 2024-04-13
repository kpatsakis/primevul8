  void set_progress_handler(progress_callback pcb, void *data)
  {
    callbacks.progresscb_data = data;
    callbacks.progress_cb = pcb;
  }