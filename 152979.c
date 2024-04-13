disable_clientpointer(DeviceIntPtr dev)
{
    int i;

    for (i = 0; i < currentMaxClients; i++) {
        ClientPtr client = clients[i];

        if (client && client->clientPtr == dev)
            client->clientPtr = NULL;
    }
}