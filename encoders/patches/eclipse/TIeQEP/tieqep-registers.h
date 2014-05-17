/**
 * TI eQEP driver for AM33xx devices
 *
 * Copyright (C) 2013 Nathaniel R. Lewis - http://nathanielrlewis.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TIEQEP_REGISTERS_H__
#define __TIEQEP_REGISTERS_H__

// eQEP register offsets from its base IO address
#define QPOSCNT    0x0000
#define QPOSINIT   0x0004
#define QPOSMAX    0x0008
#define QPOSCMP    0x000C
#define QPOSILAT   0x0010
#define QPOSSLAT   0x0014
#define QPOSLAT    0x0018
#define QUTMR      0x001C
#define QUPRD      0x0020
#define QWDTMR     0x0024
#define QWDPRD     0x0026
#define QDECCTL    0x0028
#define QEPCTL     0x002A
#define QCAPCTL    0x002C
#define QPOSCTL    0x002E
#define QEINT      0x0030
#define QFLG       0x0032
#define QCLR       0x0034
#define QFRC       0x0036
#define QEPSTS     0x0038
#define QCTMR      0x003A
#define QCPRD      0x003C
#define QCTMRLAT   0x003E
#define QCPRDLAT   0x0040
#define QREVID     0x005C

// Bits for the QDECTL register
#define QSRC1      (0x0001 << 15)
#define QSRC0      (0x0001 << 14)
#define SOEN       (0x0001 << 13)
#define SPSEL      (0x0001 << 12)
#define XCR        (0x0001 << 11)
#define SWAP       (0x0001 << 10)
#define IGATE      (0x0001 << 9)
#define QAP        (0x0001 << 8)
#define QBP        (0x0001 << 7)
#define QIP        (0x0001 << 6)
#define QSP        (0x0001 << 5)

// Bits for the QEPCTL register
#define FREESOFT1  (0x0001 << 15)
#define FREESOFT0  (0x0001 << 14)
#define PCRM1      (0x0001 << 13)
#define PCRM0      (0x0001 << 12)
#define SEI1       (0x0001 << 11)
#define SEI0       (0x0001 << 10)
#define IEI1       (0x0001 << 9)
#define IEI0       (0x0001 << 8)
#define SWI        (0x0001 << 7)
#define SEL        (0x0001 << 6)
#define IEL1       (0x0001 << 5)
#define IEL0       (0x0001 << 4)
#define PHEN       (0x0001 << 3)
#define QCLM       (0x0001 << 2)
#define UTE        (0x0001 << 1)
#define WDE        (0x0001 << 0)

// Bits for the QCAPCTL register
#define CEN        (0x0001 << 15)
#define CCPS2      (0x0001 << 6)
#define CCPS0      (0x0001 << 5)
#define CCPS1      (0x0001 << 4)
#define UPPS3      (0x0001 << 3)
#define UPPS2      (0x0001 << 2)
#define UPPS1      (0x0001 << 1)
#define UPPS0      (0x0001 << 0)

// Bits for the QPOSCTL register
#define PCSHDW     (0x0001 << 15)
#define PCLOAD     (0x0001 << 14)
#define PCPOL      (0x0001 << 13)
#define PCE        (0x0001 << 12)
#define PCSPW11    (0x0001 << 11)
#define PCSPW10    (0x0001 << 10)
#define PCSPW9    (0x0001 << 9)
#define PCSPW8    (0x0001 << 8)
#define PCSPW7    (0x0001 << 7)
#define PCSPW6    (0x0001 << 6)
#define PCSPW5    (0x0001 << 5)
#define PCSPW4    (0x0001 << 4)
#define PCSPW3    (0x0001 << 3)
#define PCSPW2    (0x0001 << 2)
#define PCSPW1    (0x0001 << 1)
#define PCSPW0    (0x0001 << 0)

// Bits for the interrupt registers
#define EQEP_INTERRUPT_MASK (0x0FFF)
#define UTOF                (0x0001 << 11)

// Bits to control the clock in the PWMSS subsystem
#define PWMSS_EQEPCLK_EN        BIT(4)
#define PWMSS_EQEPCLK_STOP_REQ  BIT(5)
#define PWMSS_EQEPCLK_EN_ACK    BIT(4)

#endif
