/* Native Luon Compiler - Minimal v2 EXTREME → WASM */
#ifndef LUON_COMPILE_H
#define LUON_COMPILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { uint8_t *d; int len, cap; } Buf;
static void buf_init(Buf *b) { b->cap=4096; b->d=calloc(1,b->cap); b->len=0; }
static void buf_byte(Buf *b, uint8_t v) {
    if(b->len>=b->cap){b->cap*=2;b->d=realloc(b->d,b->cap);}
    b->d[b->len++]=v;
}
static void buf_bytes(Buf *b, const uint8_t *s, int n) { for(int i=0;i<n;i++) buf_byte(b,s[i]); }
static void buf_uleb(Buf *b, uint64_t v) {
    do { uint8_t c=v&0x7f; v>>=7; if(v) c|=0x80; buf_byte(b,c); } while(v);
}
static void buf_sleb(Buf *b, int64_t v) {
    int more=1;
    while(more){uint8_t c=v&0x7f;v>>=7;if((v==0&&!(c&0x40))||(v==-1&&(c&0x40)))more=0;else c|=0x80;buf_byte(b,c);}
}

/* Pattern matching helpers */
static int has_str(const char *line, const char *pat) { return strstr(line,pat)!=NULL; }

static int64_t extract_number(const char *line) {
    /* Find number after comma or specific pattern markers */
    const char *p=line;
    const char *last_num=NULL;
    while(*p){
        if((*p=='-'&&p[1]>='0'&&p[1]<='9')||(*p>='0'&&*p<='9')){
            last_num=p;
            /* Find the number closest to a comma or operand position */
            const char *comma=strstr(line,", ");
            if(comma && p>comma) { return strtoll(p,NULL,10); }
            const char *paren=strrchr(line,')');
            if(!comma) return strtoll(p,NULL,10);
            p++;while(*p>='0'&&*p<='9')p++;
        } else p++;
    }
    if(last_num) return strtoll(last_num,NULL,10);
    return 0;
}

static int extract_sigma(const char *line) {
    /* Find σ followed by subscript digits or _N */
    const char *p=strstr(line,"\xcf\x83"); /* σ UTF-8 */
    if(!p) return -1;
    p+=2;
    /* Check for subscript digits (UTF-8: e2 82 8x where x=80-89 for ₀-₉) */
    int val=0; int found=0;
    while(p[0]==(char)0xe2 && p[1]==(char)0x82 && (uint8_t)p[2]>=0x80 && (uint8_t)p[2]<=0x89) {
        val = val*10 + ((uint8_t)p[2]-0x80);
        p+=3; found=1;
    }
    if(found) return val;
    if(*p=='_'){ p++; return atoi(p); }
    return -1;
}

/* Emit WASM opcodes for each IR operation */
#define GET1 buf_byte(b,0x20);buf_byte(b,0x01)
#define SET1 buf_byte(b,0x21);buf_byte(b,0x01)
#define TEE1 buf_byte(b,0x22);buf_byte(b,0x01)

static void emit_arith(Buf *b, uint8_t op, int has_k, int64_t k, int has_s, int s) {
    GET1;
    if(has_k){buf_byte(b,0x42);buf_sleb(b,k);}
    else if(has_s){buf_byte(b,0x20);buf_uleb(b,s);}
    buf_byte(b,op); SET1;
}
static void emit_cmp(Buf *b, uint8_t op, int has_k, int64_t k, int has_s, int s) {
    GET1;
    if(has_k){buf_byte(b,0x42);buf_sleb(b,k);}
    else if(has_s){buf_byte(b,0x20);buf_uleb(b,s);}
    buf_byte(b,op);buf_byte(b,0xad);SET1;
}

typedef struct { char name[64]; Buf body; int nparams; } LFunc;

static int compile_luon(const char *src, int src_len, uint8_t **out_wasm, int *out_len) {
    LFunc funcs[256]; int nf=0;
    LFunc *cur=NULL;
    char line[4096];
    int pos=0;

    while(pos<src_len) {
        int ll=0;
        while(pos<src_len && src[pos]!='\n') { if(ll<4094) line[ll++]=src[pos]; pos++; }
        line[ll]=0; pos++; /* skip \n */
        /* Trim */
        char *s=line; while(*s==' '||*s=='\t')s++;
        if(!*s||s[0]=='/'&&s[1]=='/') continue;
        /* Module decl */
        if(strstr(s,"\xe2\x88\x80")&&strstr(s,"\xf0\x9d\x94\x98")) continue; /* ∀...𝔘 */
        /* Function decl */
        if(strstr(s,"\xe2\x88\x83")&&strstr(s,"Hom")) { /* ∃...Hom */
            char *bracket=strchr(s,'[');
            if(bracket){
                cur=&funcs[nf++];
                char *end=strchr(bracket+1,']');
                int nl=end?(int)(end-bracket-1):0;
                if(nl>63)nl=63;
                memcpy(cur->name,bracket+1,nl);cur->name[nl]=0;
                buf_init(&cur->body);
                /* Count params: scan for highest σ_N in line after ']' */
                int max_p=0;
                const char *scan=end?end:s;
                while(*scan){
                    if((uint8_t)scan[0]==0xcf&&(uint8_t)scan[1]==0x83){
                        scan+=2; int pv=0; int pf=0;
                        while((uint8_t)scan[0]==0xe2&&(uint8_t)scan[1]==0x82&&(uint8_t)scan[2]>=0x80&&(uint8_t)scan[2]<=0x89){
                            pv=pv*10+((uint8_t)scan[2]-0x80); scan+=3; pf=1;
                        }
                        if(pf&&pv>max_p) max_p=pv;
                    } else scan++;
                }
                cur->nparams=max_p>0?max_p:1; /* at least acc (1 param) */
            }
            continue;
        }
        if(!cur) continue;
        if(s[0]=='}') continue;

        /* Split on semicolons outside braces */
        char *exprs[32]; int ne=0;
        int bd=0; char *ep=s;
        exprs[ne++]=s;
        for(char *c=s;*c;c++){
            if(*c=='{')bd++;else if(*c=='}')bd--;
            if(*c==';'&&bd==0){*c=0;if(c[1])exprs[ne++]=c+1;}
        }

        for(int ei=0;ei<ne;ei++){
            char *ex=exprs[ei]; while(*ex==' ')ex++;
            if(!*ex) continue;
            Buf *b=&cur->body;
            int64_t k=0; int sig=-1; int has_k=0,has_s=0;

            /* Extract operands */
            sig=extract_sigma(ex);
            if(sig>=0) has_s=1;

            if(has_str(ex,"Ext\xe2\x81\xb0")&&has_str(ex,"Spec")) { /* Add */
                k=extract_number(ex);has_k=1;emit_arith(b,0x7c,has_k,k,0,0);
            } else if(has_str(ex,"Tor\xe2\x82\x80")&&has_str(ex,"Spec")) { /* Sub */
                k=extract_number(ex);has_k=1;
                if(has_s&&sig>=0){emit_arith(b,0x7d,0,0,1,sig);}
                else{emit_arith(b,0x7d,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x8a\x97_\xe2\x84\xa4")&&has_str(ex,"\xe2\x8a\x97L")) { /* Mul */
                k=extract_number(ex);has_k=1;emit_arith(b,0x7e,has_k,k,0,0);
            } else if(has_str(ex,"RHom")) { /* Div */
                if(has_s)emit_arith(b,0x7f,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x7f,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xa1")&&has_str(ex,"acyclic")) { /* Eqz */
                GET1;buf_byte(b,0x50);buf_byte(b,0xad);SET1;
            } else if(has_str(ex,"\xe2\x89\xa1")&&has_str(ex,"d_r=0")) { /* Eq */
                if(has_s)emit_cmp(b,0x51,0,0,1,sig);else{k=extract_number(ex);emit_cmp(b,0x51,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xba")&&has_str(ex,"filtration")) { /* Lt */
                if(has_s)emit_cmp(b,0x53,0,0,1,sig);else{k=extract_number(ex);emit_cmp(b,0x53,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xbb")&&has_str(ex,"cofiltration")) { /* Gt */
                if(has_s)emit_cmp(b,0x55,0,0,1,sig);else{k=extract_number(ex);emit_cmp(b,0x55,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xbc")&&has_str(ex,"\xe2\x8a\x86""filt")) { /* Le */
                if(has_s)emit_cmp(b,0x57,0,0,1,sig);else{k=extract_number(ex);emit_cmp(b,0x57,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xbd")&&has_str(ex,"\xe2\x8a\x87""filt")) { /* Ge */
                if(has_s)emit_cmp(b,0x59,0,0,1,sig);else{k=extract_number(ex);emit_cmp(b,0x59,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x88\xa7_{\xf0\x9d\x94\xb9}")) { /* And */
                if(has_s)emit_arith(b,0x83,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x83,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x88\xa8_{\xf0\x9d\x94\xb9}")) { /* Or */
                if(has_s)emit_arith(b,0x84,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x84,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x8a\x95_{GF}")) { /* Xor */
                if(has_s)emit_arith(b,0x85,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x85,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xab_{shift}")&&has_str(ex,"s>>")) { /* Shr_S */
                if(has_s)emit_arith(b,0x87,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x87,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xab_{shift}")) { /* Shr */
                if(has_s)emit_arith(b,0x88,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x88,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x89\xaa_{shift}")) { /* Shl */
                if(has_s)emit_arith(b,0x86,0,0,1,sig);else{k=extract_number(ex);emit_arith(b,0x86,1,k,0,0);}
            } else if(has_str(ex,"\xe2\x8a\xa2_{\xce\x93;\xce\x94}")&&has_str(ex,"seq")) { /* Save_State */
                if(has_s){GET1;buf_byte(b,0x21);buf_uleb(b,sig);}
            } else if(has_str(ex,"\xe2\x8a\xa3_{\xce\x94;\xce\x93}")) { /* Load_State */
                if(has_s){buf_byte(b,0x20);buf_uleb(b,sig);SET1;}
                else{k=extract_number(ex);buf_byte(b,0x42);buf_sleb(b,k);SET1;}
            } else if(has_str(ex,"\xe2\x86\xa6\xe2\x82\x88")&&has_str(ex,"\xce\xba-forcing")) { /* Store8 */
                int ts=extract_sigma(strstr(ex,"\xe2\x86\xa6"));
                if(ts<0)ts=sig;
                GET1;buf_byte(b,0xa7);buf_byte(b,0x20);buf_uleb(b,ts);
                buf_byte(b,0x3c);buf_byte(b,0);buf_byte(b,0);
            } else if(has_str(ex,"\xe2\x86\xa6")&&has_str(ex,"\xce\xba-forcing")) { /* Store */
                int ts=extract_sigma(strstr(ex,"\xe2\x86\xa6"));
                if(ts<0){ k=extract_number(ex);
                    GET1;buf_byte(b,0xa7);buf_byte(b,0x42);buf_sleb(b,k);
                } else {
                    GET1;buf_byte(b,0xa7);buf_byte(b,0x20);buf_uleb(b,ts);
                }
                buf_byte(b,0x37);buf_byte(b,0);buf_byte(b,0);
            } else if(has_str(ex,"\xe2\x86\xa4\xe2\x82\x88")&&has_str(ex,"\xce\x93-generic")) { /* Load8 */
                GET1;buf_byte(b,0xa7);buf_byte(b,0x2d);buf_byte(b,0);buf_byte(b,0);
                buf_byte(b,0xad);SET1;
            } else if(has_str(ex,"\xe2\x86\xa4")&&has_str(ex,"\xce\x93-generic")) { /* Load */
                GET1;buf_byte(b,0xa7);buf_byte(b,0x29);buf_byte(b,0);buf_byte(b,0);SET1;
            } else if(has_str(ex,"\xe2\x8a\x83I")&&has_str(ex,"\xe2\x9f\xa6")) { /* Block_Begin */
                buf_byte(b,0x02);buf_byte(b,0x40);
            } else if(has_str(ex,"\xe2\x9f\xa7")&&has_str(ex,"\xe2\x8a\x83""E")) { /* Block_End */
                buf_byte(b,0x0b);
            } else if(has_str(ex,"\xce\xbc_{\xcf\x89")&&has_str(ex,"CK")) { /* Loop_Begin */
                buf_byte(b,0x03);buf_byte(b,0x40);
            } else if(has_str(ex,"\xe2\x9f\xa7_{\xcf\x89")&&has_str(ex,"CK")) { /* Loop_End */
                buf_byte(b,0x0b);
            } else if(has_str(ex,"\xe2\x8a\xac")&&has_str(ex,"G\xc3\xb6""del")&&has_str(ex,"\xe2\x88\x82")) { /* Conditional br_if */
                int depth=0;
                char *sp=strstr(ex,"\xcf\x83");
                if(sp){int ds=extract_sigma(sp);if(ds>=0)depth=ds;}
                GET1;buf_byte(b,0xa7);buf_byte(b,0x0d);buf_uleb(b,depth);
            } else if(has_str(ex,"\xe2\x8a\xac")&&has_str(ex,"G\xc3\xb6""del")) { /* Unconditional br */
                int depth=0;
                char *sp=strstr(ex,"\xcf\x83");
                if(sp){int ds=extract_sigma(sp);if(ds>=0)depth=ds;}
                buf_byte(b,0x0c);buf_uleb(b,depth);
            } else if(has_str(ex,"\xe2\x8a\xa5_{\xf0\x9d\x92\xaf}")&&has_str(ex,"ex falso")) { /* Return */
                GET1;buf_byte(b,0x0f);
            } else if(has_str(ex,"\xce\xb7_")&&has_str(ex,"Kan")) { /* Call */
                char *eta=strstr(ex,"\xce\xb7_");
                int fidx=0;
                if(eta){fidx=atoi(eta+3);}
                GET1;buf_byte(b,0x10);buf_uleb(b,fidx);SET1;
            } else if(has_str(ex,"H^n_{")&&has_str(ex,"Galois")) { /* Hash */
                GET1;buf_byte(b,0x42);buf_sleb(b,0x811c9dc5);
                buf_byte(b,0x85);buf_byte(b,0x42);buf_sleb(b,0x01000193);
                buf_byte(b,0x7e);SET1;
            }
            /* Floats, WASI, crypto stubs omitted for brevity - add as needed */
        }
    }

    if(nf==0){*out_wasm=NULL;*out_len=0;return -1;}

    /* Build WASM binary */
    Buf wasm; buf_init(&wasm);
    buf_bytes(&wasm,(const uint8_t*)"\0asm",4);
    uint8_t ver[]={1,0,0,0}; buf_bytes(&wasm,ver,4);

    /* Type section — emit types for 1..max_params */
    int max_p=1;
    for(int i=0;i<nf;i++) if(funcs[i].nparams>max_p) max_p=funcs[i].nparams;
    {
        Buf tsb; buf_init(&tsb);
        buf_uleb(&tsb,max_p); /* number of types */
        for(int np=1;np<=max_p;np++){
            buf_byte(&tsb,0x60); /* func type */
            buf_uleb(&tsb,np);   /* param count */
            for(int j=0;j<np;j++) buf_byte(&tsb,0x7e); /* i64 params */
            buf_uleb(&tsb,1); buf_byte(&tsb,0x7e); /* 1 i64 result */
        }
        buf_byte(&wasm,1);buf_uleb(&wasm,tsb.len);buf_bytes(&wasm,tsb.d,tsb.len);free(tsb.d);
    }
    /* Function section — map each func to type index (nparams-1) */
    Buf fs; buf_init(&fs);
    buf_uleb(&fs,nf);for(int i=0;i<nf;i++)buf_uleb(&fs,funcs[i].nparams-1);
    buf_byte(&wasm,3);buf_uleb(&wasm,fs.len);buf_bytes(&wasm,fs.d,fs.len);free(fs.d);
    /* Memory section */
    uint8_t ms_d[]={1,0,30};
    buf_byte(&wasm,5);buf_uleb(&wasm,3);buf_bytes(&wasm,ms_d,3);
    /* Export section */
    int main_idx=0;
    for(int i=0;i<nf;i++)if(strcmp(funcs[i].name,"main")==0){main_idx=i;break;}
    Buf es; buf_init(&es);
    buf_uleb(&es,2);
    buf_uleb(&es,6);buf_bytes(&es,(const uint8_t*)"memory",6);buf_byte(&es,2);buf_uleb(&es,0);
    buf_uleb(&es,4);buf_bytes(&es,(const uint8_t*)"main",4);buf_byte(&es,0);buf_uleb(&es,main_idx);
    buf_byte(&wasm,7);buf_uleb(&wasm,es.len);buf_bytes(&wasm,es.d,es.len);free(es.d);
    /* Code section */
    Buf cs; buf_init(&cs);
    buf_uleb(&cs,nf);
    for(int i=0;i<nf;i++){
        Buf body; buf_init(&body);
        int np=funcs[i].nparams;
        buf_uleb(&body,1);buf_uleb(&body,127);buf_byte(&body,0x7e); /* 127 i64 locals */
        /* Prologue: copy WASM params to register locals */
        /* param0 → local[1] (acc), param1 → local[2] (σ₂), etc. */
        for(int p=0;p<np;p++){
            buf_byte(&body,0x20);buf_uleb(&body,p);   /* local.get p */
            buf_byte(&body,0x21);buf_uleb(&body,p+1); /* local.set p+1 */
        }
        buf_bytes(&body,funcs[i].body.d,funcs[i].body.len);
        uint8_t epi[]={0x20,0x01,0x0b}; buf_bytes(&body,epi,3); /* epilogue */
        /* Peephole: set1+get1 → tee1 */
        Buf opt; buf_init(&opt);
        for(int j=0;j<body.len;j++){
            if(j+3<body.len&&body.d[j]==0x21&&body.d[j+1]==0x01&&body.d[j+2]==0x20&&body.d[j+3]==0x01){
                buf_byte(&opt,0x22);buf_byte(&opt,0x01);j+=3;
            } else buf_byte(&opt,body.d[j]);
        }
        buf_uleb(&cs,opt.len);buf_bytes(&cs,opt.d,opt.len);
        free(body.d);free(opt.d);free(funcs[i].body.d);
    }
    buf_byte(&wasm,10);buf_uleb(&wasm,cs.len);buf_bytes(&wasm,cs.d,cs.len);free(cs.d);

    *out_wasm=wasm.d; *out_len=wasm.len;
    return 0;
}
#endif
