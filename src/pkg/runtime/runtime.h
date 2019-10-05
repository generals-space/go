// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/*
 * basic types
 */
typedef	signed char		int8;
typedef	unsigned char		uint8;
typedef	signed short		int16;
typedef	unsigned short		uint16;
typedef	signed int		int32;
typedef	unsigned int		uint32;
typedef	signed long long int	int64;
typedef	unsigned long long int	uint64;
typedef	float			float32;
typedef	double			float64;

#ifdef _64BIT
typedef	uint64		uintptr;
typedef	int64		intptr;
typedef	int64		intgo; // Go's int
typedef	uint64		uintgo; // Go's uint
#else
typedef	uint32		uintptr;
typedef	int32		intptr;
typedef	int32		intgo; // Go's int
typedef	uint32		uintgo; // Go's uint
#endif

/*
 * get rid of C types
 * the / / / forces a syntax error immediately,
 * which will show "last name: XXunsigned".
 */
#define	unsigned		XXunsigned / / /
#define	signed			XXsigned / / /
#define	char			XXchar / / /
#define	short			XXshort / / /
#define	int			XXint / / /
#define	long			XXlong / / /
#define	float			XXfloat / / /
#define	double			XXdouble / / /

/*
 * defined types
 */
typedef	uint8			bool;
typedef	uint8			byte;
typedef	struct	Func		Func;
typedef	struct	G		G;
typedef	struct	Gobuf		Gobuf;
typedef	struct	Lock		Lock;
typedef	struct	M		M;
typedef	struct	P		P;
typedef	struct	Note		Note;
typedef	struct	Slice		Slice;
typedef	struct	Stktop		Stktop;
typedef	struct	String		String;
typedef	struct	FuncVal		FuncVal;
typedef	struct	SigTab		SigTab;
typedef	struct	MCache		MCache;
typedef	struct	FixAlloc	FixAlloc;
typedef	struct	Iface		Iface;
typedef	struct	Itab		Itab;
typedef	struct	InterfaceType	InterfaceType;
typedef	struct	Eface		Eface;
typedef	struct	Type		Type;
typedef	struct	ChanType		ChanType;
typedef	struct	MapType		MapType;
typedef	struct	Defer		Defer;
typedef	struct	DeferChunk	DeferChunk;
typedef	struct	Panic		Panic;
typedef	struct	Hmap		Hmap;
typedef	struct	Hchan		Hchan;
typedef	struct	Complex64	Complex64;
typedef	struct	Complex128	Complex128;
typedef	struct	WinCall		WinCall;
typedef	struct	SEH		SEH;
typedef	struct	WinCallbackContext	WinCallbackContext;
typedef	struct	Timers		Timers;
typedef	struct	Timer		Timer;
typedef	struct	GCStats		GCStats;
typedef	struct	LFNode		LFNode;
typedef	struct	ParFor		ParFor;
typedef	struct	ParForThread	ParForThread;
typedef	struct	CgoMal		CgoMal;
typedef	struct	PollDesc	PollDesc;
typedef	struct	DebugVars	DebugVars;

/*
 * Per-CPU declaration.
 *
 * "extern register" is a special storage class implemented by 6c, 8c, etc.
 * register 寄存器?
 * On the ARM, it is an actual register; 
 * elsewhere it is a slot in thread-local storage indexed by a segment register. 
 * See zasmhdr in src/cmd/dist/buildruntime.c for details, 
 * and be aware that the linker may make further OS-specific changes to the compiler's output. 
 * For example, 6l/linux rewrites 0(GS) as -16(FS).
 *
 * Every C file linked into a Go program must include runtime.h 
 * so that the C compiler (6c, 8c, etc.) knows to avoid other uses of these dedicated registers. 
 * The Go compiler (6g, 8g, etc.) knows to avoid them.
 */
extern	register	G*	g;
extern	register	M*	m;

/*
 * defined constants
 */
enum
{
	// G status
	//
	// If you add to this list, add to the list
	// of "okay during garbage collection" status
	// in mgc0.c too.
	Gidle,
	Grunnable,
	Grunning,
	Gsyscall,
	Gwaiting,
	Gmoribund_unused,  // currently unused, but hardcoded in gdb scripts
	Gdead,
};
enum
{
	// P status
	Pidle,
	Prunning,
	Psyscall,
	Pgcstop, // runtime·stoptheworld()的时候会将正在运行p设置为这个状态.
	Pdead,
};
enum
{
	true	= 1,
	false	= 0,
};
enum
{
	PtrSize = sizeof(void*),
};
enum
{
	// Per-M stack segment cache size.
	StackCacheSize = 32,
	// Global <-> per-M stack segment cache transfer batch size.
	StackCacheBatch = 16,
};
/*
 * structures
 */
struct	Lock
{
	// Futex-based impl treats it as uint32 key,
	// while sema-based impl as M* waitm.
	// Used to be a union, but unions break precise GC.
	// 基于futex的实现将key看作uint32的key, 
	// 而基于信号量的实现会将其看作M* waitm (...这tm什么玩意?)
	// 之前是union联合对象, 但是unions会打断精准GC (性能问题吧?)
	uintptr	key;
};
struct	Note
{
	// Futex-based impl treats it as uint32 key,
	// while sema-based impl as M* waitm.
	// Used to be a union, but unions break precise GC.
	uintptr	key;
};
struct String
{
	byte*	str;
	intgo	len;
};
struct FuncVal
{
	void	(*fn)(void);
	// variable-size, fn-specific data here
};
struct Iface
{
	Itab*	tab;
	void*	data;
};
struct Eface
{
	Type*	type;
	void*	data;
};
struct Complex64
{
	float32	real;
	float32	imag;
};
struct Complex128
{
	float64	real;
	float64	imag;
};

// must not move anything
struct	Slice
{
	byte*	array;		// actual data
	uintgo	len;		// number of elements
	uintgo	cap;		// allocated number of elements
};
struct	Gobuf
{
	// The offsets of sp, pc, and g are known to (hard-coded in) libmach.
	// sp, pc和g字段的偏移量是编译器可预见的.
	uintptr	sp;
	uintptr	pc;
	// g成员指向 gobuf 对象所属的g对象, 相当于反向引用.
	G*	g;
	uintptr	ret;
	void*	ctxt;
	uintptr	lr;
};
struct	GCStats
{
	// the struct must consist of only uint64's,
	// because it is casted to uint64[].
	uint64	nhandoff;
	uint64	nhandoffcnt;
	uint64	nprocyield;
	uint64	nosyield;
	uint64	nsleep;
};

struct	WinCall
{
	void	(*fn)(void*);
	uintptr	n;	// number of parameters
	void*	args;	// parameters
	uintptr	r1;	// return values
	uintptr	r2;
	uintptr	err;	// error number
};
struct	SEH
{
	void*	prev;
	void*	handler;
};
// describes how to handle callback
struct	WinCallbackContext
{
	void*	gobody;		// Go function to call
	uintptr	argsize;	// callback arguments size (in bytes)
	uintptr	restorestack;	// adjust stack on return by (in bytes) (386 only)
};

struct	G
{
	// stackguard0 can be set to StackPreempt as opposed to stackguard
	// cannot move - also known to linker, libmach, runtime/cgo
	// stackguard0 目前发现有两个可能值: stackguard, 或 StackPreempt
	// 被设置为 StackPreempt 时, 表示当前g对象被抢占(对比 preempt 字段)
	// 唯一一次有效使用在 stack.c -> runtime·newstack() 函数中.
	uintptr	stackguard0;
	// cannot move - also known to libmach, runtime/cgo
	// 当前g对象栈空间顶部top部分的起始地址.
	uintptr	stackbase;
	uint32	panicwrap;	// cannot move - also known to linker
	uint32	selgen;		// valid sudog pointer
	Defer*	defer;
	Panic*	panic;
	Gobuf	sched;
	uintptr	syscallstack;	// if status==Gsyscall, syscallstack = stackbase to use during gc
	uintptr	syscallsp;	// if status==Gsyscall, syscallsp = sched.sp to use during gc
	uintptr	syscallpc;	// if status==Gsyscall, syscallpc = sched.pc to use during gc
	uintptr	syscallguard;	// if status==Gsyscall, syscallguard = stackguard to use during gc
	// same as stackguard0, but not set to StackPreempt
	uintptr	stackguard;
	// 当前g对象栈空间的起始地址
	uintptr	stack0;
	// 当前g线程的栈大小, 不可超过 runtime·maxstacksize
	uintptr	stacksize;
	// on allg
	// ...这个指向 runtime·allg 变量吧
	G*	alllink;
	void*	param;		// passed parameter on wakeup
	int16	status;
	int64	goid;
	int8*	waitreason;	// if status==Gwaiting
	// 貌似这个链表指针指向下一个待调度执行的g对象.
	G*	schedlink; 
	bool	ispanic;
	bool	issystem;	// do not output in stack dump
	// ignore in deadlock detector
	// 如下g处于 isbackground 状态, 将不受死锁检测
	bool	isbackground;
	// preemption signal, duplicates stackguard0 = StackPreempt
	// 如果当前协程被抢占(如调用 preemptone()时), 则这个字段为true, 
	// 同时 stackguard0 字段也会被设置为 StackPreempt
	bool	preempt;
	int8	raceignore;	// ignore race detection events
	M*	m;		// for debuggers, but offset not hard-coded
	M*	lockedm;
	int32	sig;
	int32	writenbuf;
	byte*	writebuf;
	DeferChunk*	dchunk;
	DeferChunk*	dchunknext;
	uintptr	sigcode0;
	uintptr	sigcode1;
	uintptr	sigpc;
	// pc of go statement that created this goroutine
	// 创建此g对象的go()语句的pc(程序计数器)
	// 或者说是主调函数的函数地址(不是调用处的地址), 
	// 只在 proc.c -> runtime·newproc1() 函数中被赋值.
	uintptr	gopc;
	uintptr	racectx;
	uintptr	end[];
};
struct	M
{
	// goroutine with scheduling stack
	G*	g0;
	void*	moreargp;	// argument pointer for more stack
	// gobuf arg to morestack
	// morebuf 应该等于 curg->sched, 表示当前运行的g对象的 Gobuf
	Gobuf	morebuf;	

	// Fields not known to debuggers.
	uint32	moreframesize;	// size arguments to morestack
	uint32	moreargsize;
	uintptr	cret;		// return value from C
	uint64	procid;		// for debuggers, but offset not hard-coded
	G*	gsignal;	// signal-handling G
	uintptr	tls[4];		// thread-local storage (for x86 extern register)
	void	(*mstartfn)(void);
	// current running goroutine
	// 当前正在运行的g对象
	G*	curg;
	G*	caughtsig;	// goroutine running during fatal signal
	// attached P for executing Go code (nil if not executing Go code)
	// 这里应该是单个p对象, 一个m只绑定一个p
	P*	p;
	// 这是下一个要切换的p? 
	P*	nextp;
	int32	id;
	// 话说 G和P都有各自的枚举状态, M就没有.
	// 下面这些貌似是可以重合的, 所以不能只用一个 status 字段来表示???
	// 类似于锁的变量, 1表示此m对象正在分配内存, 0表示分配完成.
	int32	mallocing; 
	int32	throwing;
	int32	gcing;
	// 对 m->locks 的++和--操作总是成对出现, 
	// 偶尔在--操作后, 会对 locks 的值与0做一下判断, 不可以小于0(会抛出异常).
	// 加锁的情况并没有特别的规定, 貌似 gc, 系统调用, 或是 futex 锁
	// 都会加减 locks 值.
	// 但至少在自减后等于0时, 可以认为没有其他情况占用 locks,
	// 所以与0值的比较一般会伴随着对g的 preempt 字段的判断.
	// 也就是说, 在处于对 locks 加锁的这些操作中时, g是不可以被抢占的.
	// locks 貌似就是用来检测是否可以设置 preempt 的.
	int32	locks;
	int32	dying;
	int32	profilehz;
	int32	helpgc;
	bool	spinning;
	uint32	fastrand;
	uint64	ncgocall;	// number of cgo calls in total
	int32	ncgo;		// number of cgo calls currently in progress
	CgoMal*	cgomal;
	// m 休眠|等待的地址.
	// 在 stoplockedm(), stopm() 两处被调用 notesleep().
	Note	park;
	M*	alllink;	// on allm
	M*	schedlink;
	uint32	machport;	// Return address for Mach IPC (OS X)
	MCache*	mcache;
	
	// stackcache 数组中已使用的数量
	int32	stackinuse;
	// 下一个可用的 stackcache 成员索引, 
	// 不会超过 StackCacheSize
	uint32	stackcachepos;
	// stackcache 中可用成员的数量
	uint32	stackcachecnt;
	void*	stackcache[StackCacheSize];

	G*	lockedg;
	uintptr	createstack[32];// Stack that created this thread.
	uint32	freglo[16];	// D[i] lsb and F[i]
	uint32	freghi[16];	// D[i] msb and F[i+16]
	uint32	fflag;		// floating point compare flags
	uint32	locked;		// tracking for LockOSThread
	M*	nextwaitm;	// next M waiting for lock
	uintptr	waitsema;	// semaphore for parking on locks
	uint32	waitsemacount;
	uint32	waitsemalock;
	GCStats	gcstats;
	bool	racecall;
	bool	needextram;
	void	(*waitunlockf)(Lock*);
	void*	waitlock;

	uintptr	settype_buf[1024];
	uintptr	settype_bufsize;

#ifdef GOOS_windows
	void*	thread;		// thread handle
	WinCall	wincall;
#endif
#ifdef GOOS_plan9
	int8*	notesig;
	byte*	errstr;
#endif
	SEH*	seh;
	uintptr	end[];
};

struct P
{
	Lock;

	int32	id;
	uint32	status;		// one of Pidle/Prunning/...
	// 这是一个指向下一个p的指针, 组成链表.
	P*	link;
	// incremented on every scheduler call
	// 每次被调度器调度时加1, execute()
	uint32	schedtick;
	// incremented on every system call
	// 每次系统调用(完成)时加1, runtime·exitsyscall()
	uint32	syscalltick;
	// back-link to associated M (nil if idle)
	// 反向引用绑定的m对象, 如果p处于 idle状态则为nil
	M*	m;
	MCache*	mcache;

	// Queue of runnable goroutines.
	G**	runq;
	int32	runqhead;
	int32	runqtail;
	int32	runqsize;

	// Available G's (status == Gdead)
	G*	gfree;
	int32	gfreecnt;

	byte	pad[64];
};

// The m->locked word holds two pieces of state counting active calls to LockOSThread/lockOSThread.
// The low bit (LockExternal) is a boolean reporting whether any LockOSThread call is active.
// External locks are not recursive; a second lock is silently ignored.
// The upper bits of m->lockedcount record the nesting depth of calls to lockOSThread
// (counting up by LockInternal), 
// popped by unlockOSThread (counting down by LockInternal).
// Internal locks can be recursive. 
// For instance, a lock for cgo can occur while the main
// goroutine is holding the lock during the initialization phase.
// 
enum
{
	LockExternal = 1,
	LockInternal = 2,
};

struct	Stktop
{
	// The offsets of these fields are known to (hard-coded in) libmach.
	// 这些成员的偏移量是编译器可预见的.
	uintptr	stackguard;
	uintptr	stackbase;
	Gobuf	gobuf;
	uint32	argsize;
	uint32	panicwrap;

	uint8*	argp;	// pointer to arguments in old frame
	uintptr	free;	// if free>0, call stackfree using free as size
	bool	panic;	// is this frame the top of a panic?
};
struct	SigTab
{
	int32	flags;
	int8	*name;
};
enum
{
	SigNotify = 1<<0,	// let signal.Notify have signal, even if from kernel
	SigKill = 1<<1,		// if signal.Notify doesn't take it, exit quietly
	SigThrow = 1<<2,	// if signal.Notify doesn't take it, exit loudly
	SigPanic = 1<<3,	// if the signal is from the kernel, panic
	SigDefault = 1<<4,	// if the signal isn't explicitly requested, don't monitor it
	SigHandling = 1<<5,	// our signal handler is registered
	SigIgnored = 1<<6,	// the signal was ignored before we registered for it
};

// Layout of in-memory per-function information prepared by linker
// See http://golang.org/s/go12symtab.
// Keep in sync with linker and with ../../libmach/sym.c
// and with package debug/gosym.
struct	Func
{
	// start pc
	// 入口地址, 一般与 pc 变量进行运算.
	uintptr	entry;
	// function name
	// 函数名称, 其实是距 pclntab(在symtab.c中定义) 的偏移量
	// ...匿名函数的话, 这个值会不会是0???
	int32	nameoff;
	
	int32	args;	// in/out args size
	int32	frame;	// legacy frame size; use pcsp if possible

	int32	pcsp;
	int32	pcfile;
	int32	pcln;
	int32	npcdata;
	int32	nfuncdata;
};

// layout of Itab known to compilers
// allocated in non-garbage-collected memory
struct	Itab
{
	InterfaceType*	inter;
	Type*	type;
	Itab*	link;
	int32	bad;
	int32	unused;
	void	(*fun[])(void);
};

#ifdef GOOS_windows
enum {
   Windows = 1
};
#else
enum {
   Windows = 0
};
#endif

struct	Timers
{
	Lock;
	G	*timerproc;
	bool		sleeping;
	bool		rescheduling;
	Note	waitnote;
	Timer	**t;
	int32	len;
	int32	cap;
};

// Package time knows the layout of this structure.
// If this struct changes, adjust ../time/sleep.go:/runtimeTimer.
struct	Timer
{
	int32	i;	// heap index

	// Timer wakes up at when, and then at when+period, ... (period > 0 only)
	// each time calling f(now, arg) in the timer goroutine, so f must be
	// a well-behaved function and not block.
	int64	when;
	int64	period;
	FuncVal	*fv;
	Eface	arg;
};

// Lock-free stack node.
struct LFNode
{
	LFNode	*next;
	uintptr	pushcnt;
};

// Parallel for descriptor.
struct ParFor
{
	void (*body)(ParFor*, uint32);	// executed for each element
	uint32 done;			// number of idle threads
	uint32 nthr;			// total number of threads
	uint32 nthrmax;			// maximum number of threads
	uint32 thrseq;			// thread id sequencer
	uint32 cnt;			// iteration space [0, cnt)
	void *ctx;			// arbitrary user context
	bool wait;			// if true, wait while all threads finish processing,
					// otherwise parfor may return while other threads are still working
	ParForThread *thr;		// array of thread descriptors
	uint32 pad;			// to align ParForThread.pos for 64-bit atomic operations
	// stats
	uint64 nsteal;
	uint64 nstealcnt;
	uint64 nprocyield;
	uint64 nosyield;
	uint64 nsleep;
};

// Track memory allocated by code not written in Go during a cgo call,
// so that the garbage collector can see them.
struct CgoMal
{
	CgoMal	*next;
	void	*alloc;
};

// Holds variables parsed from GODEBUG env var.
struct DebugVars
{
	int32	gctrace;
	int32	schedtrace;
	int32	scheddetail;
};

extern bool runtime·precisestack;

/*
 * defined macros
 *    you need super-gopher-guru privilege
 *    to add this list.
 */
#define	nelem(x)	(sizeof(x)/sizeof((x)[0]))
#define	nil		((void*)0)
#define	offsetof(s,m)	(uint32)(&(((s*)0)->m))
#define	ROUND(x, n)	(((x)+(n)-1)&~((n)-1)) /* all-caps to mark as macro: it evaluates n twice */

/*
 * known to compiler
 */
enum {
	Structrnd = sizeof(uintptr)
};

/*
 * type algorithms - known to compiler
 */
enum
{
	AMEM,
	AMEM0,
	AMEM8,
	AMEM16,
	AMEM32,
	AMEM64,
	AMEM128,
	ANOEQ,
	ANOEQ0,
	ANOEQ8,
	ANOEQ16,
	ANOEQ32,
	ANOEQ64,
	ANOEQ128,
	ASTRING,
	AINTER,
	ANILINTER,
	ASLICE,
	AFLOAT32,
	AFLOAT64,
	ACPLX64,
	ACPLX128,
	Amax
};
typedef	struct	Alg		Alg;
struct	Alg
{
	void	(*hash)(uintptr*, uintptr, void*);
	void	(*equal)(bool*, uintptr, void*, void*);
	void	(*print)(uintptr, void*);
	void	(*copy)(uintptr, void*, void*);
};

extern	Alg	runtime·algarray[Amax];

byte*	runtime·startup_random_data;
uint32	runtime·startup_random_data_len;
void	runtime·get_random_data(byte**, int32*);

enum {
	// hashinit wants this many random bytes
	HashRandomBytes = 32
};
void	runtime·hashinit(void);

void	runtime·memhash(uintptr*, uintptr, void*);
void	runtime·nohash(uintptr*, uintptr, void*);
void	runtime·strhash(uintptr*, uintptr, void*);
void	runtime·interhash(uintptr*, uintptr, void*);
void	runtime·nilinterhash(uintptr*, uintptr, void*);
void	runtime·aeshash(uintptr*, uintptr, void*);
void	runtime·aeshash32(uintptr*, uintptr, void*);
void	runtime·aeshash64(uintptr*, uintptr, void*);
void	runtime·aeshashstr(uintptr*, uintptr, void*);

void	runtime·memequal(bool*, uintptr, void*, void*);
void	runtime·noequal(bool*, uintptr, void*, void*);
void	runtime·strequal(bool*, uintptr, void*, void*);
void	runtime·interequal(bool*, uintptr, void*, void*);
void	runtime·nilinterequal(bool*, uintptr, void*, void*);

bool	runtime·memeq(void*, void*, uintptr);

void	runtime·memprint(uintptr, void*);
void	runtime·strprint(uintptr, void*);
void	runtime·interprint(uintptr, void*);
void	runtime·nilinterprint(uintptr, void*);

void	runtime·memcopy(uintptr, void*, void*);
void	runtime·memcopy8(uintptr, void*, void*);
void	runtime·memcopy16(uintptr, void*, void*);
void	runtime·memcopy32(uintptr, void*, void*);
void	runtime·memcopy64(uintptr, void*, void*);
void	runtime·memcopy128(uintptr, void*, void*);
void	runtime·strcopy(uintptr, void*, void*);
void	runtime·algslicecopy(uintptr, void*, void*);
void	runtime·intercopy(uintptr, void*, void*);
void	runtime·nilintercopy(uintptr, void*, void*);

/*
 * deferred subroutine calls
 */
struct Defer
{
	int32	siz;
	bool	special;	// not part of defer frame
	bool	free;		// if special, free when done
	byte*	argp;		// where args were copied from
	byte*	pc;
	FuncVal*	fn;
	Defer*	link;
	void*	args[1];	// padded to actual size
};

struct DeferChunk
{
	DeferChunk	*prev;
	uintptr	off;
};

/*
 * panics
 */
struct Panic
{
	Eface	arg;		// argument to panic
	uintptr	stackbase;	// g->stackbase in panic
	Panic*	link;		// link to earlier panic
	bool	recovered;	// whether this panic is over
};

/*
 * stack traces
 */
typedef struct Stkframe Stkframe;
struct Stkframe
{
	Func*	fn;	// function being run
	uintptr	pc;	// program counter within fn
	uintptr	lr;	// program counter at caller aka link register
	uintptr	sp;	// stack pointer at pc
	uintptr	fp;	// stack pointer at caller aka frame pointer
	byte*	varp;	// top of local variables
	byte*	argp;	// pointer to function arguments
	uintptr	arglen;	// number of bytes at argp
};

int32	runtime·gentraceback(uintptr, uintptr, uintptr, G*, int32, uintptr*, int32, void(*)(Stkframe*, void*), void*, bool);
void	runtime·traceback(uintptr pc, uintptr sp, uintptr lr, G* gp);
void	runtime·tracebackothers(G*);
bool	runtime·haszeroargs(uintptr pc);
bool	runtime·topofstack(Func*);

/*
 * external data
 */
extern	String	runtime·emptystring;
extern	uintptr runtime·zerobase;
// 各p的本地队列中, 加上全局队列中所有G对象的链表
extern	G*	runtime·allg; 
extern	G*	runtime·lastg;
extern	M*	runtime·allm;
// P指针链表, 在runtime·schedinit()中按(MaxGomaxprocs+1)申请的空间
// 就是说这个链表可容纳MaxGomaxprocs+1个P对象.
extern	P**	runtime·allp; 
extern	int32	runtime·gomaxprocs;
extern	uint32	runtime·needextram;
extern	uint32	runtime·panicking;
extern	int8*	runtime·goos;
extern	int32	runtime·ncpu;
extern	bool	runtime·iscgo;
extern 	void	(*runtime·sysargs)(int32, uint8**);
extern	uintptr	runtime·maxstring;
extern	uint32	runtime·Hchansize;
extern	uint32	runtime·cpuid_ecx;
extern	uint32	runtime·cpuid_edx;
extern	DebugVars	runtime·debug;
extern	uintptr	runtime·maxstacksize;

/*
 * common functions and data
 */
int32	runtime·strcmp(byte*, byte*);
byte*	runtime·strstr(byte*, byte*);
intgo	runtime·findnull(byte*);
intgo	runtime·findnullw(uint16*);
void	runtime·dump(byte*, int32);
int32	runtime·runetochar(byte*, int32);
int32	runtime·charntorune(int32*, uint8*, int32);

/*
 * very low level c-called
 */
#define FLUSH(x)	USED(x)

void	runtime·gogo(Gobuf*);
void	runtime·gostartcall(Gobuf*, void(*)(void), void*);
void	runtime·gostartcallfn(Gobuf*, FuncVal*);
void	runtime·gosave(Gobuf*);
void	runtime·lessstack(void);
void	runtime·goargs(void);
void	runtime·goenvs(void);
void	runtime·goenvs_unix(void);
void*	runtime·getu(void);
void	runtime·throw(int8*);
void	runtime·panicstring(int8*);
void	runtime·prints(int8*);
void	runtime·printf(int8*, ...);
byte*	runtime·mchr(byte*, byte, byte*);
int32	runtime·mcmp(byte*, byte*, uintptr);
void	runtime·memmove(void*, void*, uintptr);
void*	runtime·mal(uintptr);
String	runtime·catstring(String, String);
String	runtime·gostring(byte*);
String  runtime·gostringn(byte*, intgo);
Slice	runtime·gobytes(byte*, intgo);
String	runtime·gostringnocopy(byte*);
String	runtime·gostringw(uint16*);
void	runtime·initsig(void);
void	runtime·sigenable(uint32 sig);
void	runtime·sigdisable(uint32 sig);
int32	runtime·gotraceback(bool *crash);
void	runtime·goroutineheader(G*);
int32	runtime·open(int8*, int32, int32);
int32	runtime·read(int32, void*, int32);
int32	runtime·write(int32, void*, int32);
int32	runtime·close(int32);
int32	runtime·mincore(void*, uintptr, byte*);
bool	runtime·cas(uint32*, uint32, uint32);
bool	runtime·cas64(uint64*, uint64, uint64);
bool	runtime·casp(void**, void*, void*);
// Don't confuse with XADD x86 instruction,
// this one is actually 'addx', that is, add-and-fetch.
uint32	runtime·xadd(uint32 volatile*, int32);
uint64	runtime·xadd64(uint64 volatile*, int64);
uint32	runtime·xchg(uint32 volatile*, uint32);
uint64	runtime·xchg64(uint64 volatile*, uint64);
uint32	runtime·atomicload(uint32 volatile*);
void	runtime·atomicstore(uint32 volatile*, uint32);
void	runtime·atomicstore64(uint64 volatile*, uint64);
uint64	runtime·atomicload64(uint64 volatile*);
void*	runtime·atomicloadp(void* volatile*);
void	runtime·atomicstorep(void* volatile*, void*);
void	runtime·jmpdefer(FuncVal*, void*);
void	runtime·exit1(int32);
void	runtime·ready(G*);
byte*	runtime·getenv(int8*);
int32	runtime·atoi(byte*);
void	runtime·newosproc(M *mp, void *stk);
void	runtime·mstart(void);
G*	runtime·malg(int32);
void	runtime·asminit(void);
void	runtime·mpreinit(M*);
void	runtime·minit(void);
void	runtime·unminit(void);
void	runtime·signalstack(byte*, int32);
void	runtime·symtabinit(void);
Func*	runtime·findfunc(uintptr);
int32	runtime·funcline(Func*, uintptr, String*);
int32	runtime·funcarglen(Func*, uintptr);
int32	runtime·funcspdelta(Func*, uintptr);
int8*	runtime·funcname(Func*);
int32	runtime·pcdatavalue(Func*, int32, uintptr);
void*	runtime·stackalloc(uint32);
void	runtime·stackfree(void*, uintptr);
MCache*	runtime·allocmcache(void);
void	runtime·freemcache(MCache*);
void	runtime·mallocinit(void);
void	runtime·mprofinit(void);
bool	runtime·ifaceeq_c(Iface, Iface);
bool	runtime·efaceeq_c(Eface, Eface);
uintptr	runtime·ifacehash(Iface, uintptr);
uintptr	runtime·efacehash(Eface, uintptr);
void*	runtime·malloc(uintptr size);
void	runtime·free(void *v);
void	runtime·runpanic(Panic*);
uintptr	runtime·getcallersp(void*);
int32	runtime·mcount(void);
int32	runtime·gcount(void);
void	runtime·mcall(void(*)(G*));
uint32	runtime·fastrand1(void);
void	runtime·rewindmorestack(Gobuf*);
int32	runtime·timediv(int64, int32, int32*);

void runtime·setmg(M*, G*);
void runtime·newextram(void);
void	runtime·exit(int32);
void	runtime·breakpoint(void);
void	runtime·gosched(void);
void	runtime·gosched0(G*);
void	runtime·schedtrace(bool);
void	runtime·park(void(*)(Lock*), Lock*, int8*);
void	runtime·tsleep(int64, int8*);
M*	runtime·newm(void);
void	runtime·goexit(void);
void	runtime·asmcgocall(void (*fn)(void*), void*);
void	runtime·entersyscall(void);
void	runtime·entersyscallblock(void);
void	runtime·exitsyscall(void);
G*	runtime·newproc1(FuncVal*, byte*, int32, int32, void*);
bool	runtime·sigsend(int32 sig);
int32	runtime·callers(int32, uintptr*, int32);
int64	runtime·nanotime(void);
void	runtime·dopanic(int32);
void	runtime·startpanic(void);
void	runtime·freezetheworld(void);
void	runtime·unwindstack(G*, byte*);
void	runtime·sigprof(uint8 *pc, uint8 *sp, uint8 *lr, G *gp);
void	runtime·resetcpuprofiler(int32);
void	runtime·setcpuprofilerate(void(*)(uintptr*, int32), int32);
void	runtime·usleep(uint32);
int64	runtime·cputicks(void);
int64	runtime·tickspersecond(void);
void	runtime·blockevent(int64, int32);
extern int64 runtime·blockprofilerate;
void	runtime·addtimer(Timer*);
bool	runtime·deltimer(Timer*);
G*	runtime·netpoll(bool);
void	runtime·netpollinit(void);
int32	runtime·netpollopen(uintptr, PollDesc*);
int32   runtime·netpollclose(uintptr);
void	runtime·netpollready(G**, PollDesc*, int32);
uintptr	runtime·netpollfd(PollDesc*);
void	runtime·crash(void);
void	runtime·parsedebugvars(void);
void	_rt0_go(void);
void*	runtime·funcdata(Func*, int32);

#pragma	varargck	argpos	runtime·printf	1
#pragma	varargck	type	"c"	int32
#pragma	varargck	type	"d"	int32
#pragma	varargck	type	"d"	uint32
#pragma	varargck	type	"D"	int64
#pragma	varargck	type	"D"	uint64
#pragma	varargck	type	"x"	int32
#pragma	varargck	type	"x"	uint32
#pragma	varargck	type	"X"	int64
#pragma	varargck	type	"X"	uint64
#pragma	varargck	type	"p"	void*
#pragma	varargck	type	"p"	uintptr
#pragma	varargck	type	"s"	int8*
#pragma	varargck	type	"s"	uint8*
#pragma	varargck	type	"S"	String

void	runtime·stoptheworld(void);
void	runtime·starttheworld(void);
extern uint32 runtime·worldsema;

/*
 * mutual exclusion locks. 
 * in the uncontended case,
 * as fast as spin locks (just a few user-level instructions),
 * but on the contention path they sleep in the kernel.
 * a zeroed Lock is unlocked (no need to initialize each lock).
 * 互斥锁Mutex实现
 * 在没有竞争的情况下, 与自旋锁一样快(只有一些用户层的操作),
 * 但存在竞争时, ta会在内核层休眠.
 * 0值的Lock实例对象是没有加锁的(不需要对Lock实例对象进行初始化)
 * 目前看到两处runtime·lock的实现: lock_futex.c, lock_sema.c
 * 编译时, 不同的系统平台会选择不同的方式, 
 * linux, freebsd, dragonfly选择futex实现
 * darwin netbsd openbsd plan9 windows选择信号量实现.
 *
 * 不过很奇怪, lock/unlock函数的参数是什么?
 * 在主调函数里参数类型都是一个地址, 难道能和Lock*类型相互转换?
 */
void	runtime·lock(Lock*);
void	runtime·unlock(Lock*);

/*
 * sleep and wakeup on one-time events.
 * before any calls to notesleep or notewakeup,
 * must call noteclear to initialize the Note.
 * then, exactly one thread can call notesleep
 * and exactly one thread can call notewakeup (once).
 * once notewakeup has been called, the notesleep will return. 
 * future notesleep will return immediately.
 * subsequent noteclear must be called only after
 * previous notesleep has returned, e.g. it's disallowed
 * to call noteclear straight after notewakeup.
 * 在一次性事件中休眠或唤醒. 在调用notesleep()和notewakeup()之前,
 * 必须先调用noteclear()初始化Note对象.
 * 然后, 只有一个线程可以调用notesleep(), 也只有一个线程可以调用notewakeup()
 * 一旦notewakeup()被调用, notesleep()就会返回, 且以后notesleep()都会立即返回.
 * 我想, 要想再次实现这种休眠与唤醒, 需要再次初始化Note对象, 重新来过吧.
 * 另外, noteclear()只能在上一次的notesleep()返回后才能调用,
 * 即, 调用notewakeup()后不可立即调用noteclear(), 需要等到notesleep()返回后才行.
 *
 * notetsleep is like notesleep but wakes up after a given number of nanoseconds 
 * even if the event has not yet happened. 
 * if a goroutine uses notetsleep to wake up early,
 * it must wait to call noteclear until 
 * it can be sure that no other goroutine is calling notewakeup.
 * notetsleep与notesleep相似, 但ta会在一个给定时间内(单位为纳秒)返回,
 * 即使目标事件并没有发生.
 * 如果一个协程使用notetsleep()希望能够在调用wakeup()前提前结束,
 * 那ta必须...啥, 直到能够确认没有其他协程会调用notewakeup()
 * notesleep/notetsleep are generally called on g0,
 * notetsleepg is similar to notetsleep but is called on user g.
 * notesleep/notetsleep一般在g0上调用,
 * 而notetsleepg与notetsleep类似, 但是是在用户级协程g对象上调用.
 */
void	runtime·noteclear(Note*);
void	runtime·notesleep(Note*);
void	runtime·notewakeup(Note*);
// false - timeout
// 以下两个函数, 返回false时都表示超时时间到而返回, 而不是因为被唤醒.
bool	runtime·notetsleep(Note*, int64);
// false - timeout
bool	runtime·notetsleepg(Note*, int64);

/*
 * low-level synchronization for implementing the above
 * 底层同步机制, 为上面的noteXXX与lockXXX提供了支持.
 * win平台使用的sema, linux使用的是futex.
 */
uintptr	runtime·semacreate(void);
int32	runtime·semasleep(int64);
void	runtime·semawakeup(M*);
// or
void	runtime·futexsleep(uint32*, uint32, int64);
void	runtime·futexwakeup(uint32*, uint32);

/*
 * Lock-free stack.
 * Initialize uint64 head to 0, compare with 0 to test for emptiness.
 * The stack does not keep pointers to nodes,
 * so they can be garbage collected if there are no other pointers to nodes.
 */
void	runtime·lfstackpush(uint64 *head, LFNode *node);
LFNode*	runtime·lfstackpop(uint64 *head);

/*
 * Parallel for over [0, n).
 * body() is executed for each iteration.
 * nthr - total number of worker threads.
 * ctx - arbitrary user context.
 * if wait=true, threads return from parfor() when all work is done;
 * otherwise, threads can return while other threads are still finishing processing.
 */
ParFor*	runtime·parforalloc(uint32 nthrmax);
void	runtime·parforsetup(ParFor *desc, uint32 nthr, uint32 n, void *ctx, bool wait, void (*body)(ParFor*, uint32));
void	runtime·parfordo(ParFor *desc);

/*
 * This is consistent across Linux and BSD.
 * If a new OS is added that is different, move this to
 * $GOOS/$GOARCH/defs.h.
 */
#define EACCES		13

/*
 * low level C-called
 */
// for mmap, we only pass the lower 32 bits of file offset to the 
// assembly routine; the higher bits (if required), 
// should be provided by the assembly routine as 0.
// 底层C调用
// ...啥意思???
uint8*	runtime·mmap(byte*, uintptr, int32, int32, int32, uint32);
void	runtime·munmap(byte*, uintptr);
void	runtime·madvise(byte*, uintptr, int32);
void	runtime·memclr(byte*, uintptr);
void	runtime·setcallerpc(void*, void*);
void*	runtime·getcallerpc(void*);

/*
 * runtime go-called
 */
void	runtime·printbool(bool);
void	runtime·printbyte(int8);
void	runtime·printfloat(float64);
void	runtime·printint(int64);
void	runtime·printiface(Iface);
void	runtime·printeface(Eface);
void	runtime·printstring(String);
void	runtime·printpc(void*);
void	runtime·printpointer(void*);
void	runtime·printuint(uint64);
void	runtime·printhex(uint64);
void	runtime·printslice(Slice);
void	runtime·printcomplex(Complex128);
void	runtime·newstackcall(FuncVal*, byte*, uint32);
void	reflect·call(FuncVal*, byte*, uint32);
void	runtime·panic(Eface);
void	runtime·panicindex(void);
void	runtime·panicslice(void);

/*
 * runtime c-called (but written in Go)
 */
void	runtime·printany(Eface);
void	runtime·newTypeAssertionError(String*, String*, String*, String*, Eface*);
void	runtime·newErrorString(String, Eface*);
void	runtime·newErrorCString(int8*, Eface*);
void	runtime·fadd64c(uint64, uint64, uint64*);
void	runtime·fsub64c(uint64, uint64, uint64*);
void	runtime·fmul64c(uint64, uint64, uint64*);
void	runtime·fdiv64c(uint64, uint64, uint64*);
void	runtime·fneg64c(uint64, uint64*);
void	runtime·f32to64c(uint32, uint64*);
void	runtime·f64to32c(uint64, uint32*);
void	runtime·fcmp64c(uint64, uint64, int32*, bool*);
void	runtime·fintto64c(int64, uint64*);
void	runtime·f64tointc(uint64, int64*, bool*);

/*
 * wrapped for go users
 */
float64	runtime·Inf(int32 sign);
float64	runtime·NaN(void);
float32	runtime·float32frombits(uint32 i);
uint32	runtime·float32tobits(float32 f);
float64	runtime·float64frombits(uint64 i);
uint64	runtime·float64tobits(float64 f);
float64	runtime·frexp(float64 d, int32 *ep);
bool	runtime·isInf(float64 f, int32 sign);
bool	runtime·isNaN(float64 f);
float64	runtime·ldexp(float64 d, int32 e);
float64	runtime·modf(float64 d, float64 *ip);
void	runtime·semacquire(uint32*, bool);
void	runtime·semrelease(uint32*);
int32	runtime·gomaxprocsfunc(int32 n);
void	runtime·procyield(uint32);
void	runtime·osyield(void);
void	runtime·lockOSThread(void);
void	runtime·unlockOSThread(void);

void	runtime·mapassign(MapType*, Hmap*, byte*, byte*);
void	runtime·mapaccess(MapType*, Hmap*, byte*, byte*, bool*);
void	runtime·mapiternext(struct hash_iter*);
bool	runtime·mapiterkey(struct hash_iter*, void*);
Hmap*	runtime·makemap_c(MapType*, int64);

Hchan*	runtime·makechan_c(ChanType*, int64);
void	runtime·chansend(ChanType*, Hchan*, byte*, bool*, void*);
void	runtime·chanrecv(ChanType*, Hchan*, byte*, bool*, bool*);
bool	runtime·showframe(Func*, G*);
void	runtime·printcreatedby(G*);

void	runtime·ifaceE2I(InterfaceType*, Eface, Iface*);
bool	runtime·ifaceE2I2(InterfaceType*, Eface, Iface*);
uintptr	runtime·memlimit(void);

// float.c
extern float64 runtime·nan;
extern float64 runtime·posinf;
extern float64 runtime·neginf;
extern uint64 ·nan;
extern uint64 ·posinf;
extern uint64 ·neginf;
#define ISNAN(f) ((f) != (f))

enum
{
	UseSpanType = 1,
};
