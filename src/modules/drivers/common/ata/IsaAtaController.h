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

#ifndef ATA_ATA_CONTROLLER_H
#define ATA_ATA_CONTROLLER_H

#include "AtaController.h"
#include "pedigree/kernel/machine/types.h"
#include "pedigree/kernel/processor/state_forward.h"
#include "pedigree/kernel/processor/types.h"
#include "pedigree/kernel/utilities/StaticString.h"
#include "pedigree/kernel/utilities/String.h"

class Controller;

/** The controller for up to two AtaDisks. This uses a background thread
 * and a request queue. */
class IsaAtaController
    : public AtaController /* public Controller, public RequestQueue, public
                              IrqHandler */
{
  public:
    IsaAtaController(Controller *pDev, int nController = 0);
    virtual ~IsaAtaController();

    virtual void getName(String &str)
    {
        TinyStaticString s;
        s.clear();
        s += "isa-ata-";
        s.append(m_nController);
        str = String(static_cast<const char *>(s));
    }

    virtual bool sendCommand(
        size_t nUnit, uintptr_t pCommand, uint8_t nCommandSize,
        uintptr_t pRespBuffer, uint16_t nRespBytes, bool bWrite);

    virtual uint64_t executeRequest(
        uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5,
        uint64_t p6, uint64_t p7, uint64_t p8);

    // IRQ handler callback.
    virtual bool irq(irq_id_t number, InterruptState &state);

  private:
    IsaAtaController(const IsaAtaController &);
    void operator=(const IsaAtaController &);
};

#endif
