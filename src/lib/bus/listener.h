/*
* kinetic-c
* Copyright (C) 2015 Seagate Technology.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/
#ifndef LISTENER_H
#define LISTENER_H

#include "bus_types.h"
#include "bus_internal_types.h"

/** How many bits to >> the backpressure value from commands
 * delivered to the listener. */
#define LISTENER_BACKPRESSURE_SHIFT 0 /* TODO */

/** How many bits to >> the backpressure value from the listener when a
 * send has completed. */
#define LISTENER_EXPECT_BACKPRESSURE_SHIFT 7

/** Manager of incoming messages from drives, both responses and
 * unsolicited status updates. */
struct listener;

/** Initialize the listener. */
struct listener *Listener_Init(struct bus *b, struct bus_config *cfg);

/** Add/remove sockets' metadata from internal info. Blocking. */
bool Listener_AddSocket(struct listener *l, connection_info *ci, int *notify_fd);
bool Listener_RemoveSocket(struct listener *l, int fd, int *notify_fd);

/** The client is about to start a write, the listener should hold on to
 * the response (with timeout) if it arrives before receiving further
 * instructions from the client. */
bool Listener_HoldResponse(struct listener *l, int fd,
    int64_t seq_id, int16_t timeout_sec, int *notify_fd);

/** The client has finished a write, the listener should expect a response. */
bool Listener_ExpectResponse(struct listener *l, boxed_msg *box,
    uint16_t *backpressure);

/** Shut down the listener. Blocking. */
bool Listener_Shutdown(struct listener *l, int *notify_fd);

/** Free the listener, which must already be shut down. */
void Listener_Free(struct listener *l);

#ifdef TEST
#include "listener_internal_types.h"
#endif

#endif
