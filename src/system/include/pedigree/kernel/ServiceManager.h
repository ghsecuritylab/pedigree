/*
 * Copyright (c) 2008-2014, Pedigree Developers
 *
 * Please see the CONTRIB file in the root of the source tree for a full
 * list of contributors.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "pedigree/kernel/compiler.h"
#include "pedigree/kernel/utilities/RadixTree.h"
#include "pedigree/kernel/utilities/String.h"

class Service;
class ServiceFeatures;

/// \todo Integrate with the Event system somehow

/** Service Manager
 *
 *  The service manager controls Services in a central location, by allowing
 *  them to be referred to by name.
 *
 *  It also provides services such as enumeration of Service operations.
 */
class EXPORTED_PUBLIC ServiceManager
{
  public:
    ServiceManager();
    virtual ~ServiceManager();

    static ServiceManager &instance()
    {
        return m_Instance;
    }

    /**
     *  Enumerates all possible operations that can be performed for a
     *  given Service
     */
    ServiceFeatures *enumerateOperations(const String &serviceName);

    /** Adds a service to the manager */
    void
    addService(const String &serviceName, Service *s, ServiceFeatures *feats);

    /** Removes a service from the manager */
    void removeService(const String &serviceName);

    /** Gets the Service object for a service */
    Service *getService(const String &serviceName);

  private:
    static ServiceManager m_Instance;

    /** Internal representation of a Service */
    class InternalService
    {
      public:
        /// The Service itself
        Service *pService;

        /// Service operation enumeration
        ServiceFeatures *pFeatures;
    };

    /** Services we know about */
    RadixTree<InternalService *> m_Services;
};
#endif
