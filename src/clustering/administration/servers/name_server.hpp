// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_SERVERS_NAME_SERVER_HPP_
#define CLUSTERING_ADMINISTRATION_SERVERS_NAME_SERVER_HPP_

#include "clustering/administration/metadata.hpp"
#include "clustering/administration/servers/name_metadata.hpp"
#include "containers/incremental_lenses.hpp"
#include "rpc/semilattice/view.hpp"

/* A `server_name_server_t` is responsible for managing a server's name. Server names are
set on the command line when RethinkDB is first started, and they can be changed at
runtime. They are stored in the semilattices and persisted across restarts.

A server's name entry in the semilattices should only be modified by that server itself,
although other servers may delete the entry if the server is being permanently removed;
so all rename requests must be first sent to the `server_name_server_t` for the server
being renamed.

If two servers ever come to have the same name due to a race condition, one of the
`server_name_server_t`s will automatically rename itself. */

class server_name_server_t : public home_thread_mixin_t {
public:
    server_name_server_t(
        mailbox_manager_t *_mailbox_manager,
        machine_id_t _my_machine_id,
        clone_ptr_t<watchable_t<change_tracking_map_t<peer_id_t,
            cluster_directory_metadata_t> > > _directory_view,
        boost::shared_ptr<semilattice_readwrite_view_t<machines_semilattice_metadata_t> >
            _semilattice_view
        );

    server_name_business_card_t get_business_card();

    name_string_t get_my_name() {
        guarantee(!permanently_removed_cond.is_pulsed());
        return my_name;
    }

    signal_t *get_permanently_removed_signal() {
        return &permanently_removed_cond;
    }

private:
    /* `rename_me()` renames the server unconditionally, without checking for conflicts.
    It does not block. The `server_metadata_server_t` calls it internally, and the
    `server_metadata_client_t` also calls it remotely through `rename_mailbox`. */
    void rename_me(const name_string_t &new_name);

    /* `on_rename_request()` is called in response to a rename request over the network
    */
    void on_rename_request(const name_string_t &new_name,
                           mailbox_t<void()>::address_t ack_addr);

    /* `on_semilattice_change()` checks if we have been deleted and also checks for name
    conflicts. It does not block, but it may call `on_rename()`. */
    void on_semilattice_change();
    
    mailbox_manager_t *mailbox_manager;
    machine_id_t my_machine_id;
    time_t startup_time;
    name_string_t my_name;
    cond_t permanently_removed_cond;

    clone_ptr_t<watchable_t<change_tracking_map_t<peer_id_t,
        cluster_directory_metadata_t> > > directory_view;
    boost::shared_ptr<semilattice_readwrite_view_t<machines_semilattice_metadata_t> >
        semilattice_view;

    auto_drainer_t drainer;

    server_name_business_card_t::rename_mailbox_t rename_mailbox;
    semilattice_readwrite_view_t<machines_semilattice_metadata_t>::subscription_t
        semilattice_subs;
};

#endif /* CLUSTERING_ADMINISTRATION_SERVERS_NAME_SERVER_HPP_ */

