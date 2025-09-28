#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/io.h>

#include <asm/nyx_api.h>

#define MODULE_NAME "xhci_fuzzer"

/* xHCI capability registers */
#define HC_CAPBASE        0x00
#define HC_HCSPARAMS1     0x04
#define HC_HCSPARAMS2     0x08
#define HC_HCSPARAMS3     0x0C
#define HC_HCCPARAMS1     0x10
#define HC_DOORBELLOFF    0x14
#define HC_RUNTIMEOFF     0x18
#define HC_HCCPARAMS2     0x1C

/* xHCI operational registers */
#define HC_USBCMD         0x00
#define HC_USBSTS         0x04
#define HC_PAGESIZE       0x08
#define HC_DNCTRL         0x14
#define HC_CRCR_LOW       0x18
#define HC_CRCR_HIGH      0x1C
#define HC_DCBAAP_LOW     0x30
#define HC_DCBAAP_HIGH    0x34
#define HC_CONFIG         0x38

/* xHCI runtime registers */
#define HC_MFINDEX       0x00

/** Size of the xHCI memory-mapped I/O region. */
#define XHCI_MMIO_SIZE              _64K

/** Size of the capability part of the MMIO region.  */
#define XHCI_CAPS_REG_SIZE          0x80

/** Offset of the port registers in operational register space. */
#define XHCI_PORT_REG_OFFSET        0x400

/** Offset of xHCI extended capabilities in MMIO region.  */
#define XHCI_XECP_OFFSET            0x1000

/** Offset of the run-time registers in MMIO region.  */
#define XHCI_RTREG_OFFSET           0x2000

/** Offset of the doorbell registers in MMIO region.  */
#define XHCI_DOORBELL_OFFSET        0x3000

/** Size of the extended capability area. */
#define XHCI_EXT_CAP_SIZE           1024

/* xHCI context structures */
struct xhci_device_context {
    u32 slot_context[8];
    u32 endpoint_context[8];
};

struct xhci_input_control_context {
    u32 drop_context_flags;
    u32 add_context_flags;
    u32 rsvd[6];
};

struct xhci_input_context {
    struct xhci_input_control_context control;
    struct xhci_device_context device;
};

/* Module globals */
static void __iomem *xhci_mmio_base;
static struct pci_dev *xhci_pci_dev;
static struct usb_device *target_device;
static struct urb *fuzz_urb;
static dma_addr_t input_ctx_dma;
static struct xhci_input_context *input_ctx;
static xhci_payload_t* payload = NULL;

static struct resource *res = NULL;
static phys_addr_t phys_addr = 0;


static int xhci_fuzzer_probe(struct usb_interface *interface,
                            const struct usb_device_id *id);
static void xhci_fuzzer_disconnect(struct usb_interface *interface);

void xhci_trigger_complex_failures(void __iomem *base);

static struct usb_device_id xhci_fuzzer_table[] = {
    { USB_DEVICE(0x80ee, 0x0021) }, // VirtualBox USB tablet
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, xhci_fuzzer_table);

static struct usb_driver xhci_fuzzer_driver = {
    .name = MODULE_NAME,
    .probe = xhci_fuzzer_probe,
    .disconnect = xhci_fuzzer_disconnect,
    .id_table = xhci_fuzzer_table,
};

/* Helper functions */
static u32 xhci_read32(u32 offset)
{
    return readl(xhci_mmio_base + offset);
}

static void xhci_write32(u32 offset, u32 value)
{
    writel(value, xhci_mmio_base + offset);
}

static void xhci_dump_cap_regs(void)
{
    printk(KERN_INFO "xHCI Capability Registers:\n");
    printk(KERN_INFO "HC_CAPBASE:    0x%08x\n", xhci_read32(HC_CAPBASE));
    printk(KERN_INFO "HC_HCSPARAMS1: 0x%08x\n", xhci_read32(HC_HCSPARAMS1));
    printk(KERN_INFO "HC_HCSPARAMS2: 0x%08x\n", xhci_read32(HC_HCSPARAMS2));
    printk(KERN_INFO "HC_HCSPARAMS3: 0x%08x\n", xhci_read32(HC_HCSPARAMS3));
    printk(KERN_INFO "HC_HCCPARAMS1: 0x%08x\n", xhci_read32(HC_HCCPARAMS1));
}

#define MAX_FUZZ_ITERATIONS 15
#define MIN_FUZZ_ITERATIONS 10

size_t _kafl_fuzz(void* buf, size_t sz);

size_t
_kafl_fuzz(void* buf, size_t sz)
{
	if (!kafl_fuzz_buffer(buf, sz)) {
		xhci_write32(0x1340, 0x8888);
	}

	return 0;
}

void dumb_fuzzing(void);

void
dumb_fuzzing(void)
{
	uint32_t val = 0;
	//uint32_t n = 0;
	uint32_t offt = 0;


	//_kafl_fuzz(&n, sizeof(uint32_t));

	//kafl_hprintf("Before starting fuzzing\n");
    	
	
	xhci_write32(0x1448, 0x8888); // start fuzzing and acquire
    	
	
	//xhci_write32(0x1348, 0x8888); // start fuzzing and acquire
	//xhci_read32(offt);
    	//_kafl_fuzz(&offt, sizeof(uint32_t));
	//_kafl_fuzz(&val, sizeof(uint32_t));

	//xhci_write32(offt, val == 0x8888 ? val - 1 : val);

	//xhci_write32(0x0, 0x8888); // start fuzzing and acquire
	//xhci_write32(0x1340, 0x8888); // release 
	
	//kAFL_hypercall(HYPERCALL_KAFL_NESTED_ACQUIRE, 0);	
	int i = 0;
	//_kafl_fuzz(&offt, sizeof(uint32_t));	
	//_kafl_fuzz(&val, sizeof(uint32_t));
	//xhci_write32(offt, val);	
	//printk(KERN_INFO "test\n");
	//kafl_hprintf("Before the fuzzing loop\n");
	/*for (i = 0; i < 10; ++i) {
		kafl_hprintf("In the fuzzing loop");

		offt = (offt + 3) & ~0x3;
		

		_kafl_fuzz(&offt, sizeof(uint32_t));	
		offt = (offt + 3) & ~0x3;
		
		
	}*/
	
	//kafl_hprintf("Release\n");
    	//xhci_write32(0x1340, 0x8888); // release
	//kAFL_hypercall(HYPERCALL_KAFL_NESTED_EARLY_RELEASE, 0);
	//xhci_write32(0x1340, 0x8888);// release
}

enum urb_type {
    URB_BULK,
    URB_INTERRUPT,
    URB_CONTROL,
    URB_ISOCHRONOUS,
    URB_TYPE_MAX
};

static int xhci_fuzzer_probe(struct usb_interface *interface,
                            const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    
    printk(KERN_INFO "Found target USB device: %04x:%04x\n",
           le16_to_cpu(udev->descriptor.idVendor),
           le16_to_cpu(udev->descriptor.idProduct));
           
    target_device = udev;
    
    // Create a URB for fuzzing
    fuzz_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!fuzz_urb) {
        printk(KERN_ERR "Failed to allocate URB\n");
        return -ENOMEM;
    }
    
    // Set up a simple bulk URB
    usb_fill_bulk_urb(fuzz_urb, udev,
                     usb_rcvbulkpipe(udev, 0x81),
                     kmalloc(64, GFP_KERNEL), 64,
                     NULL, NULL);

    printk(KERN_INFO "probe initialized");

    return 0;
}

static void xhci_fuzzer_disconnect(struct usb_interface *interface)
{
    if (fuzz_urb) {
        usb_kill_urb(fuzz_urb);
        if (fuzz_urb->transfer_buffer)
            kfree(fuzz_urb->transfer_buffer);
        usb_free_urb(fuzz_urb);
        fuzz_urb = NULL;
    }
    
    target_device = NULL;
    printk(KERN_INFO "Target USB device disconnected\n");
}

// Add this to your module initialization before PCI operations
static void unbind_xhci_driver(void)
{
    struct pci_dev *pdev = NULL;
    
    // Find the xHCI controller
    pdev = pci_get_class(PCI_CLASS_SERIAL_USB_XHCI, NULL);
    if (!pdev) {
        printk(KERN_ERR "No xHCI controller found\n");
        return;
    }
    
    // Unbind the standard driver
    if (pdev->dev.driver) {
        printk(KERN_INFO "Unbinding xhci_hcd driver\n");
        device_release_driver(&pdev->dev);
    }
    
    pci_dev_put(pdev);
}

#include <linux/random.h>
#include <linux/bitops.h>
#include <linux/delay.h>

// Masks for writable bits (adjust per xHCI spec)
#define USBCMD_MASK     (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4)) // RS, HCRST, etc.
#define CRCR_MASK       0xFFFFFFFF  // Command Ring Control (full fuzz)
#define PORTSC_MASK     (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(9) | BIT(13)) // CCS, PR, PLS, etc.
#define IMAN_MASK       (BIT(0) | BIT(1))  // IE (Interrupt Enable), IP (Interrupt Pending)

/**
 * xhci_trigger_complex_failures() - Deliberately destabilize xHCI to find bugs.
 * @base: MMIO base address (from ioremap).
 *
 * Targets:
 * 1. Command Ring corruption
 * 2. Event Ring pointer races
 * 3. Interrupt storm generation
 * 4. Port state machine confusion
 */
void xhci_trigger_complex_failures(void __iomem *base) {
    // u32 orig_crcr, orig_erdp, orig_portsc, orig_iman;

    payload = _pre_xhci_fuzz();

    int i = 0;
    for (i = 0; i < 30; ++i) {
        xhci_write32(payload->target_reg, payload->fuzz_val);
        // mmiowb();
        //(void)xhci_read32(payload->target_reg);
        //_hci_feed(payload);
    }

    // // --- 1. Corrupt Command Ring Control Register (CRCR) ---
    // orig_crcr = xhci_read32(0x14);
    // xhci_write32(0x14, 0xDEADBEEF);  // Invalid pointer
    // mdelay(1);
    // xhci_write32(0x14, orig_crcr);   // Restore (optional)

    // // --- 2. Race Event Ring Dequeue Pointer (ERDP) updates ---
    // orig_erdp = xhci_read32(0x18);
    // for (int i = 0; i < 10; i++) {
    //     xhci_write32(0x18, orig_erdp + (i * 0x10));  // Rapidly change ERDP
    //     udelay(100);
    // }
    // xhci_write32(0x18, orig_erdp);

    // // --- 3. Force Interrupt Storm ---
    // orig_iman = xhci_read32(0x20);
    // xhci_write32(0x20, orig_iman | IMAN_MASK);  // Enable interrupts aggressively
    // for (int i = 0; i < 5; i++) {
    //     xhci_write32(0x20, orig_iman | IMAN_MASK);  // Spam writes
    //     udelay(50);
    // }

    // // --- 4. Confuse Port State Machine ---
    // orig_portsc = xhci_read32(0x400);  // PORTSC of port 0
    // xhci_write32(0x400, orig_portsc | PORTSC_MASK);  // Set all writable bits
    // mdelay(2);
    // xhci_write32(0x400, orig_portsc & ~PORTSC_MASK); // Clear all writable bits
    // mdelay(2);
    // xhci_write32(0x400, orig_portsc);  // Restore

    // // --- 5. Toggle Controller Run/Stop Rapidly ---
    // for (int i = 0; i < 3; i++) {
    //     xhci_write32(0x00, 0x0);  // Stop controller (RS=0)
    //     udelay(200);
    //     xhci_write32(0x00, 0x1);  // Start controller (RS=1)
    //     udelay(200);
    // }

    _post_xhci_fuzz(payload);
}

#include <linux/pci.h>       // For PCI functions (pci_iomap, pci_resource_start, etc.)
#include <linux/io.h>        // For I/O memory mapping functions


static int __init xhci_fuzzer_init(void)
{
    // int ret;
    struct pci_dev *pdev = NULL;

    //kafl_hprintf("test123\n");
    //kafl_agent_init();

    printk(KERN_INFO "After panic\n");
    unbind_xhci_driver();

    // Find xHCI controller
    while ((pdev = pci_get_class(PCI_CLASS_SERIAL_USB_XHCI, pdev))) {
        if (pci_enable_device(pdev)) {
            printk(KERN_ERR "Failed to enable PCI device\n");
            continue;
        }
        
        if (pci_request_regions(pdev, MODULE_NAME)) {
            printk(KERN_ERR "Failed to request PCI regions\n");
            pci_disable_device(pdev);
            continue;
        }
        
        xhci_mmio_base = pci_iomap(pdev, 0, 0);
        if (!xhci_mmio_base) {
            printk(KERN_ERR "Failed to map MMIO space\n");
            pci_release_regions(pdev);
            pci_disable_device(pdev);
            continue;
        }
        
        xhci_pci_dev = pdev;
        break;
    }
    
    if (!xhci_pci_dev) {
        printk(KERN_ERR "No xHCI controller found\n");
        return -ENODEV;
    }

    // Dump capability registers
    xhci_dump_cap_regs();

    // struct resource *res = &xhci_pci_dev->resource[0];  // BAR0
    // // void __iomem *xhci_base = xhci_mmio_base;
    // printk(KERN_INFO "Physical MMIO start: 0x%llx, size: 0x%llx\n",
    //    (u64)res->start, (u64)resource_size(res));
    // printk(KERN_INFO "xhci_base: %llx\n", (unsigned long long)*(unsigned long long* )xhci_base);

    // 1. Halt the controller (clear USBCMD.RS)
    // writel(0x0, xhci_base + 0x00);
    // for (size_t i = 0; i < 10*8; i += 8) {
    //     xhci_write32(HC_ + i, 0x1337);
    // }
    
    // xhci_read32(HC_CAPBASE);

    // // 2. Reset (set USBCMD.HCRST)
    // xhci_write32(HC_USBCMD, 0);

    //xhci_trigger_complex_failures(NULL);
    // while (readl(xhci_base + 0x00) & 0x2)  // Wait for reset
    //     cpu_relax();

    // // 3. Reinitialize (optional)
    // writel(0x1, xhci_base + 0x00);  // Set USBCMD.RS to start

    // Set up input context for command fuzzing
    // ret = setup_input_context();
    // if (ret) {
    //     pci_iounmap(xhci_pci_dev, xhci_mmio_base);
    //     pci_release_regions(xhci_pci_dev);
    //     pci_disable_device(xhci_pci_dev);
    //     return ret;
    // }

    // target_device = get_xhci_usb_device(xhci_pci_dev);

    // // Register USB driver
    // ret = usb_register(&xhci_fuzzer_driver);
    // if (ret) {
    //     printk(KERN_ERR "Failed to register USB driver: %d\n", ret);
    //     dma_free_coherent(&xhci_pci_dev->dev,
    //                      sizeof(struct xhci_input_context),
    //                      input_ctx, input_ctx_dma);
    //     pci_iounmap(xhci_pci_dev, xhci_mmio_base);
    //     pci_release_regions(xhci_pci_dev);
    //     pci_disable_device(xhci_pci_dev);
    //     return ret;
    // }
    
    // // Start fuzzing thread
    // perform_fuzzing();
    
    printk(KERN_INFO "xHCI fuzzer module loaded\n");

    /* initial fuzzer handshake */
    //kAFL_hypercall(HYPERCALL_KAFL_NESTED_ACQUIRE, 0);
    //kAFL_hypercall(HYPERCALL_KAFL_NESTED_RELEASE, 0);

    //phys_addr = pci_resource_start(pdev, 0);


    //kafl_agent_init((uint64_t)phys_addr);
    
    dumb_fuzzing();
    return 0;
}

static void __exit xhci_fuzzer_exit(void)
{
    usb_deregister(&xhci_fuzzer_driver);
    
    if (input_ctx) {
        dma_free_coherent(&xhci_pci_dev->dev,
                         sizeof(struct xhci_input_context),
                         input_ctx, input_ctx_dma);
    }
    
    if (xhci_mmio_base) {
        pci_iounmap(xhci_pci_dev, xhci_mmio_base);
    }
    
    if (xhci_pci_dev) {
        pci_release_regions(xhci_pci_dev);
        pci_disable_device(xhci_pci_dev);
    }
    
    printk(KERN_INFO "xHCI fuzzer module unloaded\n");
}

module_init(xhci_fuzzer_init);
module_exit(xhci_fuzzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("nasm");
MODULE_DESCRIPTION("xHCI Fuzzer for VirtualBox");
MODULE_VERSION("0.1");
