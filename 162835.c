gst_rtsp_watch_wait_backlog (GstRTSPWatch * watch, GTimeVal * timeout)
{
  gint64 end_time;
  GstClockTime to;

  g_return_val_if_fail (watch != NULL, GST_RTSP_EINVAL);

  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;
  end_time = g_get_monotonic_time () + GST_TIME_AS_USECONDS (to);

  g_mutex_lock (&watch->mutex);
  if (watch->flushing)
    goto flushing;

  while (IS_BACKLOG_FULL (watch)) {
    gboolean res;

    res = g_cond_wait_until (&watch->queue_not_full, &watch->mutex, end_time);
    if (watch->flushing)
      goto flushing;

    if (!res)
      goto timeout;
  }
  g_mutex_unlock (&watch->mutex);

  return GST_RTSP_OK;

  /* ERRORS */
flushing:
  {
    GST_DEBUG ("we are flushing");
    g_mutex_unlock (&watch->mutex);
    return GST_RTSP_EINTR;
  }
timeout:
  {
    GST_DEBUG ("we timed out");
    g_mutex_unlock (&watch->mutex);
    return GST_RTSP_ETIMEOUT;
  }
}