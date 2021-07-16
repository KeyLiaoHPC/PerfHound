	.file	"test1_3regs_add.c"
# GNU C17 (GCC) version 9.3.1 20200408 (Red Hat 9.3.1-2) (x86_64-redhat-linux)
#	compiled by GNU C version 9.3.1 20200408 (Red Hat 9.3.1-2), GMP version 6.0.0, MPFR version 3.1.1, MPC version 1.0.1, isl version none
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -I PERFHOUND/include -D NADD=10841 -D NMEASURE=10000
# -D PERFHOUND test1_3regs_add.c -mtune=generic -march=x86-64
# -auxbase-strip test1_3regs_add_PERFHOUND.s -O2 -funroll-all-loops
# -fverbose-asm
# options enabled:  -faggressive-loop-optimizations -falign-functions
# -falign-jumps -falign-labels -falign-loops -fassume-phsa
# -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg
# -fcaller-saves -fcode-hoisting -fcombine-stack-adjustments -fcommon
# -fcompare-elim -fcprop-registers -fcrossjumping -fcse-follow-jumps
# -fdefer-pop -fdelete-null-pointer-checks -fdevirtualize
# -fdevirtualize-speculatively -fdwarf2-cfi-asm -fearly-inlining
# -feliminate-unused-debug-types -fexpensive-optimizations
# -fforward-propagate -ffp-int-builtin-inexact -ffunction-cse -fgcse
# -fgcse-lm -fgnu-runtime -fgnu-unique -fguess-branch-probability
# -fhoist-adjacent-loads -fident -fif-conversion -fif-conversion2
# -findirect-inlining -finline -finline-atomics
# -finline-functions-called-once -finline-small-functions -fipa-bit-cp
# -fipa-cp -fipa-icf -fipa-icf-functions -fipa-icf-variables -fipa-profile
# -fipa-pure-const -fipa-ra -fipa-reference -fipa-reference-addressable
# -fipa-sra -fipa-stack-alignment -fipa-vrp -fira-hoist-pressure
# -fira-share-save-slots -fira-share-spill-slots
# -fisolate-erroneous-paths-dereference -fivopts -fkeep-static-consts
# -fleading-underscore -flifetime-dse -flra-remat -flto-odr-type-merging
# -fmath-errno -fmerge-constants -fmerge-debug-strings
# -fmove-loop-invariants -fomit-frame-pointer -foptimize-sibling-calls
# -foptimize-strlen -fpartial-inlining -fpeephole -fpeephole2 -fplt
# -fprefetch-loop-arrays -free -freg-struct-return -frename-registers
# -freorder-blocks -freorder-blocks-and-partition -freorder-functions
# -frerun-cse-after-loop -fsched-critical-path-heuristic
# -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
# -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
# -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fschedule-fusion
# -fschedule-insns2 -fsemantic-interposition -fshow-column -fshrink-wrap
# -fshrink-wrap-separate -fsigned-zeros -fsplit-ivs-in-unroller
# -fsplit-wide-types -fssa-backprop -fssa-phiopt -fstdarg-opt
# -fstore-merging -fstrict-aliasing -fstrict-volatile-bitfields
# -fsync-libcalls -fthread-jumps -ftoplevel-reorder -ftrapping-math
# -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp -ftree-ch
# -ftree-coalesce-vars -ftree-copy-prop -ftree-cselim -ftree-dce
# -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
# -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
# -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr
# -ftree-sra -ftree-switch-conversion -ftree-tail-merge -ftree-ter
# -ftree-vrp -funit-at-a-time -funroll-all-loops -funroll-loops
# -funwind-tables -fverbose-asm -fweb -fzero-initialized-in-bss
# -m128bit-long-double -m64 -m80387 -malign-stringops
# -mavx256-split-unaligned-load -mavx256-split-unaligned-store
# -mfancy-math-387 -mfp-ret-in-387 -mfxsr -mglibc -mieee-fp
# -mlong-double-80 -mmmx -mno-sse4 -mpush-args -mred-zone -msse -msse2
# -mstv -mtls-direct-seg-refs -mvzeroupper

	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ipdps-1_6248"
.LC1:
	.string	"."
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC2:
	.string	"Failed at initailizing PerfHound."
	.section	.rodata.str1.1
.LC7:
	.string	"%d\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB20:
	.cfi_startproc
	pushq	%r13	#
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
# test1_3regs_add.c:43:     if (vt_init(".", "ipdps-1_6248")) {
	movl	$.LC0, %esi	#,
	movl	$.LC1, %edi	#,
# test1_3regs_add.c:33: main(int argc, char **argv) {
	pushq	%r12	#
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbx	#
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	subq	$32, %rsp	#,
	.cfi_def_cfa_offset 64
# test1_3regs_add.c:43:     if (vt_init(".", "ipdps-1_6248")) {
	call	vt_init	#
# test1_3regs_add.c:43:     if (vt_init(".", "ipdps-1_6248")) {
	testl	%eax, %eax	# tmp1791
	jne	.L1648	#,
# test1_3regs_add.c:48:     vt_commit();
	xorl	%eax, %eax	#
	call	vt_commit	#
# test1_3regs_add.c:69:     clock_gettime(CLOCK_MONOTONIC, &tv);
	leaq	16(%rsp), %rsi	#, tmp1792
	movl	$1, %edi	#,
	call	clock_gettime	#
# test1_3regs_add.c:70:     sec = tv.tv_sec;
	movq	16(%rsp), %rdx	# tv.tv_sec, _2
# test1_3regs_add.c:71:     nsec = tv.tv_nsec;
	movq	24(%rsp), %rdi	# tv.tv_nsec, _4
# test1_3regs_add.c:70:     sec = tv.tv_sec;
	movq	%rdx, (%rsp)	# _2, sec
# test1_3regs_add.c:71:     nsec = tv.tv_nsec;
	movq	%rdi, 8(%rsp)	# _4, nsec
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movq	(%rsp), %rsi	# sec, sec.0_6
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movq	8(%rsp), %rcx	# nsec, nsec.1_9
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	testq	%rsi, %rsi	# sec.0_6
	js	.L3	#,
	pxor	%xmm0, %xmm0	# tmp118
	cvtsi2sdq	%rsi, %xmm0	# sec.0_6, tmp118
.L4:
	mulsd	.LC3(%rip), %xmm0	#, tmp122
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	testq	%rcx, %rcx	# nsec.1_9
	js	.L5	#,
	pxor	%xmm1, %xmm1	# tmp124
	cvtsi2sdq	%rcx, %xmm1	# nsec.1_9, tmp124
.L6:
	addsd	%xmm1, %xmm0	# tmp124, tmp128
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	addsd	.LC4(%rip), %xmm0	#, tmp129
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movsd	.LC5(%rip), %xmm2	#, tmp131
	comisd	%xmm2, %xmm0	# tmp131, tmp129
	jnb	.L7	#,
	cvttsd2siq	%xmm0, %r8	# tmp129, _13
.L8:
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	pxor	%xmm4, %xmm4	# tmp135
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	pxor	%xmm3, %xmm3	# tmp138
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movq	%r8, 8(%rsp)	# _13, nsec
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	movq	8(%rsp), %r9	# nsec, nsec.2_62
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	cvtsi2sdq	%rdx, %xmm4	# _2, tmp135
	mulsd	.LC3(%rip), %xmm4	#, tmp136
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	cvtsi2sdq	%rdi, %xmm3	# _4, tmp138
	addsd	%xmm3, %xmm4	# tmp138, tmp139
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	testq	%r9, %r9	# nsec.2_62
	js	.L10	#,
	pxor	%xmm6, %xmm6	# tmp140
	cvtsi2sdq	%r9, %xmm6	# nsec.2_62, tmp140
.L11:
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	comisd	%xmm4, %xmm6	# tmp139, tmp140
	jbe	.L9	#,
.L1649:
# test1_3regs_add.c:74:         clock_gettime(CLOCK_MONOTONIC, &tv);
	leaq	16(%rsp), %rsi	#, tmp1796
	movl	$1, %edi	#,
	call	clock_gettime	#
# test1_3regs_add.c:75:         srand(tv.tv_nsec);
	movl	24(%rsp), %edi	# tv.tv_nsec,
	call	srand	#
# test1_3regs_add.c:76:         res += rand() % 2 + tv.tv_nsec;
	call	rand	#
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	pxor	%xmm4, %xmm4	# tmp146
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	pxor	%xmm5, %xmm5	# tmp149
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	movq	8(%rsp), %r11	# nsec, nsec.2_22
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	cvtsi2sdq	16(%rsp), %xmm4	# tv.tv_sec, tmp146
	mulsd	.LC3(%rip), %xmm4	#, tmp147
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	cvtsi2sdq	24(%rsp), %xmm5	# tv.tv_nsec, tmp149
	addsd	%xmm5, %xmm4	# tmp149, tmp150
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	testq	%r11, %r11	# nsec.2_22
	js	.L14	#,
	pxor	%xmm6, %xmm6	# tmp151
	cvtsi2sdq	%r11, %xmm6	# nsec.2_22, tmp151
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	comisd	%xmm4, %xmm6	# tmp139, tmp140
	ja	.L1649	#,
.L9:
# test1_3regs_add.c:79:     asm volatile (
#APP
# 79 "test1_3regs_add.c" 1
	mov    $0, %r11
	mov    $0, %r12
	mov    $1, %r13
	
# 0 "" 2
#NO_APP
	movl	$10000, %ebx	#, ivtmp_28
	.p2align 4,,10
	.p2align 3
.L17:
# test1_3regs_add.c:94:         vt_read(1, 1, 0, 0, 0);
	xorl	%ecx, %ecx	#
	xorl	%edx, %edx	#
	pxor	%xmm0, %xmm0	#
	movl	$1, %esi	#,
	movl	$1, %edi	#,
	call	vt_read	#
# test1_3regs_add.c:104:              asm volatile(
#APP
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
#NO_APP
	movl	$10240, %edx	#, ivtmp_59
.L16:
# test1_3regs_add.c:104:              asm volatile(
#APP
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
#NO_APP
	subl	$1, %edx	#, tmp161
#APP
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# 104 "test1_3regs_add.c" 1
	lea    (%r11,%r13), %r12
	lea    (%r12,%r13), %r11
	
# 0 "" 2
# test1_3regs_add.c:103:         for (i = 0; i < NADD; i ++) {
#NO_APP
	subl	$1023, %edx	#, ivtmp_59
	jne	.L16	#,
# test1_3regs_add.c:112:         vt_read(1, 2, 0, 0, 0);
	xorl	%ecx, %ecx	#
	xorl	%edx, %edx	#
	pxor	%xmm0, %xmm0	#
	movl	$2, %esi	#,
	movl	$1, %edi	#,
	call	vt_read	#
# test1_3regs_add.c:91:     while (ic--){
	subl	$1, %ebx	#, ivtmp_28
	jne	.L17	#,
# test1_3regs_add.c:128:     asm volatile (
#APP
# 128 "test1_3regs_add.c" 1
	mov    %r11, %rsi	# res
	
# 0 "" 2
# test1_3regs_add.c:135:     printf("%d\n", res);
#NO_APP
	movl	$.LC7, %edi	#,
	xorl	%eax, %eax	#
	call	printf	#
# test1_3regs_add.c:138:     vt_clean();
	xorl	%eax, %eax	#
	call	vt_clean	#
# test1_3regs_add.c:152: }
	addq	$32, %rsp	#,
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	xorl	%eax, %eax	#
	popq	%rbx	#
	.cfi_def_cfa_offset 24
	popq	%r12	#
	.cfi_def_cfa_offset 16
	popq	%r13	#
	.cfi_def_cfa_offset 8
	ret	
.L7:
	.cfi_restore_state
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	subsd	%xmm2, %xmm0	# tmp131, tmp132
	cvttsd2siq	%xmm0, %r8	# tmp132, _13
	btcq	$63, %r8	#, _13
	jmp	.L8	#
.L14:
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	movq	%r11, %r12	# nsec.2_22, tmp153
	andl	$1, %r11d	#, tmp154
	pxor	%xmm6, %xmm6	# tmp152
	shrq	%r12	# tmp153
	orq	%r11, %r12	# tmp154, tmp166
	cvtsi2sdq	%r12, %xmm6	# tmp166, tmp152
	addsd	%xmm6, %xmm6	# tmp152, tmp151
	jmp	.L11	#
.L5:
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movq	%rcx, %rbx	# nsec.1_9, tmp126
	andl	$1, %ecx	#, tmp127
	pxor	%xmm1, %xmm1	# tmp125
	shrq	%rbx	# tmp126
	orq	%rcx, %rbx	# tmp127, tmp163
	cvtsi2sdq	%rbx, %xmm1	# tmp163, tmp125
	addsd	%xmm1, %xmm1	# tmp125, tmp124
	jmp	.L6	#
.L10:
# test1_3regs_add.c:73:     while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
	movq	%r9, %r10	# nsec.2_62, tmp142
	andl	$1, %r9d	#, tmp143
	pxor	%xmm6, %xmm6	# tmp141
	shrq	%r10	# tmp142
	orq	%r9, %r10	# tmp143, tmp165
	cvtsi2sdq	%r10, %xmm6	# tmp165, tmp141
	addsd	%xmm6, %xmm6	# tmp141, tmp140
	jmp	.L11	#
.L3:
# test1_3regs_add.c:72:     nsec = sec * 1e9 + nsec + 5e8;
	movq	%rsi, %rax	# sec.0_6, tmp120
	andl	$1, %esi	#, tmp121
	pxor	%xmm0, %xmm0	# tmp119
	shrq	%rax	# tmp120
	orq	%rsi, %rax	# tmp121, tmp162
	cvtsi2sdq	%rax, %xmm0	# tmp162, tmp119
	addsd	%xmm0, %xmm0	# tmp119, tmp118
	jmp	.L4	#
.L1648:
# test1_3regs_add.c:44:         printf("Failed at initailizing PerfHound.\n");
	movl	$.LC2, %edi	#,
	call	puts	#
# test1_3regs_add.c:45:         exit(1);
	movl	$1, %edi	#,
	call	exit	#
	.cfi_endproc
.LFE20:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC3:
	.long	0
	.long	1104006501
	.align 8
.LC4:
	.long	0
	.long	1102957925
	.align 8
.LC5:
	.long	0
	.long	1138753536
	.ident	"GCC: (GNU) 9.3.1 20200408 (Red Hat 9.3.1-2)"
	.section	.note.GNU-stack,"",@progbits
