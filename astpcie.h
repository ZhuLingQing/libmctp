/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */

/* This is a private header file defining binding structure for PCIe binding */

#ifndef _ASTPCIE_H
#define _ASTPCIE_H

#include "libmctp.h"

#define MCTP_ASTPCIE_BINDING_DEFAULT_BUFFER 1024

struct mctp_binding_astpcie {
	struct mctp_binding binding;
	uint16_t bdf;
	int fd;
};

/* returns pointer to structure holding this generic binding */
#define binding_to_astpcie(b)                                                  \
	container_of(b, struct mctp_binding_astpcie, binding)

/* driver device file */
#define AST_DRV_FILE "/dev/aspeed-mctp"

struct mctp_pcie_hdr {
	uint8_t fmt_type;
	uint8_t mbz;
	uint16_t mbz_attr_length;
	uint16_t requester;
	uint8_t tag;
	uint8_t code;
	uint16_t target;
	uint16_t vendor;
} __attribute__((packed));

/*
 * MCTP PCIe template values
 * The following non-zero values are defined by DSP0238 DMTF Spec as constants:
 * .fmt_type:
 * ----------
 * [4:0]: Type[4:3] = 10b to indicate a message.
 * [6:5]: Fmt = 11b to indicate 4 dword header with data.
 * ----------
 * .mbz_attr_length:
 * [5:4]: Attr[1:0] = 01b for all MCTP over PCIe VDM
 * ----------
 * .code
 * ----------
 * [7:0]: Message Code = 0111_1111b to indicate a Type 1 VDM
 * ----------
 * .vendor
 * ----------
 * byte2[7:0]: Vendor ID MSB = 0x1a - DMTF VDMs
 * byte3[7:0]: Vendor ID LSB = 0xb4 - DMTF VDMs
 *
 * See more details in Table 1 of DSP0238 DMTF Spec.
 */
#define MSG_4DW_HDR 0x70
#define MCTP_PCIE_VDM_ATTR 0x0010
#define MSG_CODE_VDM_TYPE_1 0x7f
#define VENDOR_ID_DMTF_VDM 0xb41a

/*
 * Calculates offset of payload for whole PCIe VMD frame
 */
#define PCIE_MCTP_HDR_OFFSET (sizeof(struct pcie_header))
#define PCIE_PAYLOAD_OFFSET (PCIE_MCTP_HDR_OFFSET + sizeof(struct mctp_hdr))

/* clang-format off */
/*
 * Defines PCIe medium specific header according to spec: DSP0238
 */
struct pcie_header {
#define PCIE_FTR_FMT_SHIFT (5)
#define PCIE_FTR_FMT_MASK (0x3)
#define PCIE_FTR_TYPE_SHIFT (0)
#define PCIE_FTR_TYPE_MASK (0x3)
#define PCIE_FTR_ROUTING_SHIFT (0)
#define PCIE_FTR_ROUTING_MASK (0x7)
	uint8_t r_fmt_type_rout;    /* [7]   - reserved
				     * [6:5] - format: =11b for 4 dword header
				     * [4:3] - =10b type message
				     * [2:0] - pci message routing
				     */
#define PCIE_TR_TRCL_SHIFT (4)
#define PCIE_TR_TRCL_MASK (0x7)
#define PCIE_TR_FLAG_ATTR (1 << 2)
#define PCIE_TR_FLAG_TH (1 << 0)
	uint8_t r_trcl_r;           /* [7]   - reserved
				     * [6:4] - traffic class =000b
				     * [3:0] - reserved2, or
				     * [3]   - reserved2
				     * [2]   - attr: =0b
				     * [1]   - reserved3
				     * [0]   - TH: =0b
				     */
#define PCIE_TEARL_MASK_TD (0x1)
#define PCIE_TEARL_SHIFT_TD (7)
#define PCIE_TEARL_FLAG_TD (1<<7)
#define PCIE_TEARL_MASK_EP (0x1)
#define PCIE_TEARL_SHIFT_EP (6)
#define PCIE_TEARL_FLAG_EP (1<<6)
#define PCIE_TEARL_ATTR_SHIFT (4)
#define PCIE_TEARL_ATTR_MASK (0x30)
#define PCIE_TEARL_LEN1_SHIFT (0)
#define PCIE_TEARL_LEN1_MASK (0x3)
	uint8_t td_ep_attr_r_l1;    /* [7]    - TD =0b
				     * [6]    - EP =0b
				     * [5:4] - Attr =00b or =01b
				     * [3:2] - reserved
				     * [1:0]   - length msb
				     */
	uint8_t len2;               /* [7:0] - length lsb */
	uint16_t pci_requester_id;  /* [15:0] - PCI Requester Id (bdf)
				     */
#define PCIE_PCITAG_PADLEN_SHIFT (4)
#define PCIE_PCITAG_PADLEN_MASK (0x3)
#define PCIE_PCITAG_MVC_SHIFT (0)
#define PCIE_PCITAG_MVC_MASK (0xf)
#define PCIE_PCITAG_MVC_VALUE (0x0) /* hardcoded value */
	uint8_t pcitag;             /* [7:0] - PCI Tag Field
				     * [7:6] - reserved
				     * [5:4] - pad len
				     * [3:0] - mctp vdm code =0000b
				     */

	uint8_t message_code;       /* 0x7F for Type 1 VDM */
	uint16_t pci_target_id;     /* pci target id*/
	uint16_t vendor_id;         /* vendor id =0x1ab4 for DMTF */
} __attribute__ ((packed));

/*
 * Calculates length from header fields
 */
#define PCIE_GET_LEN(header)                                                   \
	(size_t)(((header)->td_ep_attr_r_l1 & PCIE_TEARL_LEN1_MASK)            \
			 << CHAR_BIT |                                         \
		 (header)->len2)
/*
 * Gets padding from header
 */
#define PCIE_GET_PAD(header)                                                   \
	(size_t)(((header)->pcitag >> PCIE_PCITAG_PADLEN_SHIFT) &              \
		 PCIE_PCITAG_PADLEN_MASK)

/*
 * Calculate padding to get dword alignment
 */
#define PCIE_COUNT_PAD(len) ((-len) & (sizeof(uint32_t) - 1))

#endif
