/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _SD_IMPRESSREMOTE_SERVER_HXX
#define _SD_IMPRESSREMOTE_SERVER_HXX

// SERVER
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <vector>

#include <osl/mutex.hxx>
#include <osl/socket.hxx>
#include <rtl/ref.hxx>
#include <salhelper/thread.hxx>

#include <com/sun/star/presentation/XSlideShowController.hpp>

#include "sddllapi.h"

/**
* The port for use for the main communication between LibO and remote control app.
*/
#define PORT 1599

#define CHARSET RTL_TEXTENCODING_UTF8

namespace sd
{
    class Communicator;
    class BufferedStreamSocket;

    struct ClientInfo
    {
        rtl::OUString mName;
        rtl::OUString mAddress;

        enum PROTOCOL { NETWORK = 1, BLUETOOTH };
        ClientInfo( const rtl::OUString rName, const rtl::OUString rAddress ) :
            mName( rName ),
            mAddress( rAddress ) {}
    };

    struct ClientInfoInternal;

    class RemoteServer : public salhelper::Thread
    {
        public:
            // Internal setup
            static void setup();

            // For slideshowimpl to inform us.
            static void presentationStarted( const css::uno::Reference<
                css::presentation::XSlideShowController > &rController );
            static void presentationStopped();

            // For the control dialog
            SD_DLLPUBLIC static std::vector<ClientInfo*> getClients();
            SD_DLLPUBLIC static sal_Bool connectClient( ClientInfo *pClient,
                                                        rtl::OUString aPin );

            /// ensure that discoverability (eg. for Bluetooth) is enabled
            SD_DLLPUBLIC static void ensureDiscoverable();
            /// restore the state of discoverability from before ensureDiscoverable
            SD_DLLPUBLIC static void restoreDiscoverable();

            // For the communicator
            static void removeCommunicator( Communicator* pCommunicator );
        private:
            RemoteServer();
            ~RemoteServer();
            static RemoteServer *spServer;
            static ::osl::Mutex sDataMutex;
            static ::std::vector<Communicator*> sCommunicators;
            osl::AcceptorSocket mSocket;

            ::std::vector<ClientInfoInternal*> mAvailableClients;

            void execute();
    };
}

#endif // _SD_IMPRESSREMOTE_SERVER_HXX
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
