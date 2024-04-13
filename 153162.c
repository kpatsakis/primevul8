sw_data_destroy (GstTypeFindData * sw_data)
{
  if (G_LIKELY (sw_data->caps != NULL))
    gst_caps_unref (sw_data->caps);
  g_slice_free (GstTypeFindData, sw_data);
}