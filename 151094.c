snmpPortOpened(const Comm::ConnectionPointer &conn, int)
{
    if (!Comm::IsConnOpen(conn))
        fatalf("Cannot open SNMP %s Port",(conn->fd == snmpIncomingConn->fd?"receiving":"sending"));

    Comm::SetSelect(conn->fd, COMM_SELECT_READ, snmpHandleUdp, NULL, 0);

    if (conn->fd == snmpIncomingConn->fd)
        debugs(1, DBG_IMPORTANT, "Accepting SNMP messages on " << snmpIncomingConn->local);
    else if (conn->fd == snmpOutgoingConn->fd)
        debugs(1, DBG_IMPORTANT, "Sending SNMP messages from " << snmpOutgoingConn->local);
    else
        fatalf("Lost SNMP port (%d) on FD %d", (int)conn->local.port(), conn->fd);
}