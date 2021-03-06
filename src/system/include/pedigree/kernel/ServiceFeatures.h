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

#ifndef SERVICE_FEATURES_H
#define SERVICE_FEATURES_H

#include "pedigree/kernel/compiler.h"
#include "pedigree/kernel/processor/types.h"

/** Service Operation Enumeration
 *
 *  This class provides a central interface for finding out the features
 *  a Service exposes to the system.
 */
class EXPORTED_PUBLIC ServiceFeatures
{
  public:
    enum Type
    {
        /** Write: send data to the Service. Open to interpretation (OTI) */
        write = 0,

        /** Read: obtain data form the service. OTI */
        read = 1,

        /** Touch: inform of new state, pass relevant object data */
        touch = 2,

        /** Probe: what's happening? OTI */
        probe = 4
    };

    ServiceFeatures();
    virtual ~ServiceFeatures();

    /** Does the operation provide a specific service? */
    virtual bool provides(Type service);

    /** Used by the Service itself when installing */
    virtual void add(Type s);

    /** Used by the Service itself to dynamically change offered features */
    virtual void remove(Type s);

  private:
    uint32_t m_OpEnum;
};

#endif
