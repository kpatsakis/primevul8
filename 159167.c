parse_flow_and_packet(int argc, const char *argv[],
                      struct ofproto_dpif **ofprotop, struct flow *flow,
                      struct dp_packet **packetp,
                      struct ovs_list *next_ct_states,
                      bool *consistent)
{
    const struct dpif_backer *backer = NULL;
    const char *error = NULL;
    char *m_err = NULL;
    struct simap port_names = SIMAP_INITIALIZER(&port_names);
    struct dp_packet *packet = NULL;
    uint8_t *l7 = NULL;
    size_t l7_len = 64;
    struct ofpbuf odp_key;
    struct ofpbuf odp_mask;

    ofpbuf_init(&odp_key, 0);
    ofpbuf_init(&odp_mask, 0);

    const char *args[3];
    int n_args = 0;
    bool generate_packet = false;
    if (consistent) {
        *consistent = false;
    }
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (!strcmp(arg, "-generate") || !strcmp(arg, "--generate")) {
            generate_packet = true;
        } else if (!strcmp(arg, "--l7")) {
            if (i + 1 >= argc) {
                m_err = xasprintf("Missing argument for option %s", arg);
                goto exit;
            }

            struct dp_packet payload;
            memset(&payload, 0, sizeof payload);
            dp_packet_init(&payload, 0);
            if (dp_packet_put_hex(&payload, argv[++i], NULL)[0] != '\0') {
                dp_packet_uninit(&payload);
                error = "Trailing garbage in packet data";
                goto exit;
            }
            free(l7);
            l7_len = dp_packet_size(&payload);
            l7 = dp_packet_steal_data(&payload);
        } else if (!strcmp(arg, "--l7-len")) {
            if (i + 1 >= argc) {
                m_err = xasprintf("Missing argument for option %s", arg);
                goto exit;
            }
            free(l7);
            l7 = NULL;
            l7_len = atoi(argv[++i]);
            if (l7_len > 64000) {
                m_err = xasprintf("%s: too much L7 data", argv[i]);
                goto exit;
            }
        } else if (consistent
                   && (!strcmp(arg, "-consistent") ||
                       !strcmp(arg, "--consistent"))) {
            *consistent = true;
        } else if (!strcmp(arg, "--ct-next")) {
            if (i + 1 >= argc) {
                m_err = xasprintf("Missing argument for option %s", arg);
                goto exit;
            }

            uint32_t ct_state;
            struct ds ds = DS_EMPTY_INITIALIZER;
            if (!parse_ct_state(argv[++i], 0, &ct_state, &ds)
                || !validate_ct_state(ct_state, &ds)) {
                m_err = ds_steal_cstr(&ds);
                goto exit;
            }
            oftrace_push_ct_state(next_ct_states, ct_state);
        } else if (arg[0] == '-') {
            m_err = xasprintf("%s: unknown option", arg);
            goto exit;
        } else if (n_args >= ARRAY_SIZE(args)) {
            m_err = xstrdup("too many arguments");
            goto exit;
        } else {
            args[n_args++] = arg;
        }
    }

    /* 'args' must now have one of the following forms:
     *
     *     odp_flow
     *     dpname odp_flow
     *     bridge br_flow
     *     odp_flow packet
     *     dpname odp_flow packet
     *     bridge br_flow packet
     *
     * Parse the packet if it's there.  Note that:
     *
     *     - If there is one argument, there cannot be a packet.
     *
     *     - If there are three arguments, there must be a packet.
     *
     * If there is a packet, we strip it off.
     */
    if (!generate_packet && n_args > 1) {
        error = eth_from_hex(args[n_args - 1], &packet);
        if (!error) {
            n_args--;
        } else if (n_args > 2) {
            /* The 3-argument form must end in a hex string. */
            goto exit;
        }
        error = NULL;
    }

    /* We stripped off the packet if there was one, so 'args' now has one of
     * the following forms:
     *
     *     odp_flow
     *     dpname odp_flow
     *     bridge br_flow
     *
     * Before we parse the flow, try to identify the backer, then use that
     * backer to assemble a collection of port names.  The port names are
     * useful so that the user can specify ports by name instead of number in
     * the flow. */
    if (n_args == 2) {
        /* args[0] might be dpname. */
        const char *dp_type;
        if (!strncmp(args[0], "ovs-", 4)) {
            dp_type = args[0] + 4;
        } else {
            dp_type = args[0];
        }
        backer = shash_find_data(&all_dpif_backers, dp_type);
    } else if (n_args == 1) {
        /* Pick default backer. */
        struct shash_node *node;
        if (shash_count(&all_dpif_backers) == 1) {
            node = shash_first(&all_dpif_backers);
            backer = node->data;
        }
    } else {
        error = "Syntax error";
        goto exit;
    }
    if (backer && backer->dpif) {
        struct dpif_port dpif_port;
        struct dpif_port_dump port_dump;
        DPIF_PORT_FOR_EACH (&dpif_port, &port_dump, backer->dpif) {
            simap_put(&port_names, dpif_port.name,
                      odp_to_u32(dpif_port.port_no));
        }
    }

    /* Parse the flow and determine whether a datapath or
     * bridge is specified. If function odp_flow_key_from_string()
     * returns 0, the flow is a odp_flow. If function
     * parse_ofp_exact_flow() returns NULL, the flow is a br_flow. */
    if (!odp_flow_from_string(args[n_args - 1], &port_names,
                              &odp_key, &odp_mask)) {
        if (!backer) {
            error = "Cannot find the datapath";
            goto exit;
        }

        if (odp_flow_key_to_flow(odp_key.data, odp_key.size, flow) == ODP_FIT_ERROR) {
            error = "Failed to parse datapath flow key";
            goto exit;
        }

        *ofprotop = xlate_lookup_ofproto(backer, flow,
                                         &flow->in_port.ofp_port);
        if (*ofprotop == NULL) {
            error = "Invalid datapath flow";
            goto exit;
        }

        flow->tunnel.metadata.tab = ofproto_get_tun_tab(&(*ofprotop)->up);

        /* Convert Geneve options to OpenFlow format now. This isn't actually
         * required in order to get the right results since the ofproto xlate
         * actions will handle this for us. However, converting now ensures
         * that our formatting code will always be able to consistently print
         * in OpenFlow format, which is what we use here. */
        if (flow->tunnel.flags & FLOW_TNL_F_UDPIF) {
            struct flow_tnl tnl;
            int err;

            memcpy(&tnl, &flow->tunnel, sizeof tnl);
            err = tun_metadata_from_geneve_udpif(flow->tunnel.metadata.tab,
                                                 &tnl, &tnl, &flow->tunnel);
            if (err) {
                error = "Failed to parse Geneve options";
                goto exit;
            }
        }
    } else {
        char *err;

        if (n_args != 2) {
            error = "Must specify bridge name";
            goto exit;
        }

        *ofprotop = ofproto_dpif_lookup_by_name(args[0]);
        if (!*ofprotop) {
            m_err = xasprintf("%s: unknown bridge", args[0]);
            goto exit;
        }

        struct ofputil_port_map map = OFPUTIL_PORT_MAP_INITIALIZER(&map);
        const struct ofport *ofport;
        HMAP_FOR_EACH (ofport, hmap_node, &(*ofprotop)->up.ports) {
            ofputil_port_map_put(&map, ofport->ofp_port,
                                 netdev_get_name(ofport->netdev));
        }
        err = parse_ofp_exact_flow(flow, NULL,
                                   ofproto_get_tun_tab(&(*ofprotop)->up),
                                   args[n_args - 1], &map);
        ofputil_port_map_destroy(&map);
        if (err) {
            m_err = xasprintf("Bad openflow flow syntax: %s", err);
            free(err);
            goto exit;
        }
    }

    if (generate_packet) {
        /* Generate a packet, as requested. */
        packet = dp_packet_new(0);
        flow_compose(packet, flow, l7, l7_len);
    } else if (packet) {
        /* Use the metadata from the flow and the packet argument to
         * reconstruct the flow. */
        pkt_metadata_from_flow(&packet->md, flow);
        flow_extract(packet, flow);
    }

exit:
    if (error && !m_err) {
        m_err = xstrdup(error);
    }
    if (m_err) {
        dp_packet_delete(packet);
        packet = NULL;
    }
    *packetp = packet;
    ofpbuf_uninit(&odp_key);
    ofpbuf_uninit(&odp_mask);
    simap_destroy(&port_names);
    free(l7);
    return m_err;
}