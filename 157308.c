static void agent_connect(UdscsConnection *conn)
{
    struct agent_data *agent_data;
    agent_data = g_new0(struct agent_data, 1);
    GError *err = NULL;

    if (session_info) {
        PidUid pid_uid = vdagent_connection_get_peer_pid_uid(VDAGENT_CONNECTION(conn), &err);
        if (err || pid_uid.pid <= 0) {
            static const char msg[] = "Could not get peer PID, disconnecting new client";
            if (err) {
                syslog(LOG_ERR, "%s: %s", msg, err->message);
                g_error_free(err);
            } else {
                syslog(LOG_ERR, "%s", msg);
            }
            agent_data_destroy(agent_data);
            udscs_server_destroy_connection(server, conn);
            return;
        }

        agent_data->session = session_info_session_for_pid(session_info, pid_uid.pid);

        uid_t session_uid = session_info_uid_for_session(session_info, agent_data->session);

        /* Check that the UID of the PID did not change, this should be done after
         * computing the session to avoid race conditions.
         * This can happen as vdagent_connection_get_peer_pid_uid get information
         * from the time of creating the socket, but the process in the meantime
         * have been replaced */
        if (!check_uid_of_pid(pid_uid.pid, pid_uid.uid) ||
            /* Check that the user launching the Agent is the same as session one
             * or root user.
             * This prevents session hijacks from other users. */
            (pid_uid.uid != 0 && pid_uid.uid != session_uid)) {
            syslog(LOG_ERR, "UID mismatch: UID=%u PID=%u suid=%u", pid_uid.uid,
                   pid_uid.pid, session_uid);
            agent_data_destroy(agent_data);
            udscs_server_destroy_connection(server, conn);
            return;
        }

        // Check there are no other connection for this session
        // Note that "conn" is not counted as "agent_data" is still not attached to it
        if (udscs_server_for_all_clients(server, connection_matches_session,
                                         agent_data->session) > 0) {
            syslog(LOG_ERR, "An agent is already connected for this session");
            agent_data_destroy(agent_data);
            udscs_server_destroy_connection(server, conn);
            return;
        }
    }

    g_object_set_data_full(G_OBJECT(conn), "agent_data", agent_data,
                           (GDestroyNotify) agent_data_destroy);
    udscs_write(conn, VDAGENTD_VERSION, 0, 0,
                (uint8_t *)VERSION, strlen(VERSION) + 1);
    update_active_session_connection(conn);

    if (device_info) {
        forward_data_to_session_agent(VDAGENTD_GRAPHICS_DEVICE_INFO,
                                      (uint8_t *) device_info, device_info_size);
    }
}