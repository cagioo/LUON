/*
 * Vesege — Luon Native WASM Runtime
 * Minimal WASM interpreter for executing Luon-compiled modules.
 * This is the bootstrap loader: compile once, then Luon is self-hosting.
 *
 * Build: gcc -O2 -o luon luon_vm.c
 * Usage: ./luon build src/main.luon
 *        ./luon run src/main.luon -a 42
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>

#define MAX_STACK    65536
#define MAX_FUNCS    256
#define MAX_EXPORTS  64
#define MAX_LOCALS   256
#define MAX_BLOCKS   256
#define MAX_CALL     1024
#define MEM_PAGES    30
#define MEM_SIZE     (MEM_PAGES * 65536)
#define MAX_ITER     50000000

/* ═══ Types ═══ */
typedef int64_t  i64;
typedef uint64_t u64;
typedef uint8_t  u8;

typedef struct {
    int    n_locals;
    u8    *code;
    int    code_len;
} Func;

typedef struct {
    char name[64];
    int  func_idx;
} Export;

typedef struct {
    int  kind;  /* 0=block, 1=loop, 2=if */
    int  pos;
    int  stack_height;
} Block;

typedef struct {
    Func    funcs[MAX_FUNCS];
    int     n_funcs;
    Export  exports[MAX_EXPORTS];
    int     n_exports;
    u8     *memory;
    int     mem_pages;
    i64     globals[256];
    int     func_nparams[MAX_FUNCS]; /* param count per function */
    int     type_nparams[64];        /* param count per type */
    int     func_nreturns[MAX_FUNCS];
    int     type_nreturns[64];
    int     func_type[MAX_FUNCS];    /* type index per function */
    int     n_imports;               /* number of imported functions */
} Module;

/* ═══ LEB128 Decoding ═══ */
static u64 read_uleb(const u8 *data, int *pos) {
    u64 r = 0; int s = 0;
    while (1) {
        u8 b = data[(*pos)++];
        r |= (u64)(b & 0x7f) << s;
        if (!(b & 0x80)) break;
        s += 7;
    }
    return r;
}

static i64 read_sleb(const u8 *data, int *pos) {
    i64 r = 0; int s = 0; u8 b;
    do {
        b = data[(*pos)++];
        r |= (i64)(b & 0x7f) << s;
        s += 7;
    } while (b & 0x80);
    if (s < 64 && (b & 0x40)) r |= -(1LL << s);
    return r;
}

/* ═══ Module Parser ═══ */
static int parse_module(const u8 *data, int len, Module *m) {
    memset(m, 0, sizeof(*m));
    m->mem_pages = MEM_PAGES;
    m->memory = calloc(MEM_SIZE, 1);
    if (!m->memory) return -1;

    int pos = 8; /* skip magic + version */
    while (pos < len) {
        int sid = data[pos++];
        int ssz = (int)read_uleb(data, &pos);
        int send = pos + ssz;

        if (sid == 1) { /* Type section */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt && i < 64; i++) {
                pos++; /* skip 0x60 */
                int np = (int)read_uleb(data, &pos);
                m->type_nparams[i] = np;
                for (int j = 0; j < np; j++) pos++; /* skip param types */
                int nr = (int)read_uleb(data, &pos);
                m->type_nreturns[i] = nr;
                for (int j = 0; j < nr; j++) pos++; /* skip result types */
            }
        } else if (sid == 2) { /* Import section */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt; i++) {
                int mlen = (int)read_uleb(data, &pos); pos += mlen; /* skip module name */
                int flen = (int)read_uleb(data, &pos); pos += flen; /* skip field name */
                int kind = data[pos++];
                if (kind == 0) { /* function import */
                    read_uleb(data, &pos); /* skip type index */
                    m->n_imports++;
                } else if (kind == 1) { /* table */
                    pos++; read_uleb(data, &pos); /* elem type + limits */
                    if (data[pos-1] & 1) read_uleb(data, &pos);
                } else if (kind == 2) { /* memory */
                    int flags = data[pos++]; read_uleb(data, &pos);
                    if (flags & 1) read_uleb(data, &pos);
                } else if (kind == 3) { /* global */
                    pos += 2; /* type + mutability */
                }
            }
        } else if (sid == 3) { /* Function section */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt && i < MAX_FUNCS; i++) {
                int ti = (int)read_uleb(data, &pos);
                m->func_type[i] = ti;
                m->func_nparams[i] = (ti < 64) ? m->type_nparams[ti] : 1;
                m->func_nreturns[i] = (ti < 64) ? m->type_nreturns[ti] : 1;
            }
        } else if (sid == 5) { /* Memory */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt; i++) {
                int flags = data[pos++];
                m->mem_pages = (int)read_uleb(data, &pos);
                if (flags & 1) read_uleb(data, &pos);
            }
        } else if (sid == 7) { /* Export */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt && m->n_exports < MAX_EXPORTS; i++) {
                int nlen = (int)read_uleb(data, &pos);
                int cplen = nlen < 63 ? nlen : 63;
                memcpy(m->exports[m->n_exports].name, data + pos, cplen);
                m->exports[m->n_exports].name[cplen] = 0;
                pos += nlen;
                int kind = data[pos++];
                int idx = (int)read_uleb(data, &pos);
                if (kind == 0) {
                    m->exports[m->n_exports].func_idx = idx;
                    m->n_exports++;
                }
            }
        } else if (sid == 10) { /* Code */
            int cnt = (int)read_uleb(data, &pos);
            for (int i = 0; i < cnt && m->n_funcs < MAX_FUNCS; i++) {
                int bsz = (int)read_uleb(data, &pos);
                int bend = pos + bsz;
                int ldcnt = (int)read_uleb(data, &pos);
                int tl = 0;
                for (int j = 0; j < ldcnt; j++) {
                    int lc = (int)read_uleb(data, &pos);
                    pos++; /* type */
                    tl += lc;
                }
                m->funcs[m->n_funcs].n_locals = tl;
                m->funcs[m->n_funcs].code = (u8*)(data + pos);
                m->funcs[m->n_funcs].code_len = bend - pos;
                m->n_funcs++;
                pos = bend;
            }
        } else {
            pos = send;
        }
    }
    return 0;
}

static int find_export(Module *m, const char *name) {
    for (int i = 0; i < m->n_exports; i++)
        if (strcmp(m->exports[i].name, name) == 0)
            return m->exports[i].func_idx;
    return -1;
}

/* ═══ VM Execution ═══ */
static int call_depth = 0;

static void skip_block(const u8 *code, int *pos_ptr, int clen, int target_d) {
    int pos = *pos_ptr;
    int d = target_d;
    while(d > 0 && pos < clen) {
        u8 o = code[pos++];
        if(o==0x02 || o==0x03 || o==0x04) { pos++; d++; } /* block, loop, if */
        else if(o==0x0B) { d--; } /* end */
        else if(o==0x0C || o==0x0D || o==0x20 || o==0x21 || o==0x22 || o==0x23 || o==0x24 || o==0xD0 || o==0xD2) { read_uleb(code, &pos); }
        else if(o==0x10) { read_uleb(code, &pos); } /* call */
        else if(o==0x11) { read_uleb(code, &pos); read_uleb(code, &pos); } /* call_indirect */
        else if(o==0x41) { read_sleb(code, &pos); } /* i32.const */
        else if(o==0x42) { read_sleb(code, &pos); } /* i64.const */
        else if(o==0x43) { pos += 4; } /* f32.const */
        else if(o==0x44) { pos += 8; } /* f64.const */
        else if(o>=0x28 && o<=0x3E) { read_uleb(code, &pos); read_uleb(code, &pos); } /* mem load/store */
        else if(o==0x3F || o==0x40) { pos++; } /* mem size/grow */
        else if(o==0x0E) { /* br_table */
            int cnt = read_uleb(code, &pos);
            for(int i=0; i<=cnt; i++) read_uleb(code, &pos);
        }
    }
    *pos_ptr = pos;
}

static void vm_exec(Module *m, int fidx, i64 *args, int nargs, i64 *rets, int nrets) {
    if (fidx < 0 || fidx >= m->n_funcs) {
        fprintf(stderr, "Error: invalid function index %d\n", fidx);
        return;
    }
    if (++call_depth > MAX_CALL) {
        fprintf(stderr, "Error: stack overflow\n");
        call_depth--;
        return;
    }

    Func *f = &m->funcs[fidx];
    u8 *code = f->code;
    int clen = f->code_len;
    i64 locals[MAX_LOCALS] = {0};
    for (int i = 0; i < nargs && i < MAX_LOCALS; i++) locals[i] = args[i];

    i64 *stack = malloc(MAX_STACK * sizeof(i64));
    if (!stack) { fprintf(stderr, "Error: out of memory\n"); call_depth--; return; }
    int sp = 0;
    Block blocks[MAX_BLOCKS];
    int bp = 0;
    int pos = 0;
    long iter = 0;

#define PUSH(v) do { if(sp<MAX_STACK) stack[sp++]=(v); } while(0)
#define POP()   (sp>0 ? stack[--sp] : 0)
#define PEEK()  (sp>0 ? stack[sp-1] : 0)
#define I64(v)  ((i64)((u64)(v)))
#define U64(v)  ((u64)(v))

    while (pos < clen) {
        if (++iter > MAX_ITER) { fprintf(stderr, "Error: execution limit\n"); break; }
        u8 op = code[pos++];


        switch (op) {
        case 0x00: fprintf(stderr, "unreachable\n"); goto done;
        case 0x01: break; /* nop */
        case 0x02: { pos++; /* skip block type */ if(bp<MAX_BLOCKS){blocks[bp].kind=0;blocks[bp].pos=pos;blocks[bp].stack_height=sp;bp++;} break; }
        case 0x03: { pos++; /* skip block type */ if(bp<MAX_BLOCKS){blocks[bp].kind=1;blocks[bp].pos=pos;blocks[bp].stack_height=sp;bp++;} break; }
        case 0x04: { /* if */
            pos++; /* skip block type */ i64 c=POP();
            if(bp<MAX_BLOCKS){blocks[bp].kind=2;blocks[bp].pos=pos;blocks[bp].stack_height=sp;bp++;}
            if(c==0){ skip_block(code, &pos, clen, 1); } /* if skip to else or end */
            break;
        }
        case 0x05: { /* else */ skip_block(code, &pos, clen, 1); break; }
        case 0x0B: { /* end */
            if(bp>0) bp--; else { 
                for(int i=nrets-1; i>=0; i--) rets[i] = sp>0 ? stack[--sp] : 0;
                free(stack); call_depth--; return; 
            }
            break;
        }
        case 0x0C: { /* br */
            int label=(int)read_uleb(code,&pos);
            int ti=bp-1-label;
            if(ti<0){pos=clen;break;}
            if(blocks[ti].kind==1){bp=ti+1;pos=blocks[ti].pos;sp=blocks[ti].stack_height;}
            else{bp=ti;sp=blocks[ti].stack_height; skip_block(code, &pos, clen, label+1);}
            break;
        }
        case 0x0D: { /* br_if */
            int label=(int)read_uleb(code,&pos); i64 c=POP();
            if(c!=0){int ti=bp-1-label;if(ti<0){pos=clen;break;}
            if(blocks[ti].kind==1){bp=ti+1;pos=blocks[ti].pos;sp=blocks[ti].stack_height;}
            else{bp=ti;sp=blocks[ti].stack_height; skip_block(code, &pos, clen, label+1);}}
            break;
        }
        case 0x0E: { /* br_table */
            int cnt=(int)read_uleb(code,&pos);
            int *targets=alloca((cnt+1)*sizeof(int));
            for(int i=0;i<cnt;i++) targets[i]=(int)read_uleb(code,&pos);
            targets[cnt]=(int)read_uleb(code,&pos);
            i64 idx=POP(); int label=(idx>=0&&idx<cnt)?targets[(int)idx]:targets[cnt];
            int ti=bp-1-label;if(ti<0){pos=clen;break;}
            if(blocks[ti].kind==1){bp=ti+1;pos=blocks[ti].pos;sp=blocks[ti].stack_height;}
            else{bp=ti;sp=blocks[ti].stack_height; skip_block(code, &pos, clen, label+1);}
            break;
        }
        case 0x0F: { 
            for(int i=nrets-1; i>=0; i--) {
                rets[i] = sp>0 ? stack[--sp] : 0;
            }
            free(stack); call_depth--; return; 
        }
        case 0x10: { 
            int fi=(int)read_uleb(code,&pos);
            if (fi < m->n_imports) {
                /* WASI host function call */
                /* Currently only fd_write (import index 0): fd_write(fd, iovs, iovs_len, nwritten) */
                i64 p3=POP(), p2=POP(), p1=POP(), p0=POP();
                int fd=(int)p0, iovs_ptr=(int)p1, iovs_cnt=(int)p2, nwritten_ptr=(int)p3;
                int total=0;
                for (int iov=0; iov<iovs_cnt; iov++) {
                    int iov_addr = (iovs_ptr + iov*8) & (MEM_SIZE-1);
                    int buf_ptr=0, buf_len=0;
                    memcpy(&buf_ptr, m->memory+iov_addr, 4);
                    memcpy(&buf_len, m->memory+iov_addr+4, 4);
                    buf_ptr &= (MEM_SIZE-1);
                    if (buf_len > 0 && buf_ptr+buf_len <= MEM_SIZE) {
                        if (fd==1) fwrite(m->memory+buf_ptr, 1, buf_len, stdout);
                        else if (fd==2) fwrite(m->memory+buf_ptr, 1, buf_len, stderr);
                        total += buf_len;
                    }
                }
                if (nwritten_ptr > 0 && nwritten_ptr < MEM_SIZE-4) {
                    memcpy(m->memory+nwritten_ptr, &total, 4);
                }
                PUSH(0); /* return errno=0 (success) */
                break;
            }
            int local_fi = fi - m->n_imports;
            if (local_fi < 0 || local_fi >= m->n_funcs) { fprintf(stderr, "Error: call to undefined function %d\n", fi); PUSH(0); break; }
            int np=m->func_nparams[local_fi]; if(np<1) np=1;
            int nr=m->func_nreturns[local_fi]; if(nr<1) nr=1;
            i64 cargs[16]={0};
            i64 crets[16]={0};
            for(int i=np-1;i>=0;i--) cargs[i]=POP();
            vm_exec(m,local_fi,cargs,np,crets,nr);
            for(int i=0;i<nr;i++) PUSH(crets[i]);
            break;
        }
        case 0x1A: POP(); break;
        case 0x1B: { i64 c=POP(),b=POP(),a=POP(); PUSH(c!=0?a:b); break; }
        case 0x20: { int i=(int)read_uleb(code,&pos); PUSH(i<MAX_LOCALS?locals[i]:0); break; }
        case 0x21: { int i=(int)read_uleb(code,&pos); i64 v=POP(); if(i<MAX_LOCALS)locals[i]=v; break; }
        case 0x22: { int i=(int)read_uleb(code,&pos); if(i<MAX_LOCALS)locals[i]=PEEK(); break; }
        case 0x23: { int i=(int)read_uleb(code,&pos); PUSH(i<256?m->globals[i]:0); break; }
        case 0x24: { int i=(int)read_uleb(code,&pos); i64 v=POP(); if(i<256)m->globals[i]=v; break; }
        case 0x28: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); int addr=((int)POP()+off)&(MEM_SIZE-1); i64 v; memcpy(&v,m->memory+addr,4); PUSH((i64)(int32_t)v); break; }
        case 0x29: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); int addr=((int)POP()+off)&(MEM_SIZE-1); i64 v; memcpy(&v,m->memory+addr,8); PUSH(v); break; }
        case 0x2C: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); int addr=((int)POP()+off)&(MEM_SIZE-1); PUSH((i64)(int8_t)m->memory[addr]); break; }
        case 0x2D: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); int addr=((int)POP()+off)&(MEM_SIZE-1); PUSH((i64)m->memory[addr]); break; }
        case 0x36: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); i64 v=POP(); int addr=((int)POP()+off)&(MEM_SIZE-1); int32_t sv=(int32_t)v; memcpy(m->memory+addr,&sv,4); break; }
        case 0x37: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); i64 v=POP(); int addr=((int)POP()+off)&(MEM_SIZE-1); memcpy(m->memory+addr,&v,8); break; }
        case 0x3A: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); i64 v=POP(); int addr=((int)POP()+off)&(MEM_SIZE-1); m->memory[addr]=(u8)(v&0xFF); break; }
        case 0x3C: { read_uleb(code,&pos); int off=(int)read_uleb(code,&pos); i64 v=POP(); int addr=((int)POP()+off)&(MEM_SIZE-1); m->memory[addr]=(u8)(v&0xFF); break; }
        case 0x3F: { pos++; PUSH((i64)m->mem_pages); break; } /* memory.size */
        case 0x40: { pos++; int delta=(int)POP(); int old=m->mem_pages; if(delta>0 && old+delta<=256) { m->memory=realloc(m->memory,(old+delta)*65536); memset(m->memory+old*65536,0,delta*65536); m->mem_pages=old+delta; PUSH((i64)old); } else { PUSH((i64)-1); } break; } /* memory.grow */
        case 0x41: { PUSH((i64)(int32_t)read_sleb(code,&pos)); break; }
        case 0x42: { PUSH(read_sleb(code,&pos)); break; }
        case 0x45: { i64 v=POP(); PUSH(v==0?1:0); break; }
        case 0x46: { i64 b=POP(),a=POP(); PUSH(a==b?1:0); break; }
        case 0x47: { i64 b=POP(),a=POP(); PUSH(a!=b?1:0); break; }
        case 0x48: { i64 b=POP(),a=POP(); PUSH((int32_t)a<(int32_t)b?1:0); break; }
        case 0x4A: { i64 b=POP(),a=POP(); PUSH((int32_t)a>(int32_t)b?1:0); break; }
        case 0x50: { i64 v=POP(); PUSH(v==0?1:0); break; }
        case 0x51: { i64 b=POP(),a=POP(); PUSH(a==b?1:0); break; }
        case 0x52: { i64 b=POP(),a=POP(); PUSH(a!=b?1:0); break; }
        case 0x53: { i64 b=POP(),a=POP(); PUSH(a<b?1:0); break; }
        case 0x54: { i64 b=POP(),a=POP(); PUSH(U64(a)<U64(b)?1:0); break; }
        case 0x55: { i64 b=POP(),a=POP(); PUSH(a>b?1:0); break; }
        case 0x56: { i64 b=POP(),a=POP(); PUSH(U64(a)>U64(b)?1:0); break; }
        case 0x57: { i64 b=POP(),a=POP(); PUSH(a<=b?1:0); break; }
        case 0x58: { i64 b=POP(),a=POP(); PUSH(U64(a)<=U64(b)?1:0); break; }
        case 0x59: { i64 b=POP(),a=POP(); PUSH(a>=b?1:0); break; }
        case 0x5A: { i64 b=POP(),a=POP(); PUSH(U64(a)>=U64(b)?1:0); break; }
        case 0x7C: { i64 b=POP(),a=POP(); PUSH(a+b); break; }
        case 0x7D: { i64 b=POP(),a=POP(); PUSH(a-b); break; }
        case 0x7E: { i64 b=POP(),a=POP(); PUSH(a*b); break; }
        case 0x7F: { i64 b=POP(),a=POP(); if(!b){fprintf(stderr,"div/0\n");PUSH(0);}else PUSH(a/b); break; }
        case 0x80: { i64 b=POP(),a=POP(); if(!b){fprintf(stderr,"div/0\n");PUSH(0);}else PUSH((i64)(U64(a)/U64(b))); break; }
        case 0x81: { i64 b=POP(),a=POP(); if(!b){fprintf(stderr,"rem/0\n");PUSH(0);}else PUSH(a%b); break; }
        case 0x82: { i64 b=POP(),a=POP(); if(!b){fprintf(stderr,"rem/0\n");PUSH(0);}else PUSH((i64)(U64(a)%U64(b))); break; }
        case 0x83: { i64 b=POP(),a=POP(); PUSH(a&b); break; }
        case 0x84: { i64 b=POP(),a=POP(); PUSH(a|b); break; }
        case 0x85: { i64 b=POP(),a=POP(); PUSH(a^b); break; }
        case 0x86: { i64 b=POP(),a=POP(); PUSH(a<<(b&63)); break; }
        case 0x87: { i64 b=POP(),a=POP(); PUSH(a>>(b&63)); break; }
        case 0x88: { i64 b=POP(),a=POP(); PUSH((i64)(U64(a)>>(b&63))); break; }
        case 0xA7: { i64 v=POP(); PUSH(v&0xFFFFFFFF); break; }
        case 0xAC: { i64 v=POP()&0xFFFFFFFF; if(v>=0x80000000)v-=0x100000000LL; PUSH(v); break; }
        case 0xAD: { i64 v=POP(); PUSH(v&0xFFFFFFFF); break; }
        /* i64 unary: clz, ctz, popcnt */
        case 0x79: { i64 v=POP(); PUSH(v==0?64:__builtin_clzll(v)); break; } /* i64.clz */
        case 0x7A: { i64 v=POP(); PUSH(v==0?64:__builtin_ctzll(v)); break; } /* i64.ctz */
        case 0x7B: { i64 v=POP(); PUSH(__builtin_popcountll(v)); break; } /* i64.popcnt */
        /* i64 rotate */
        case 0x89: { i64 b=POP(),a=POP(); int s=(int)(b&63); PUSH((i64)(U64(a)<<s | U64(a)>>(64-s))); break; } /* i64.rotl */
        case 0x8A: { i64 b=POP(),a=POP(); int s=(int)(b&63); PUSH((i64)(U64(a)>>s | U64(a)<<(64-s))); break; } /* i64.rotr */
        /* i32 arithmetic (wasm i32 opcodes) */
        case 0x6A: { i64 b=POP(),a=POP(); PUSH((i64)(int32_t)((int32_t)a+(int32_t)b)); break; } /* i32.add */
        case 0x6B: { i64 b=POP(),a=POP(); PUSH((i64)(int32_t)((int32_t)a-(int32_t)b)); break; } /* i32.sub */
        case 0x6C: { i64 b=POP(),a=POP(); PUSH((i64)(int32_t)((int32_t)a*(int32_t)b)); break; } /* i32.mul */
        case 0x6D: { i64 b=POP(),a=POP(); if(!(int32_t)b){fprintf(stderr,"div/0\n");PUSH(0);}else PUSH((i64)((int32_t)a/(int32_t)b)); break; } /* i32.div_s */
        case 0x6F: { i64 b=POP(),a=POP(); if(!(int32_t)b){fprintf(stderr,"rem/0\n");PUSH(0);}else PUSH((i64)((int32_t)a%(int32_t)b)); break; } /* i32.rem_s */
        case 0x71: { i64 b=POP(),a=POP(); PUSH((a&b)&0xFFFFFFFF); break; } /* i32.and */
        case 0x72: { i64 b=POP(),a=POP(); PUSH((a|b)&0xFFFFFFFF); break; } /* i32.or */
        case 0x73: { i64 b=POP(),a=POP(); PUSH((a^b)&0xFFFFFFFF); break; } /* i32.xor */
        /* Float reinterpret (bit-cast) */
        case 0xB9: { i64 v=POP(); double d=(double)v; i64 r; memcpy(&r,&d,8); PUSH(r); break; } /* f64.convert_i64_s */
        case 0xBF: { i64 v=POP(); PUSH(v); break; } /* f64.reinterpret_i64 (nop for i64 stack — bits unchanged) */
        case 0xBD: { i64 v=POP(); PUSH(v); break; } /* i64.reinterpret_f64 (nop for i64 stack — bits unchanged) */
        case 0xBC: { i64 v=POP(); PUSH(v&0xFFFFFFFF); break; } /* i32.reinterpret_f32 */
        case 0xB0: { i64 v=POP(); double d; memcpy(&d,&v,8); PUSH((i64)d); break; } /* i64.trunc_f64_s */
        /* f64 arithmetic — operands are i64 bit patterns on stack */
        case 0x9F: { i64 v=POP(); double d; memcpy(&d,&v,8); d=sqrt(d); memcpy(&v,&d,8); PUSH(v); break; } /* f64.sqrt */
        case 0xA0: { i64 bv=POP(),av=POP(); double a,b,r; memcpy(&a,&av,8); memcpy(&b,&bv,8); r=a+b; i64 rv; memcpy(&rv,&r,8); PUSH(rv); break; } /* f64.add */
        case 0xA1: { i64 bv=POP(),av=POP(); double a,b,r; memcpy(&a,&av,8); memcpy(&b,&bv,8); r=a-b; i64 rv; memcpy(&rv,&r,8); PUSH(rv); break; } /* f64.sub */
        case 0xA2: { i64 bv=POP(),av=POP(); double a,b,r; memcpy(&a,&av,8); memcpy(&b,&bv,8); r=a*b; i64 rv; memcpy(&rv,&r,8); PUSH(rv); break; } /* f64.mul */
        case 0xA3: { i64 bv=POP(),av=POP(); double a,b,r; memcpy(&a,&av,8); memcpy(&b,&bv,8); r=b!=0?a/b:0; i64 rv; memcpy(&rv,&r,8); PUSH(rv); break; } /* f64.div */
        /* f64.const — 8 raw bytes */
        case 0x44: { i64 v=0; memcpy(&v, code+pos, 8); pos+=8; PUSH(v); break; } /* f64.const */
        default: break;
        }
    }
done:
    for(int i=nrets-1; i>=0; i--) rets[i] = sp>0 ? stack[--sp] : 0;
    free(stack);
    call_depth--;
    return;
}

/* ═══ File I/O ═══ */
static u8* read_file(const char *path, int *out_len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    *out_len = (int)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    u8 *buf = malloc(*out_len);
    if (buf) fread(buf, 1, *out_len, fp);
    fclose(fp);
    return buf;
}

/* ═══ Find compiler.wasm relative to binary ═══ */
static char* find_compiler_wasm(const char *argv0) {
    static char path[4096];
    /* Try 1: relative to binary — ../bootstrap/compiler.wasm */
    const char *slash = strrchr(argv0, '/');
    if (slash) {
        int dirlen = (int)(slash - argv0);
        snprintf(path, sizeof(path), "%.*s/bootstrap/compiler.wasm", dirlen, argv0);
        FILE *fp = fopen(path, "rb");
        if (fp) { fclose(fp); return path; }
        snprintf(path, sizeof(path), "%.*s/../bootstrap/compiler.wasm", dirlen, argv0);
        fp = fopen(path, "rb");
        if (fp) { fclose(fp); return path; }
    }
    /* Try 2: current directory */
    snprintf(path, sizeof(path), "bootstrap/compiler.wasm");
    FILE *fp = fopen(path, "rb");
    if (fp) { fclose(fp); return path; }
    /* Try 3: same directory as binary */
    if (slash) {
        int dirlen = (int)(slash - argv0);
        snprintf(path, sizeof(path), "%.*s/compiler.wasm", dirlen, argv0);
        fp = fopen(path, "rb");
        if (fp) { fclose(fp); return path; }
    }
    return NULL;
}

/* ═══ Build: compile .luon → .wasm using native compiler ═══ */
#include "luon_compile.h"

static int cmd_build(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: luon build <file.luon> [-o output.wasm]\n");
        return 1;
    }
    const char *src_path = argv[2];
    const char *out_path = NULL;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i+1 < argc) out_path = argv[++i];
    }
    static char auto_out[4096];
    if (!out_path) {
        strncpy(auto_out, src_path, sizeof(auto_out)-6);
        char *dot = strrchr(auto_out, '.');
        if (dot) strcpy(dot, ".wasm"); else strcat(auto_out, ".wasm");
        out_path = auto_out;
    }
    int src_len = 0;
    u8 *src = read_file(src_path, &src_len);
    if (!src) { fprintf(stderr, "Error: cannot open %s\n", src_path); return 1; }

    /* Set LUON_SRC_PATH for import resolution relative to source file */
    {
        static char src_dir[4096];
        strncpy(src_dir, src_path, sizeof(src_dir)-1);
        char *slash = strrchr(src_dir, '/');
        if (slash) { *slash = 0; } else { strcpy(src_dir, "."); }
        setenv("LUON_SRC_PATH", src_dir, 1);
    }

    /* Native compilation — no external dependencies */
    u8 *wasm_out = NULL;
    int wasm_len = 0;
    if (compile_luon((const char*)src, src_len, &wasm_out, &wasm_len) != 0 || !wasm_out) {
        fprintf(stderr, "Error: compilation failed\n");
        free(src); return 1;
    }

    FILE *fp = fopen(out_path, "wb");
    if (!fp) { 
        fprintf(stderr, "Error: cannot write %s\n", out_path); 
        free(src); free(wasm_out); return 1; 
    }
    fwrite(wasm_out, 1, wasm_len, fp);
    fclose(fp);

    printf("  ✓ Built %s (%d bytes, %s)\n", out_path, wasm_len, src_path);
    
    free(src);
    free(wasm_out);
    return 0;
}

/* ═══ Run: execute .wasm or .luon ═══ */
static int cmd_run(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: luon run <file.wasm|file.luon> [-a N] [-e entry]\n");
        return 1;
    }
    const char *path = argv[2];
    i64 arg = 0;
    const char *entry = "main";

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 && i+1 < argc) arg = atoll(argv[++i]);
        else if (strcmp(argv[i], "-e") == 0 && i+1 < argc) entry = argv[++i];
    }

    /* If .luon file, compile first then run */
    int path_len = (int)strlen(path);
    int is_luon = (path_len > 5 && strcmp(path + path_len - 5, ".luon") == 0);

    u8 *wasm_data = NULL;
    int wasm_len = 0;
    char tmp_path[4096] = {0};

    if (is_luon) {
        /* Build to temp, then run */
        snprintf(tmp_path, sizeof(tmp_path), "/tmp/_luon_tmp_%d.wasm", (int)getpid());
        /* Construct build args */
        char *build_argv[5];
        build_argv[0] = argv[0];
        build_argv[1] = "build";
        build_argv[2] = (char*)path;
        build_argv[3] = "-o";
        build_argv[4] = tmp_path;
        int rc = cmd_build(5, build_argv);
        if (rc != 0) return rc;
        wasm_data = read_file(tmp_path, &wasm_len);
        if (!wasm_data) { fprintf(stderr, "Error: build succeeded but cannot read output\n"); return 1; }
    } else {
        wasm_data = read_file(path, &wasm_len);
        if (!wasm_data) { fprintf(stderr, "Error: cannot open %s\n", path); return 1; }
    }

    if (wasm_len < 8 || memcmp(wasm_data, "\0asm", 4) != 0) {
        fprintf(stderr, "Error: invalid WASM file\n"); free(wasm_data); return 1;
    }

    Module m;
    if (parse_module(wasm_data, wasm_len, &m) != 0) {
        fprintf(stderr, "Error: parse failed\n"); free(wasm_data); return 1;
    }

    int fidx = find_export(&m, entry);
    if (fidx < 0) { fprintf(stderr, "Error: export '%s' not found\n", entry); free(wasm_data); free(m.memory); return 1; }

    int local_fidx = fidx - m.n_imports; /* adjust for imported functions */
    if (local_fidx < 0 || local_fidx >= m.n_funcs) { fprintf(stderr, "Error: invalid export index\n"); free(wasm_data); free(m.memory); return 1; }

    call_depth = 0;
    i64 run_args[1] = {arg};
    i64 run_rets[16] = {0};
    int nr = m.func_nreturns[local_fidx]; if(nr<1) nr=1;
    vm_exec(&m, local_fidx, run_args, 1, run_rets, nr);
    for(int i=0;i<nr;i++) {
        printf("%lld%s", (long long)run_rets[i], i==nr-1?"":" ");
    }
    printf("\n");

    free(wasm_data);
    free(m.memory);
    if (tmp_path[0]) remove(tmp_path);
    return 0;
}

/* ═══ Test: compile and run all .luon files in a directory ═══ */
static int cmd_test(int argc, char **argv) {
    (void)argc; (void)argv;
    int pass = 0, fail = 0;
    typedef struct { const char *file; i64 arg; i64 expected; } TestCase;
    TestCase tests[] = {
        {"examples/add42.luon", 10, 52},
        {"examples/add42.luon", 0, 42},
        {"examples/fibonacci.luon", 1, 1},
        {"examples/fibonacci.luon", 5, 5},
        {"examples/fibonacci.luon", 10, 55},
        {"examples/fibonacci.luon", 20, 6765},
        {"examples/test_pow.luon", 0, 64},
        {"examples/calculator.luon", 5, 70},
        {"examples/named_call_test.luon", 3, 20},
        {"examples/arithmetic.luon", 10, 190},
        {"examples/global_test.luon", 0, 42},
        {"examples/loop_test.luon", 10, 0},
        {NULL, 0, 0}
    };

    printf("  Luon Test Suite\n  ═══════════════\n\n");
    for (int t = 0; tests[t].file; t++) {
        int src_len = 0;
        u8 *src = read_file(tests[t].file, &src_len);
        if (!src) {
            printf("  ⚠ SKIP: %s (not found)\n", tests[t].file);
            continue;
        }
        u8 *wasm_out = NULL; int wasm_len = 0;
        if (compile_luon((const char*)src, src_len, &wasm_out, &wasm_len) != 0 || !wasm_out) {
            printf("  ❌ FAIL: %s (compile error)\n", tests[t].file);
            fail++; free(src); continue;
        }
        Module m;
        if (parse_module(wasm_out, wasm_len, &m) != 0) {
            printf("  ❌ FAIL: %s (parse error)\n", tests[t].file);
            fail++; free(src); free(wasm_out); continue;
        }
        int fidx = find_export(&m, "main");
        if (fidx < 0) {
            printf("  ❌ FAIL: %s (no main)\n", tests[t].file);
            fail++; free(src); free(wasm_out); free(m.memory); continue;
        }
        int local_fidx = fidx - m.n_imports;
        call_depth = 0;
        i64 args[1] = {tests[t].arg};
        i64 rets[16] = {0};
        int nr = m.func_nreturns[local_fidx]; if(nr<1) nr=1;
        vm_exec(&m, local_fidx, args, 1, rets, nr);
        if (rets[0] == tests[t].expected) {
            printf("  ✅ PASS: %s -a %lld = %lld\n", tests[t].file, (long long)tests[t].arg, (long long)rets[0]);
            pass++;
        } else {
            printf("  ❌ FAIL: %s -a %lld = %lld (expected %lld)\n", tests[t].file, (long long)tests[t].arg, (long long)rets[0], (long long)tests[t].expected);
            fail++;
        }
        free(src); free(wasm_out); free(m.memory);
    }
    printf("\n  Results: %d passed, %d failed\n", pass, fail);
    return fail > 0 ? 1 : 0;
}

/* ═══ Main CLI ═══ */
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("  Luon Compiler v2.2.0-dev\n");
        printf("  Vesege — Security-Oriented WASM Language\n\n");
        printf("  Usage:\n");
        printf("    luon build <file.luon> [-o out.wasm]  Compile .luon to .wasm\n");
        printf("    luon run   <file> [-a N] [-e entry]   Execute (auto-compiles .luon)\n");
        printf("    luon test                             Run built-in test suite\n");
        printf("    luon version                          Show version\n");
        printf("\n  No Python. No Rust. No dependencies. Pure Luon.\n");
        return 0;
    }

    if (strcmp(argv[1], "version") == 0) {
        printf("  Luon Compiler v2.2.0-dev\n");
        printf("  Vesege — Security-Oriented WASM Language\n");
        printf("  Runtime: Native C (zero dependencies)\n");
        printf("  Self-hosting: compiler.wasm (%s)\n",
               find_compiler_wasm(argv[0]) ? "found" : "not found");
        return 0;
    }

    if (strcmp(argv[1], "build") == 0) return cmd_build(argc, argv);
    if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "exec") == 0) return cmd_run(argc, argv);
    if (strcmp(argv[1], "test") == 0) return cmd_test(argc, argv);

    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    return 1;
}
