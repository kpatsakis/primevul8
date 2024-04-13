  inline void reconstruct_base()
  {
    /* this subterfuge is to overcome the private-ness of
     * LibRaw_buffer_datastream */
    (LibRaw_buffer_datastream &)*this =
        LibRaw_buffer_datastream(pView_, (size_t)cbView_);
  }