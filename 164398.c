static void evdev_attach_client(struct evdev *evdev,
				struct evdev_client *client)
{
	spin_lock(&evdev->client_lock);
	list_add_tail_rcu(&client->node, &evdev->client_list);
	spin_unlock(&evdev->client_lock);
}