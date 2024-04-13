add_auth_header (GstRTSPConnection * conn, GstRTSPMessage * message)
{
  switch (conn->auth_method) {
    case GST_RTSP_AUTH_BASIC:{
      gchar *user_pass;
      gchar *user_pass64;
      gchar *auth_string;

      if (conn->username == NULL || conn->passwd == NULL)
        break;

      user_pass = g_strdup_printf ("%s:%s", conn->username, conn->passwd);
      user_pass64 = g_base64_encode ((guchar *) user_pass, strlen (user_pass));
      auth_string = g_strdup_printf ("Basic %s", user_pass64);

      gst_rtsp_message_take_header (message, GST_RTSP_HDR_AUTHORIZATION,
          auth_string);

      g_free (user_pass);
      g_free (user_pass64);
      break;
    }
    case GST_RTSP_AUTH_DIGEST:{
      gchar *response;
      gchar *auth_string, *auth_string2;
      gchar *realm;
      gchar *nonce;
      gchar *opaque;
      const gchar *uri;
      const gchar *method;

      /* we need to have some params set */
      if (conn->auth_params == NULL || conn->username == NULL ||
          conn->passwd == NULL)
        break;

      /* we need the realm and nonce */
      realm = (gchar *) g_hash_table_lookup (conn->auth_params, "realm");
      nonce = (gchar *) g_hash_table_lookup (conn->auth_params, "nonce");
      if (realm == NULL || nonce == NULL)
        break;

      method = gst_rtsp_method_as_text (message->type_data.request.method);
      uri = message->type_data.request.uri;

      response =
          gst_rtsp_generate_digest_auth_response (NULL, method, realm,
          conn->username, conn->passwd, uri, nonce);
      auth_string =
          g_strdup_printf ("Digest username=\"%s\", "
          "realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"",
          conn->username, realm, nonce, uri, response);
      g_free (response);

      opaque = (gchar *) g_hash_table_lookup (conn->auth_params, "opaque");
      if (opaque) {
        auth_string2 = g_strdup_printf ("%s, opaque=\"%s\"", auth_string,
            opaque);
        g_free (auth_string);
        auth_string = auth_string2;
      }
      /* Do not keep any old Authorization headers */
      gst_rtsp_message_remove_header (message, GST_RTSP_HDR_AUTHORIZATION, -1);
      gst_rtsp_message_take_header (message, GST_RTSP_HDR_AUTHORIZATION,
          auth_string);
      break;
    }
    default:
      /* Nothing to do */
      break;
  }
}