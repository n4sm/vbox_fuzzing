/*
 * kAFl/Nyx low-level interface definitions
 *
 * Copyright 2022 Sergej Schumilo, Cornelius Aschermann
 * Copyright 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef NYX_API_H
#define NYX_API_H

// #include <stdarg.h>
// #include <stddef.h>

#define HYPERCALL_KAFL_RAX_ID				0x01f
#define HYPERCALL_KAFL_ACQUIRE				0
#define HYPERCALL_KAFL_GET_PAYLOAD			1
#define HYPERCALL_KAFL_GET_PROGRAM			2  /* deprecated */
#define HYPERCALL_KAFL_GET_ARGV				3  /* deprecated */
#define HYPERCALL_KAFL_RELEASE				4
#define HYPERCALL_KAFL_SUBMIT_CR3			5
#define HYPERCALL_KAFL_SUBMIT_PANIC			6
#define HYPERCALL_KAFL_SUBMIT_KASAN			7
#define HYPERCALL_KAFL_PANIC				8
#define HYPERCALL_KAFL_KASAN				9
#define HYPERCALL_KAFL_LOCK					10 /* deprecated */
#define HYPERCALL_KAFL_INFO					11 /* deprecated */
#define HYPERCALL_KAFL_NEXT_PAYLOAD			12
#define HYPERCALL_KAFL_PRINTF				13
#define HYPERCALL_KAFL_PRINTK_ADDR			14 /* deprecated */
#define HYPERCALL_KAFL_PRINTK				15 /* deprecated */

/* user space only hypercalls */
#define HYPERCALL_KAFL_USER_RANGE_ADVISE	16
#define HYPERCALL_KAFL_USER_SUBMIT_MODE		17
#define HYPERCALL_KAFL_USER_FAST_ACQUIRE	18
/* 19 is already used for exit reason KVM_EXIT_KAFL_TOPA_MAIN_FULL */
#define HYPERCALL_KAFL_USER_ABORT			20
#define HYPERCALL_KAFL_TIMEOUT				21 /* deprecated */
#define HYPERCALL_KAFL_RANGE_SUBMIT		29
#define HYPERCALL_KAFL_REQ_STREAM_DATA		30
#define HYPERCALL_KAFL_PANIC_EXTENDED		32

#define HYPERCALL_KAFL_CREATE_TMP_SNAPSHOT	33
#define HYPERCALL_KAFL_DEBUG_TMP_SNAPSHOT	34 /* hypercall for debugging / development purposes */

#define HYPERCALL_KAFL_GET_HOST_CONFIG		35
#define HYPERCALL_KAFL_SET_AGENT_CONFIG		36

#define HYPERCALL_KAFL_DUMP_FILE			37

#define HYPERCALL_KAFL_REQ_STREAM_DATA_BULK 38
#define HYPERCALL_KAFL_PERSIST_PAGE_PAST_SNAPSHOT 39

/* hypertrash only hypercalls */
#define HYPERTRASH_HYPERCALL_MASK			0xAA000000

#define HYPERCALL_KAFL_NESTED_PREPARE		(0 | HYPERTRASH_HYPERCALL_MASK)
#define HYPERCALL_KAFL_NESTED_CONFIG		(1 | HYPERTRASH_HYPERCALL_MASK)
#define HYPERCALL_KAFL_NESTED_ACQUIRE		(2 | HYPERTRASH_HYPERCALL_MASK)
#define HYPERCALL_KAFL_NESTED_RELEASE		(3 | HYPERTRASH_HYPERCALL_MASK)
#define HYPERCALL_KAFL_NESTED_HPRINTF		(4 | HYPERTRASH_HYPERCALL_MASK)gre

#define PAYLOAD_MAX_SIZE			        (256 << 10)    /* up to 256KB payloads */
#define HPRINTF_MAX_SIZE					0x1000         /* up to 4KB hprintf saatrings */

// typedef union {
// 	struct {PAYLOAD_MAX_SIZE
// 		unsigned int dump_observed :1;
// 		unsigned int dump_stats :1;
// 		unsigned int dump_callers :1;
// 	};
// 	uint32_t raw_data;
// } __attribute__((packed)) agent_flags_t;

typedef struct {
	int32_t size;
	uint8_t data[];
} kAFL_payload;

typedef struct {
	uint64_t ip[4];
	uint64_t size[4];
	uint8_t enabled[4];
} kAFL_ranges;

#define KAFL_MODE_64	0
#define KAFL_MODE_32	1
#define KAFL_MODE_16	2

#if defined(__i386__)
static inline uint32_t kAFL_hypercall(uint32_t p1, uint32_t p2)
{
	uint32_t nr = HYPERCALL_KAFL_RAX_ID;
	asm volatile ("vmcall"
				  : "=a" (nr)
				  : "a"(nr), "b"(p1), "c"(p2));
	return nr;
}
#elif defined(__x86_64__)
static inline uint64_t kAFL_hypercall(uint64_t p1, uint64_t p2)
{
	uint64_t nr = HYPERCALL_KAFL_RAX_ID;
	asm volatile ("vmcall"
				  : "=a" (nr)
				  : "a"(nr), "b"(p1), "c"(p2));
	return nr;
}
#endif

#define NYX_HOST_MAGIC  0x4878794e
#define NYX_AGENT_MAGIC 0x4178794e

#define NYX_HOST_VERSION 1
#define NYX_AGENT_VERSION 1

typedef struct host_config_s {
	uint32_t host_magic;
	uint32_t host_version;
	uint32_t bitmap_size;
	uint32_t ijon_bitmap_size;
	uint32_t payload_buffer_size;
	uint32_t worker_id;
	/* more to come */
} __attribute__((packed)) host_config_t;

typedef struct agent_config_s {
	uint32_t agent_magic;
	uint32_t agent_version;
	uint8_t agent_timeout_detection;
	uint8_t agent_tracing;
	uint8_t agent_ijon_tracing;
	uint8_t agent_non_reload_mode;
	uint64_t trace_buffer_vaddr;
	uint64_t ijon_trace_buffer_vaddr;
	uint32_t coverage_bitmap_size;
	uint32_t input_buffer_size;
	uint8_t dump_payloads; /* set by hypervisor */
	/* more to come */
} __attribute__((packed)) agent_config_t;

typedef struct kafl_dump_file_s {
	uint64_t file_name_str_ptr;
	uint64_t data_ptr;
	uint64_t bytes;
	uint8_t append;
} __attribute__((packed)) kafl_dump_file_t;

#endif /* NYX_API_H */

// ==

/*
 * Copyright 2022 Intel Corporation
 * SPDX-License-Identifier: MIT
 */

#ifdef __KERNEL__

#include <linux/kernel.h>
#include <linux/string.h>

enum kafl_event {
	KAFL_ENABLE,
	KAFL_START,
	KAFL_ABORT,
	KAFL_SETCR3,
	KAFL_DONE,
	KAFL_PANIC,
	KAFL_KASAN,
	KAFL_UBSAN,
	KAFL_HALT,
	KAFL_REBOOT,
	KAFL_SAFE_HALT,
	KAFL_TIMEOUT,
	KAFL_ERROR,
	KAFL_PAUSE,
	KAFL_RESUME,
	KAFL_TRACE,
	KAFL_EVENT_MAX
};

void kafl_fuzz_event(enum kafl_event e);

void kafl_fuzz_function(char *fname);
void kafl_fuzz_function_disable(char *fname);

size_t kafl_fuzz_buffer(void* fuzz_buf, const size_t num_bytes);

int kafl_vprintk(const char *fmt, va_list args);
void kafl_hprintf(const char *fmt, ...) __attribute__ ((unused, format (printf, 1, 2)));

#endif /* NYX_API_H */

typedef struct xhci_payload_s {
	u16 target_reg;
	u32 fuzz_val;
} xhci_payload_t;

xhci_payload_t* _hci_feed(xhci_payload_t* payload);
int _post_xhci_fuzz(xhci_payload_t* payload);
xhci_payload_t* _pre_xhci_fuzz(void);

/*
 * kAFL agent for fuzzing xhci stack
 *
 * Dynamically initiate handshake + snapshot upon first use
 */

#include <linux/kernel.h>
#include <linux/random.h>
#include <linux/percpu.h>
#include <linux/debugfs.h>
#include <linux/smp.h>
#include <linux/slab.h>
#include <linux/memblock.h>
#include <linux/kprobes.h>
#include <linux/string.h>
#include <asm-generic/sections.h>

#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/of_device.h>
#include <linux/of_pci.h>
#include <linux/pci_hotplug.h>

// #include <asm/kafl-agent.h>

#undef pr_fmt
#define pr_fmt(fmt) "kAFL: " fmt

#define PAYLOAD_SCTP_SIZE 0x2000

bool agent_initialized = false;
bool fuzz_tderror = false; // TDX error fuzzing not supported

/* abort at end of payload - otherwise we keep feeding unmodified input
 * which means we see coverage that is not represented in the payload */
bool exit_at_eof = true;

agent_config_t agent_config = {0};
host_config_t host_config = {0};

char hprintf_buffer[HPRINTF_MAX_SIZE] __attribute__((aligned(4096)));
kafl_dump_file_t dump_file __attribute__((aligned(4096)));

/* kmalloc() may not always be available - e.g. early boot */
//#define KAFL_ASSUME_KMALLOC
#ifdef KAFL_ASSUME_KMALLOC
size_t payload_buffer_size = 0;
uint8_t *payload_buffer = NULL;
#else
size_t payload_buffer_size = PAYLOAD_MAX_SIZE;
uint8_t payload_buffer[PAYLOAD_MAX_SIZE] __attribute__((aligned(4096)));
#endif

u8 *ve_buf;
u32 ve_num;
u32 ve_pos;
u32 ve_mis;

u8 *ob_buf;
u32 ob_num;
u32 ob_pos;

const char *kafl_event_name[KAFL_EVENT_MAX] = {
	"KAFL_ENABLE",
	"KAFL_START",
	"KAFL_ABORT",
	"KAFL_SETCR3",
	"KAFL_DONE",
	"KAFL_PANIC",
	"KAFL_KASAN",
	"KAFL_UBSAN",
	"KAFL_HALT",
	"KAFL_REBOOT",
	"KAFL_SAFE_HALT",
	"KAFL_TIMEOUT",
	"KAFL_ERROR",
	"KAFL_PAUSE",
	"KAFL_RESUME",
	"KAFL_TRACE",
};

static void kafl_raise_panic(void) {
	kAFL_hypercall(HYPERCALL_KAFL_PANIC, 0);
}

static void kafl_raise_kasan(void) {
	kAFL_hypercall(HYPERCALL_KAFL_KASAN, 0);
}

static void kafl_acquire(void) {
	kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);
}


static void kafl_agent_setrange(int id, void* start, void* end)
{
	uintptr_t range[3];
	range[0] = (uintptr_t)start & PAGE_MASK;
	range[1] = ((uintptr_t)end + PAGE_SIZE-1) & PAGE_MASK;
	range[2] = id;

	kafl_hprintf("Setting range %lu: %lx-%lx\n", range[2], range[0], range[1]);
	kAFL_hypercall(HYPERCALL_KAFL_RANGE_SUBMIT, (uintptr_t)range);
}

static void kafl_habort(char *msg)
{
	kAFL_hypercall(HYPERCALL_KAFL_USER_ABORT, (uintptr_t)msg);
}

// dedicated assert for raising kAFL/harness level issues
#define kafl_assert(exp) \
	do { \
		if (!(exp)) { \
			kafl_hprintf("kAFL ASSERT at %s:%d, %s\n", __FILE__, __LINE__, #exp); \
			kafl_habort("assertion fail (see hprintf logs)"); \
		} \
		BUG_ON(!(exp)); \
	} while (0)

void kafl_hprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf((char*)hprintf_buffer, HPRINTF_MAX_SIZE, fmt, args);
	kAFL_hypercall(HYPERCALL_KAFL_PRINTF, (uintptr_t)hprintf_buffer);
	va_end(args);
}

static void kafl_agent_setcr3(void)
{
	pr_debug("Submitting current CR3 value to hypervisor...\n");
	kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_CR3, 0);
}

static void kafl_agent_init(void)
{
	kAFL_payload *payload = NULL;

	if (agent_initialized) {
		kafl_habort("Warning: Agent was already initialized!\n");
	}

	kafl_hprintf("[*] Initialize kAFL Agent\n");

	/* initial fuzzer handshake */
	kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);
	kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);

	/* used for code injection and libxdc disassembly */
#if defined(__i386__)
	kAFL_hypercall(HYPERCALL_KAFL_USER_SUBMIT_MODE, KAFL_MODE_32);
#elif defined(__x86_64__)
	kAFL_hypercall(HYPERCALL_KAFL_USER_SUBMIT_MODE, KAFL_MODE_64);
#endif


	kAFL_hypercall(HYPERCALL_KAFL_GET_HOST_CONFIG, (uintptr_t)&host_config);

	kafl_hprintf("[host_config] bitmap sizes = <0x%x,0x%x>\n", host_config.bitmap_size, host_config.ijon_bitmap_size);
	kafl_hprintf("[host_config] payload size = %dKB\n", host_config.payload_buffer_size/1024);
	kafl_hprintf("[host_config] worker id = %02u\n", host_config.worker_id);

#ifdef KAFL_ASSUME_KMALLOC
	payload_buffer_size = host_config.payload_buffer_size;
	payload_buffer = kmalloc(payload_buffer_size, GFP_KERNEL|__GFP_NOFAIL);

	if (!payload_buffer) {
		kafl_habort("Failed to allocate host payload buffer!\n");
	}
#else
	if (host_config.payload_buffer_size > PAYLOAD_MAX_SIZE) {
		kafl_habort("Insufficient payload buffer size!\n");
	}
#endif

	/* ensure that the virtual memory is *really* present in physical memory... */
	memset(payload_buffer, 0xff, payload_buffer_size);

	kafl_hprintf("Submitting payload buffer address to hypervisor (%lx)\n", (uintptr_t)payload_buffer);
	kAFL_hypercall(HYPERCALL_KAFL_GET_PAYLOAD, (uintptr_t)payload_buffer);

	agent_config.agent_magic = NYX_AGENT_MAGIC;
	agent_config.agent_version = NYX_AGENT_VERSION;
	agent_config.agent_timeout_detection = 0;
	agent_config.agent_tracing = 0;
	agent_config.agent_ijon_tracing = 0;
	agent_config.agent_non_reload_mode = 0;
	agent_config.trace_buffer_vaddr = 0;
	agent_config.ijon_trace_buffer_vaddr = 0;
	//agent_config.coverage_bitmap_size = host_config.bitmap_size;
	agent_config.input_buffer_size = 0;
	agent_config.dump_payloads = 0;
	kAFL_hypercall(HYPERCALL_KAFL_SET_AGENT_CONFIG, (uintptr_t)&agent_config);

	// set PT filter ranges based on exported linker map symbols in sections.h
	kafl_agent_setrange(0, (void* )0xffffffff81000000, (void* )0xffffffff841fcc47);
	kafl_agent_setrange(1, (void* )0xffffffff85ef6000, (void* )0xffffffff86026a45);

	// fetch fuzz input for later #VE injection
	kafl_hprintf("Starting kAFL loop...\n");
	kAFL_hypercall(HYPERCALL_KAFL_NEXT_PAYLOAD, 0);

	payload = (kAFL_payload*)payload_buffer;
	ve_buf = payload->data;
	ve_num = payload->size;
	ve_pos = 0;
	ve_mis = 0;

    kafl_fuzz_event(KAFL_SETCR3);

	agent_initialized = true;

	// // start coverage tracing
	kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0); 
}


static void kafl_agent_done(void)
{
	if (!agent_initialized) {
		kafl_habort("Attempt to finish kAFL run but never initialized\n");
	}

	// Stops tracing and restore the snapshot for next round
	// Non-zero argument triggers stream_expand mutation in kAFL
	kAFL_hypercall(HYPERCALL_KAFL_RELEASE, ve_mis*sizeof(ve_buf[0]));
}

static size_t _kafl_fuzz_buffer(void *buf, size_t num_bytes)
{
	if (ve_pos + num_bytes <= ve_num) {
		memcpy(buf, ve_buf + ve_pos, num_bytes);
		ve_pos += num_bytes;
		return num_bytes;
	}

	// insufficient fuzz buffer!
	ve_mis += num_bytes;
	if (exit_at_eof) {
		kafl_agent_done(); /* no return */
	}
	return 0;
}

/*
 * Fuzz target buffer `fuzz_buf` depending on input and add/type filter settings
 * Returns number of bytes actually fuzzed (typically all or nothing)
 */
size_t kafl_fuzz_buffer(void* fuzz_buf, const size_t num_bytes)
{
	size_t num_fuzzed = 0;

	/*
	 * If agent was set to 'enable' only, perform init + snapshot
	 * here at last possible moment before first injection
	 *
	 * Note: If the harness/config does not actually consume any
	 * input, the fuzzer frontend will wait forever on this..
	 */
	if (!agent_initialized) {
		kafl_agent_init();
	}

	num_fuzzed = _kafl_fuzz_buffer(fuzz_buf, num_bytes);

	return num_fuzzed;
}
EXPORT_SYMBOL(kafl_fuzz_buffer);

xhci_payload_t* _pre_xhci_fuzz(void)
{
    // if (!agent_initialized) {
    //     kafl_agent_init();
    // } It's better to initialize the agent when the kafl_fuzz_buffer occurs for the firs time 

    xhci_payload_t* payload = kmalloc(sizeof(xhci_payload_t), GFP_KERNEL | __GFP_NOFAIL);
    // payload->kopt = kmalloc(PAYLOAD_SCTP_SIZE, GFP_KERNEL | __GFP_NOFAIL);

    if (!kafl_fuzz_buffer(&payload->target_reg, sizeof(payload->target_reg))) {
        kafl_hprintf("Cannot get data for &payload->req\n");
        kafl_agent_done();
    } else if (!kafl_fuzz_buffer(&payload->fuzz_val, sizeof(payload->fuzz_val))) {
        kafl_hprintf("Cannot get data for &payload->size\n");
        kafl_agent_done();
    } 
    // else if (!kafl_fuzz_buffer(payload->kopt, PAYLOAD_SCTP_SIZE)) {
    //     kafl_hprintf("Cannot get data for &payload->kopt\n");
    //     kafl_agent_done();
    // }

    // valid size
    // if (payload->size <= 0 || payload->size > PAYLOAD_MAX_SIZE) {
    //     kafl_agent_done();
    // }

    // // valid sockopt request
    // payload->req %= 37;

    return payload;
}

xhci_payload_t* _hci_feed(xhci_payload_t* payload)
{
    // if (!agent_initialized) {
    //     kafl_agent_init();
    // } It's better to initialize the agent when the kafl_fuzz_buffer occurs for the firs time 
    // payload->kopt = kmalloc(PAYLOAD_SCTP_SIZE, GFP_KERNEL | __GFP_NOFAIL);

    if (!kafl_fuzz_buffer(&payload->target_reg, sizeof(payload->target_reg))) {
        kafl_hprintf("Cannot get data for &payload->req\n");
        kafl_agent_done();
    } else if (!kafl_fuzz_buffer(&payload->fuzz_val, sizeof(payload->fuzz_val))) {
        kafl_hprintf("Cannot get data for &payload->size\n");
        kafl_agent_done();
    } 
    // else if (!kafl_fuzz_buffer(payload->kopt, PAYLOAD_SCTP_SIZE)) {
    //     kafl_hprintf("Cannot get data for &payload->kopt\n");
    //     kafl_agent_done();
    // }

    // valid size
    // if (payload->size <= 0 || payload->size > PAYLOAD_MAX_SIZE) {
    //     kafl_agent_done();
    // }

    // // valid sockopt request
    // payload->req %= 37;

    return payload;
}

int _post_xhci_fuzz(xhci_payload_t* payload)
{
    // if (!agent_initialized) {
    //     kafl_agent_init();
    // } It's better to initialize the agent when the kafl_fuzz_buffer occurs for the firs time 

	if (payload) {
		// kfree(payload->kopt);
		kfree(payload);
	}

    kafl_agent_done();

    return 0; // no return
}

void kafl_fuzz_event(enum kafl_event e)
{
	// pre-init actions
	switch (e) {
		case KAFL_START:
			pr_warn("[*] Agent start!\n");
			kafl_agent_init();
			return;
		case KAFL_ENABLE:
			pr_warn("[*] Agent enable!\n");
			fallthrough;
		case KAFL_RESUME:
            kafl_acquire();
			return;
		case KAFL_DONE:
			return kafl_agent_done();
		case KAFL_ABORT:
			return kafl_habort("kAFL got ABORT event.\n");
		case KAFL_SETCR3:
			return kafl_agent_setcr3();
		case KAFL_SAFE_HALT:
			// seems like a benign implementation of once in userspace, nohz_idle() constantly calls this to halt()
			return;
		default:
			break;
	}

	if (!agent_initialized) {
		pr_alert("Got event %s but not initialized?!\n", kafl_event_name[e]);
		//dump_stack();
		return;
	}

	// post-init actions - abort if we see these before fuzz_initialized=true
	// Use this table to selectively raise error conditions
	switch (e) {
		case KAFL_KASAN:
		case KAFL_UBSAN:
			return kafl_raise_kasan();
		case KAFL_PANIC:
		case KAFL_ERROR:
		case KAFL_HALT:
		case KAFL_REBOOT:
			return kafl_raise_panic();
		case KAFL_TIMEOUT:
			return kafl_habort("TODO: add a timeout handler?!\n");
		default:
			return kafl_habort("Unrecognized fuzz event.\n");
	}
}
EXPORT_SYMBOL(kafl_fuzz_event);

/*
 * Set verbosity of kernel to hprintf logging
 * Beyond early boot, this can be set using hprintf= cmdline
 */
//static int vprintk_level = KERN_DEBUG[1];
// static int vprintk_level = KERN_WARNING[1];
// //static int vprintk_level = '0'; // mute

// static int __init kafl_vprintk_setup(char *str)
// {
// 	if (str[0] >= '0' && str[0] <= '9') {
// 		vprintk_level = str[0];
// 		//hprintf("hprintf_setup: %x => %d (%d)\n", str[0], vprintk_level, vprintk_level-'0');
// 	}

// 	return 1;
// }
// __setup("hprintf=", kafl_vprintk_setup);

// /*
//  * Redirect kernel printk() to hprintf
//  */
// int kafl_vprintk(const char *fmt, va_list args)
// {
// 	static int last_msg_level = 0;

// 	char *buf;

// 	if (vprintk_level == '0')
// 		return 0; // mute printk - kafl_hprintf() still works

// 	// some callers give level as arg..
// 	vscnprintf((char*)hprintf_buffer, HPRINTF_MAX_SIZE, fmt, args);
// 	buf = hprintf_buffer;

// 	if (buf[0] != KERN_SOH[0]) {
// 		kAFL_hypercall(HYPERCALL_KAFL_PRINTF, (uintptr_t)hprintf_buffer);
// 		return 0;
// 	}

// 	if (buf[1] == KERN_CONT[1]) {
// 		if (last_msg_level <= vprintk_level) {
// 			kAFL_hypercall(HYPERCALL_KAFL_PRINTF, (uintptr_t)buf+2);
// 		}
// 		return 0;
// 	}

// 	last_msg_level = buf[1];
// 	if (buf[1] <= vprintk_level) {
// 		kAFL_hypercall(HYPERCALL_KAFL_PRINTF, (uintptr_t)buf+2);
// 	}

// 	return 0;
// }