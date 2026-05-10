/* Native Luon Compiler - Minimal v2 → WASM */
#ifndef LUON_COMPILE_H
#define LUON_COMPILE_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t *d;
  int len, cap;
} Buf;
static void buf_init(Buf *b) {
  b->cap = 4096;
  b->d = calloc(1, b->cap);
  b->len = 0;
}
static void buf_byte(Buf *b, uint8_t v) {
  if (b->len >= b->cap) {
    b->cap *= 2;
    b->d = realloc(b->d, b->cap);
  }
  b->d[b->len++] = v;
}
static void buf_bytes(Buf *b, const uint8_t *s, int n) {
  for (int i = 0; i < n; i++)
    buf_byte(b, s[i]);
}
static void buf_uleb(Buf *b, uint64_t v) {
  do {
    uint8_t c = v & 0x7f;
    v >>= 7;
    if (v)
      c |= 0x80;
    buf_byte(b, c);
  } while (v);
}
static void buf_sleb(Buf *b, int64_t v) {
  int more = 1;
  while (more) {
    uint8_t c = v & 0x7f;
    v >>= 7;
    if ((v == 0 && !(c & 0x40)) || (v == -1 && (c & 0x40)))
      more = 0;
    else
      c |= 0x80;
    buf_byte(b, c);
  }
}

/* Pattern matching helpers */
static int has_str(const char *line, const char *pat) {
  return strstr(line, pat) != NULL;
}

static int64_t extract_number(const char *line) {
  /* Find number after comma or specific pattern markers */
  const char *p = line;
  const char *last_num = NULL;
  while (*p) {
    if ((*p == '-' && p[1] >= '0' && p[1] <= '9') || (*p >= '0' && *p <= '9')) {
      last_num = p;
      /* Find the number closest to a comma or operand position */
      const char *comma = strstr(line, ", ");
      if (comma && p > comma) {
        return strtoll(p, NULL, 10);
      }
      const char *paren = strrchr(line, ')');
      if (!comma)
        return strtoll(p, NULL, 10);
      p++;
      while (*p >= '0' && *p <= '9')
        p++;
    } else
      p++;
  }
  if (last_num)
    return strtoll(last_num, NULL, 10);
  return 0;
}

static int extract_sigma(const char *line) {
  /* Find σ followed by subscript digits or _N */
  const char *p = strstr(line, "\xcf\x83"); /* σ UTF-8 */
  if (!p)
    return -1;
  p += 2;
  /* Check for subscript digits (UTF-8: e2 82 8x where x=80-89 for ₀-₉) */
  int val = 0;
  int found = 0;
  while (p[0] == (char)0xe2 && p[1] == (char)0x82 && (uint8_t)p[2] >= 0x80 &&
         (uint8_t)p[2] <= 0x89) {
    val = val * 10 + ((uint8_t)p[2] - 0x80);
    p += 3;
    found = 1;
  }
  if (found)
    return val;
  if (*p == '_') {
    p++;
    return atoi(p);
  }
  return -1;
}

/* Emit WASM opcodes for each IR operation */
#define GET1                                                                   \
  buf_byte(b, 0x20);                                                           \
  buf_byte(b, 0x01)
#define SET1                                                                   \
  buf_byte(b, 0x21);                                                           \
  buf_byte(b, 0x01)
#define TEE1                                                                   \
  buf_byte(b, 0x22);                                                           \
  buf_byte(b, 0x01)

static void emit_arith(Buf *b, uint8_t op, int has_k, int64_t k, int has_s,
                       int s) {
  GET1;
  if (has_k) {
    buf_byte(b, 0x42);
    buf_sleb(b, k);
  } else if (has_s) {
    buf_byte(b, 0x20);
    buf_uleb(b, s);
  }
  buf_byte(b, op);
  SET1;
}
static void emit_cmp(Buf *b, uint8_t op, int has_k, int64_t k, int has_s,
                     int s) {
  GET1;
  if (has_k) {
    buf_byte(b, 0x42);
    buf_sleb(b, k);
  } else if (has_s) {
    buf_byte(b, 0x20);
    buf_uleb(b, s);
  }
  buf_byte(b, op);
  buf_byte(b, 0xad);
  SET1;
}

typedef struct { char name[64]; Buf body; int nparams; int nreturns; uint8_t param_types[16]; uint8_t return_types[16]; } LFunc;
static int max_global_idx = -1; /* Track highest global used */

static int extract_global(const char *line) {
  /* Find 𝔾 (U+1D53E, UTF-8: F0 9D 94 BE) followed by subscript digits */
  const char *p = line;
  while (*p) {
    if ((uint8_t)p[0] == 0xf0 && (uint8_t)p[1] == 0x9d &&
        (uint8_t)p[2] == 0x94 && (uint8_t)p[3] == 0xbe) {
      p += 4;
      int val = 0;
      int found = 0;
      while ((uint8_t)p[0] == 0xe2 && (uint8_t)p[1] == 0x82 &&
             (uint8_t)p[2] >= 0x80 && (uint8_t)p[2] <= 0x89) {
        val = val * 10 + ((uint8_t)p[2] - 0x80);
        p += 3;
        found = 1;
      }
      if (found)
        return val;
      if (*p == '_') {
        p++;
        return atoi(p);
      }
      return 0;
    }
    p++;
  }
  return -1;
}

/* Parse WASM value type from mathematical notation */
/* Z64=i64(0x7e), Z32=i32(0x7f), F64=f64(0x7c), F32=f32(0x7d) */
static uint8_t parse_wasm_type(const char *p) {
  if ((uint8_t)p[0]==0xe2 && (uint8_t)p[1]==0x84 && (uint8_t)p[2]==0xa4) {
    const char *s = p+3;
    if ((uint8_t)s[0]==0xe2 && (uint8_t)s[1]==0x82) {
      if ((uint8_t)s[2]==0x83) return 0x7f;
      if ((uint8_t)s[2]==0x86) return 0x7e;
    }
    return 0x7e;
  }
  if ((uint8_t)p[0]==0xf0 && (uint8_t)p[1]==0x9d && (uint8_t)p[2]==0x94 && (uint8_t)p[3]==0xbd) {
    const char *s = p+4;
    if ((uint8_t)s[0]==0xe2 && (uint8_t)s[1]==0x82) {
      if ((uint8_t)s[2]==0x83) return 0x7d;
      if ((uint8_t)s[2]==0x86) return 0x7c;
    }
    return 0x7c;
  }
  return 0x7e;
}

/* Parse type annotation: :(Z64, Z32) -> Z64 */
static void parse_type_annotation(const char *line, LFunc *f) {
  for (int i=0; i<16; i++) { f->param_types[i]=0x7e; f->return_types[i]=0x7e; }
  const char *colon = strstr(line, ": (");
  if (!colon) return;
  const char *pp = strchr(colon, '(');
  if (!pp) return;
  pp++;
  int pi = 0;
  while (*pp && *pp != ')' && pi < 16) {
    if ((uint8_t)pp[0]==0xe2 || (uint8_t)pp[0]==0xf0) {
      f->param_types[pi++] = parse_wasm_type(pp);
    }
    pp++;
  }
  const char *arrow = strstr(pp, "\xe2\x86\x92");
  if (arrow) {
    const char *r = arrow+3;
    while (*r == ' ') r++;
    int ri = 0;
    while (*r && ri < 16) {
      if ((uint8_t)r[0]==0xe2 || (uint8_t)r[0]==0xf0) {
        f->return_types[ri++] = parse_wasm_type(r);
        break;
      }
      r++;
    }
  }
}

static int compile_luon(const char *in_src, int in_src_len, uint8_t **out_wasm,
                        int *out_len) {
  max_global_idx = -1;
  char *src = calloc(1, 1024 * 1024); /* 1MB buffer for combined source */
  int src_len = in_src_len;
  if (src_len > 1024 * 1024 - 1)
    src_len = 1024 * 1024 - 1;
  memcpy(src, in_src, src_len);

  /* Pass 0: Import Resolution */
  const char *p0 = src;
  while (p0 < src + src_len) {
    const char *eol = strchr(p0, '\n');
    if (!eol)
      eol = src + src_len;
    int ll = (int)(eol - p0);
    if (ll > 511)
      ll = 511;
    char line0[512] = {0};
    memcpy(line0, p0, ll);

    if (strstr(line0, "import") && strstr(line0, "\xf0\x9d\x94\x98[")) {
      char *nb = strstr(line0, "\xf0\x9d\x94\x98[");
      if (nb) {
        char *ne = strchr(nb + 5, ']');
        if (ne) {
          char modname[64] = {0};
          int nl = (int)(ne - nb - 5);
          if (nl > 63)
            nl = 63;
          memcpy(modname, nb + 5, nl);

          char filepath[256];
          snprintf(filepath, sizeof(filepath), "stdlib/%s.luon", modname);

          FILE *f = fopen(filepath, "rb");
          if (f) {
            fseek(f, 0, SEEK_END);
            long flen = ftell(f);
            fseek(f, 0, SEEK_SET);
            if (src_len + flen < 1024 * 1024) {
              fread(src + src_len, 1, flen, f);
              src_len += flen;
              src[src_len++] = '\n';
              src[src_len] = 0;
            }
            fclose(f);
          } else {
            printf("Warning: could not import module %s (file %s not found)\n",
                   modname, filepath);
          }
        }
      }
    }
    p0 = eol + 1;
  }

  LFunc funcs[256];
  int nf = 0;

  /* Pass 1: Discover functions and param counts */
  const char *p1 = src;
  while (p1 < src + src_len) {
    const char *eol = strchr(p1, '\n');
    if (!eol)
      eol = src + src_len;
    int ll = eol - p1;
    if (ll > 511)
      ll = 511;
    char line1[512] = {0};
    memcpy(line1, p1, ll);
    if (strstr(line1, "\xe2\x88\x83") && strstr(line1, "Hom")) {
      char *bracket = strchr(line1, '[');
      if (bracket) {
        LFunc *cf = &funcs[nf];
        char *end = strchr(bracket + 1, ']');
        int nl = end ? (int)(end - bracket - 1) : 0;
        if (nl > 63)
          nl = 63;
        memcpy(cf->name, bracket + 1, nl);
        cf->name[nl] = 0;
        int max_p = -1;
        const char *scan = p1; /* start from current line */
        while (scan < src + src_len) {
          /* If we hit another function signature or EOF, stop scanning for this
           * function's max_p */
          if (scan != p1 && scan[0] == '\n') {
            const char *nxt = scan + 1;
            if (nxt < src + src_len - 10) {
              if (memcmp(nxt, "\xe2\x88\x83", 3) ==
                  0) { /* next function starts */
                break;
              }
            }
          }
          if ((uint8_t)scan[0] == 0xcf && (uint8_t)scan[1] == 0x83) {
            scan += 2;
            int pv = 0;
            int pf = 0;
            while (scan < src + src_len && (uint8_t)scan[0] == 0xe2 &&
                   (uint8_t)scan[1] == 0x82 && (uint8_t)scan[2] >= 0x80 &&
                   (uint8_t)scan[2] <= 0x89) {
              pv = pv * 10 + ((uint8_t)scan[2] - 0x80);
              scan += 3;
              pf = 1;
            }
            if (pf && pv > max_p)
              max_p = pv;
          } else
            scan++;
        }
        int rets = 1;
        char *arrow = strstr(line1, "\xe2\x86\x92^{");
        if (arrow) {
          rets = atoi(arrow + 5);
          if (rets < 1)
            rets = 1;
        }
        cf->nreturns = rets;
        cf->nparams = max_p >= 0 ? max_p + 1 : 1;
        /* Parse type annotation if present */
        for (int ti=0; ti<16; ti++) { cf->param_types[ti]=0x7e; cf->return_types[ti]=0x7e; }
        parse_type_annotation(line1, cf);
        buf_init(&cf->body);
        nf++;
      }
    }
    p1 = eol + 1;
  }

  /* Pass 2: Parse bodies */
  int cur_fidx = 0;
  LFunc *cur = NULL;
  char line[4096];
  int pos = 0;

  typedef struct {
    int shadow_count;
    int shadows[8];
  } CBlock;
  CBlock blocks[64];
  int bp = 0;
  int scope_sp = 127;

  while (pos < src_len) {
    int ll = 0;
    while (pos < src_len && src[pos] != '\n') {
      if (ll < 4094)
        line[ll++] = src[pos];
      pos++;
    }
    line[ll] = 0;
    pos++; /* skip \n */
    /* Trim */
    char *s = line;
    while (*s == ' ' || *s == '\t')
      s++;
    if (!*s || s[0] == '/' && s[1] == '/')
      continue;
    /* Module decl */
    if (strstr(s, "\xe2\x88\x80") && strstr(s, "\xf0\x9d\x94\x98"))
      continue; /* ∀...𝔘 */
    /* Function decl */
    if (strstr(s, "\xe2\x88\x83") && strstr(s, "Hom")) { /* ∃...Hom */
      cur = &funcs[cur_fidx++];
      bp = 0;
      scope_sp = 127;
      continue;
    }
    if (!cur)
      continue;
    if (s[0] == '}')
      continue;

    /* Split on semicolons outside braces */
    char *exprs[32];
    int ne = 0;
    int bd = 0;
    char *ep = s;
    exprs[ne++] = s;
    for (char *c = s; *c; c++) {
      if (*c == '{')
        bd++;
      else if (*c == '}')
        bd--;
      if (*c == ';' && bd == 0) {
        *c = 0;
        if (c[1])
          exprs[ne++] = c + 1;
      }
    }

    for (int ei = 0; ei < ne; ei++) {
      char *ex = exprs[ei];
      while (*ex == ' ')
        ex++;
      if (!*ex)
        continue;
      Buf *b = &cur->body;
      int64_t k = 0;
      int sig = -1;
      int has_k = 0, has_s = 0;

      /* Extract operands */
      sig = extract_sigma(ex);
      if (sig >= 0)
        has_s = 1;

      if (has_str(ex, "Ext\xe2\x81\xb0") && has_str(ex, "Spec")) { /* Add */
        if (has_s)
          emit_arith(b, 0x7c, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x7c, 1, k, 0, 0);
        }
      } else if (has_str(ex, "Tor\xe2\x82\x80") &&
                 has_str(ex, "Spec")) { /* Sub */
        k = extract_number(ex);
        has_k = 1;
        if (has_s && sig >= 0) {
          emit_arith(b, 0x7d, 0, 0, 1, sig);
        } else {
          emit_arith(b, 0x7d, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x8a\x97_\xe2\x84\xa4") &&
                 has_str(ex, "\xe2\x8a\x97L")) { /* Mul */
        if (has_s)
          emit_arith(b, 0x7e, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x7e, 1, k, 0, 0);
        }
      } else if (has_str(ex, "RHom")) { /* Div */
        if (has_s)
          emit_arith(b, 0x7f, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x7f, 1, k, 0, 0);
        }
      } else if (has_str(ex, "mod_{\xe2\x84\xa4}")) { /* Mod */
        if (has_s)
          emit_arith(b, 0x81, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x81, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xa1") &&
                 has_str(ex, "acyclic")) { /* Eqz */
        GET1;
        buf_byte(b, 0x50);
        buf_byte(b, 0xad);
        SET1;
      } else if (has_str(ex, "\xe2\x89\xa1") && has_str(ex, "d_r=0")) { /* Eq */
        if (has_s)
          emit_cmp(b, 0x51, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x51, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xba") &&
                 has_str(ex, "filtration")) { /* Lt */
        if (has_s)
          emit_cmp(b, 0x53, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x53, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xba") &&
                 has_str(ex, "well-order")) { /* Lt (well-order) */
        if (has_s)
          emit_cmp(b, 0x53, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x53, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xbb") &&
                 has_str(ex, "cofiltration")) { /* Gt */
        if (has_s)
          emit_cmp(b, 0x55, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x55, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xbb") &&
                 has_str(ex, "well-order")) { /* Gt (well-order) */
        if (has_s)
          emit_cmp(b, 0x55, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x55, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xbc") && has_str(ex, "\xe2\x8a\x86"
                                                            "filt")) { /* Le */
        if (has_s)
          emit_cmp(b, 0x57, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x57, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xbd") && has_str(ex, "\xe2\x8a\x87"
                                                            "filt")) { /* Ge */
        if (has_s)
          emit_cmp(b, 0x59, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x59, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\xaa\xaf") &&
                 has_str(ex, "well-order")) { /* Le (well-order) */
        if (has_s)
          emit_cmp(b, 0x57, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x57, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\xaa\xb0") &&
                 has_str(ex, "well-order")) { /* Ge (well-order) */
        if (has_s)
          emit_cmp(b, 0x59, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_cmp(b, 0x59, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x88\xa7_{\xf0\x9d\x94\xb9}")) { /* And */
        if (has_s)
          emit_arith(b, 0x83, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x83, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x88\xa8_{\xf0\x9d\x94\xb9}")) { /* Or */
        if (has_s)
          emit_arith(b, 0x84, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x84, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x8a\x95_{GF}")) { /* Xor */
        if (has_s)
          emit_arith(b, 0x85, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x85, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xab_{shift}") &&
                 has_str(ex, "s>>")) { /* Shr_S */
        if (has_s)
          emit_arith(b, 0x87, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x87, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xab_{shift}")) { /* Shr */
        if (has_s)
          emit_arith(b, 0x88, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x88, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x89\xaa_{shift}")) { /* Shl */
        if (has_s)
          emit_arith(b, 0x86, 0, 0, 1, sig);
        else {
          k = extract_number(ex);
          emit_arith(b, 0x86, 1, k, 0, 0);
        }
      } else if (has_str(ex, "\xe2\x8a\xa2_{\xce\x93;\xce\x94}") &&
                 has_str(ex, "seq") && !has_str(ex, "\xf0\x9d\x94\xbe")) { /* Save_State */
        if (has_s) {
          GET1;
          buf_byte(b, 0x21);
          buf_uleb(b, sig);
        }
      } else if (has_str(ex,
                         "\xe2\x8a\xa3_{\xce\x94;\xce\x93}") && !has_str(ex, "\xf0\x9d\x94\xbe")) { /* Load_State */
        if (has_s) {
          buf_byte(b, 0x20);
          buf_uleb(b, sig);
          SET1;
        } else {
          k = extract_number(ex);
          buf_byte(b, 0x42);
          buf_sleb(b, k);
          SET1;
        }
      } else if (has_str(ex, "\xe2\x86\xa6\xe2\x82\x88") &&
                 has_str(ex, "\xce\xba-forcing")) { /* Store8 */
        int ts = extract_sigma(strstr(ex, "\xe2\x86\xa6"));
        if (ts < 0)
          ts = sig;
        GET1;
        buf_byte(b, 0xa7);
        buf_byte(b, 0x20);
        buf_uleb(b, ts);
        buf_byte(b, 0x3c);
        buf_byte(b, 0);
        buf_byte(b, 0);
      } else if (has_str(ex, "\xe2\x86\xa6") &&
                 has_str(ex, "\xce\xba-forcing")) { /* Store */
        int ts = extract_sigma(strstr(ex, "\xe2\x86\xa6"));
        if (ts < 0) {
          k = extract_number(ex);
          GET1;
          buf_byte(b, 0xa7);
          buf_byte(b, 0x42);
          buf_sleb(b, k);
        } else {
          GET1;
          buf_byte(b, 0xa7);
          buf_byte(b, 0x20);
          buf_uleb(b, ts);
        }
        buf_byte(b, 0x37);
        buf_byte(b, 0);
        buf_byte(b, 0);
      } else if (has_str(ex, "\xe2\x86\xa4\xe2\x82\x88") &&
                 has_str(ex, "\xce\x93-generic")) { /* Load8 */
        GET1;
        buf_byte(b, 0xa7);
        buf_byte(b, 0x2d);
        buf_byte(b, 0);
        buf_byte(b, 0);
        buf_byte(b, 0xad);
        SET1;
      } else if (has_str(ex, "\xe2\x86\xa4") &&
                 has_str(ex, "\xce\x93-generic")) { /* Load */
        GET1;
        buf_byte(b, 0xa7);
        buf_byte(b, 0x29);
        buf_byte(b, 0);
        buf_byte(b, 0);
        SET1;
      } else if (has_str(
                     ex,
                     "\xe2\x8a\xa3_{\x73\x63\x6f\x70\x65}")) { /* ⊣_{scope} */
        const char *scan = ex;
        if (bp < 64)
          blocks[bp].shadow_count = 0;
        while (*scan && *scan != '{') {
          if ((uint8_t)scan[0] == 0xcf && (uint8_t)scan[1] == 0x83) {
            scan += 2;
            int pv = 0;
            int fd = 0;
            while ((uint8_t)scan[0] == 0xe2 && (uint8_t)scan[1] == 0x82 &&
                   (uint8_t)scan[2] >= 0x80 && (uint8_t)scan[2] <= 0x89) {
              pv = pv * 10 + ((uint8_t)scan[2] - 0x80);
              scan += 3;
              fd = 1;
            }
            if (fd && bp < 64 && blocks[bp].shadow_count < 8) {
              scope_sp--;
              buf_byte(b, 0x20);
              buf_uleb(b, pv);
              buf_byte(b, 0x21);
              buf_uleb(b, scope_sp);
              blocks[bp].shadows[blocks[bp].shadow_count++] = pv;
            }
          } else if (strstr(scan, "\xe2\x88\x82_\xce\xa9") ==
                     scan) { /* ∂_Ω Accumulator */
            scan += 6;
            if (bp < 64 && blocks[bp].shadow_count < 8) {
              scope_sp--;
              buf_byte(b, 0x20);
              buf_uleb(b, 1);
              buf_byte(b, 0x21);
              buf_uleb(b, scope_sp);
              blocks[bp].shadows[blocks[bp].shadow_count++] = 1;
            }
          } else
            scan++;
        }
        buf_byte(b, 0x02);
        buf_byte(b, 0x40); /* block empty */
        if (bp < 64)
          bp++;
      } else if (has_str(
                     ex,
                     "\xe2\x9f\xa7_{\x73\x63\x6f\x70\x65}")) { /* ⟧_{scope} */
        if (bp > 0) {
          bp--;
          for (int i = blocks[bp].shadow_count - 1; i >= 0; i--) {
            buf_byte(b, 0x20);
            buf_uleb(b, scope_sp);
            buf_byte(b, 0x21);
            buf_uleb(b, blocks[bp].shadows[i]);
            scope_sp++;
          }
        }
        buf_byte(b, 0x0b);
      } else if (has_str(ex, "\xe2\x8a\x83I") &&
                 has_str(ex, "\xe2\x9f\xa6")) { /* Block_Begin */
        if (bp < 64) {
          blocks[bp].shadow_count = 0;
          bp++;
        }
        buf_byte(b, 0x02);
        buf_byte(b, 0x40);
      } else if (has_str(ex, "\xe2\x9f\xa7") &&
                 has_str(ex, "\xe2\x8a\x83"
                             "E")) { /* Block_End */
        if (bp > 0) {
          bp--;
          for (int i = blocks[bp].shadow_count - 1; i >= 0; i--) {
            buf_byte(b, 0x20);
            buf_uleb(b, scope_sp);
            buf_byte(b, 0x21);
            buf_uleb(b, blocks[bp].shadows[i]);
            scope_sp++;
          }
        }
        buf_byte(b, 0x0b);
      } else if (has_str(ex, "\xce\xbc_{\xcf\x89") &&
                 has_str(ex, "CK")) { /* Loop_Begin */
        if (bp < 64) {
          blocks[bp].shadow_count = 0;
          bp++;
        }
        buf_byte(b, 0x03);
        buf_byte(b, 0x40);
      } else if (has_str(ex, "\xe2\x9f\xa7_{\xcf\x89") &&
                 has_str(ex, "CK")) { /* Loop_End */
        if (bp > 0) {
          bp--;
          for (int i = blocks[bp].shadow_count - 1; i >= 0; i--) {
            buf_byte(b, 0x20);
            buf_uleb(b, scope_sp);
            buf_byte(b, 0x21);
            buf_uleb(b, blocks[bp].shadows[i]);
            scope_sp++;
          }
        }
        buf_byte(b, 0x0b);
      } else if (has_str(ex, "\xe2\x8a\xac") &&
                 has_str(ex, "G\xc3\xb6"
                             "del") &&
                 has_str(ex, "\xe2\x88\x82")) { /* Conditional br_if */
        int depth = 0;
        char *sp = strstr(ex, "\xcf\x83");
        if (sp) {
          int ds = extract_sigma(sp);
          if (ds >= 0)
            depth = ds;
        }
        GET1;
        buf_byte(b, 0xa7);
        buf_byte(b, 0x0d);
        buf_uleb(b, depth);
      } else if (has_str(ex, "\xe2\x8a\xac") &&
                 has_str(ex, "G\xc3\xb6"
                             "del")) { /* Unconditional br */
        int depth = 0;
        char *sp = strstr(ex, "\xcf\x83");
        if (sp) {
          int ds = extract_sigma(sp);
          if (ds >= 0)
            depth = ds;
        }
        buf_byte(b, 0x0c);
        buf_uleb(b, depth);
      } else if (has_str(ex, "\xe2\x8a\x9e") &&
                 has_str(ex, "br_table")) { /* ⊞ br_table N-way dispatch */
        /* Syntax: ⊞_{Π}^{br_table}(N, d)
         * N = number of sequential targets (0..N-1), d = default depth
         * Emits: local.get 1; i32.wrap; br_table [0,1,...,N-1] d */
        int n_targets = 0, def_depth = 0;
        const char *paren = strchr(ex, '(');
        if (paren) {
          n_targets = atoi(paren + 1);
          const char *comma = strchr(paren, ',');
          if (comma) def_depth = atoi(comma + 1);
          else def_depth = n_targets;
        }
        GET1; buf_byte(b, 0xa7); /* i32.wrap_i64 */
        buf_byte(b, 0x0e); /* br_table */
        buf_uleb(b, n_targets); /* vec length */
        for (int t = 0; t < n_targets; t++) buf_uleb(b, t);
        buf_uleb(b, def_depth); /* default */
      } else if (has_str(ex, "\xce\xb7_") &&
                 has_str(ex, "indirect")) { /* η_{indirect}[N] — call_indirect */
        /* Higher-order function call: call function at table index in acc
         * Syntax: η_{indirect}[type_idx] */
        int type_idx = 0;
        char *nb = strchr(ex, '[');
        if (nb) type_idx = atoi(nb + 1);
        GET1; buf_byte(b, 0xa7); /* i32.wrap_i64 */
        buf_byte(b, 0x11); /* call_indirect */
        buf_uleb(b, type_idx); /* type index */
        buf_uleb(b, 0); /* table index (always 0) */
      } else if (has_str(ex, "\xe2\x8a\xa5_{\xf0\x9d\x92\xaf}") &&
                 has_str(ex, "ex falso")) { /* Return */
        for (int i = 1; i <= cur->nreturns; i++) {
          buf_byte(b, 0x20);
          buf_uleb(b, i); /* local.get i */
        }
        buf_byte(b, 0x0f);
      } else if (has_str(ex, "\xce\xb7_") && has_str(ex, "Kan")) { /* Call */
        char *eta = strstr(ex, "\xce\xb7_");
        int fidx = 0;
        if (eta) {
          /* Try η_{Kan}[name] (call by name) first */
          char *nb = strstr(eta, "[");
          if (nb) {
            char *ne = strchr(nb + 1, ']');
            if (ne) {
              char fname[64] = {0};
              int fnl = (int)(ne - nb - 1);
              if (fnl > 63)
                fnl = 63;
              memcpy(fname, nb + 1, fnl);
              fidx = -1;
              for (int fi = 0; fi < nf; fi++) {
                if (strcmp(funcs[fi].name, fname) == 0) {
                  fidx = fi;
                  break;
                }
              }
              if (fidx < 0) {
                fidx = 0;
              } /* fallback to 0 if not found */
            }
          } else {
            /* η_{Kan}N (call by index) - backward compatible */
            char *num = eta + 3;
            while (*num && !(*num >= '0' && *num <= '9'))
              num++;
            fidx = atoi(num);
          }
        }
        int target_np = 1;
        int target_nr = 1;
        if (fidx >= 0 && fidx < nf) {
          target_np = funcs[fidx].nparams;
          target_nr = funcs[fidx].nreturns;
        }
        for (int p = 0; p < target_np; p++) {
          buf_byte(b, 0x20);
          buf_uleb(b, p + 1); /* local.get p+1 */
        }
        buf_byte(b, 0x10);
        buf_uleb(b, fidx);
        for (int r = target_nr; r >= 1; r--) {
          buf_byte(b, 0x21);
          buf_uleb(b, r); /* local.set r */
        }
      } else if (has_str(ex, "H^n_{") && has_str(ex, "Galois")) { /* Hash */
        GET1;
        buf_byte(b, 0x42);
        buf_sleb(b, 0x811c9dc5);
        buf_byte(b, 0x85);
        buf_byte(b, 0x42);
        buf_sleb(b, 0x01000193);
        buf_byte(b, 0x7e);
        SET1;
      } else if (has_str(ex, "\xf0\x9d\x94\xbe") &&
                 has_str(ex, "\xe2\x8a\xa2")) { /* Global Set: acc → 𝔾ₙ */
        int gi = extract_global(ex);
        if (gi >= 0) {
          if (gi > max_global_idx) max_global_idx = gi;
          GET1;
          buf_byte(b, 0x24);
          buf_uleb(b, gi);
        }
      } else if (has_str(ex, "\xf0\x9d\x94\xbe") &&
                 has_str(ex, "\xe2\x8a\xa3")) { /* Global Get: 𝔾ₙ → acc */
        int gi = extract_global(ex);
        if (gi >= 0) {
          if (gi > max_global_idx) max_global_idx = gi;
          buf_byte(b, 0x23);
          buf_uleb(b, gi);
          SET1;
        }
      } else if (has_str(ex, "\xf0\x9d\x94\xbe")) { /* Global bare ref: load */
        int gi = extract_global(ex);
        if (gi >= 0) {
          if (gi > max_global_idx) max_global_idx = gi;
          buf_byte(b, 0x23);
          buf_uleb(b, gi);
          SET1;
        }
      }
      /* === Additional WASM Opcodes === */
      /* Select (ternary): ⊤⊥_{select}(σ_true, σ_false) */
      else if (has_str(ex, "select")) {
        int st = 0, sf = 0;
        char *sp1 = strstr(ex, "\xcf\x83");
        if (sp1) { st = extract_sigma(sp1); sp1 += 2; while(*sp1 && (uint8_t)*sp1 >= 0x80) sp1++; }
        char *sp2 = sp1 ? strstr(sp1, "\xcf\x83") : NULL;
        if (sp2) sf = extract_sigma(sp2);
        buf_byte(b, 0x20); buf_uleb(b, st > 0 ? st : 1); /* true val */
        buf_byte(b, 0x20); buf_uleb(b, sf > 0 ? sf : 2); /* false val */
        GET1; buf_byte(b, 0xa7); /* condition i32.wrap */
        buf_byte(b, 0x1b); /* select */
        SET1;
      }
      /* Unreachable trap: ⊥_{trap} */
      else if (has_str(ex, "trap")) {
        buf_byte(b, 0x00); /* unreachable */
      }
      /* Drop: ⊥_{drop} */
      else if (has_str(ex, "drop") && !has_str(ex, "Kan")) {
        GET1;
        buf_byte(b, 0x1a); /* drop */
      }
      /* Memory size: μ_{pages} */
      else if (has_str(ex, "\xce\xbc") && has_str(ex, "pages")) {
        buf_byte(b, 0x3f); buf_byte(b, 0x00); /* memory.size */
        buf_byte(b, 0xac); /* i64.extend_i32_s */
        SET1;
      }
      /* Memory grow: μ_{grow} */
      else if (has_str(ex, "\xce\xbc") && has_str(ex, "grow")) {
        GET1; buf_byte(b, 0xa7); /* i32.wrap */
        buf_byte(b, 0x40); buf_byte(b, 0x00); /* memory.grow */
        buf_byte(b, 0xac); /* i64.extend_i32_s */
        SET1;
      }
      /* Type conversions */
      /* ℤ₆₄←ℤ₃₂: i64.extend_i32_s */
      else if (has_str(ex, "extend") && has_str(ex, "i32")) {
        GET1; buf_byte(b, 0xa7); /* i32.wrap first */
        buf_byte(b, 0xac); /* i64.extend_i32_s */
        SET1;
      }
      /* ℤ₃₂←ℤ₆₄: i32.wrap_i64 (result in i64 via extend) */
      else if (has_str(ex, "wrap") && has_str(ex, "i64")) {
        GET1; buf_byte(b, 0xa7); /* i32.wrap_i64 */
        buf_byte(b, 0xac); /* i64.extend back */
        SET1;
      }
      /* 𝔽₆₄←ℤ₆₄: f64.convert_i64_s */
      else if (has_str(ex, "f64_from_i64")) {
        GET1;
        buf_byte(b, 0xb9); /* f64.convert_i64_s */
        buf_byte(b, 0xbd); /* i64.reinterpret_f64 */
        SET1;
      }
      /* ℤ₆₄←𝔽₆₄: i64.trunc_f64_s */
      else if (has_str(ex, "i64_from_f64")) {
        GET1;
        buf_byte(b, 0xbf); /* f64.reinterpret_i64 */
        buf_byte(b, 0xb0); /* i64.trunc_f64_s */
        SET1;
      }
      /* === Null Safety: Option Type (⊘) === */
      /* ⊘_{none} — create None value (sentinel = INT64_MIN) */
      else if (has_str(ex, "\xe2\x8a\x98") && has_str(ex, "none")) {
        buf_byte(b, 0x42); /* i64.const */
        buf_sleb(b, (int64_t)0x8000000000000000LL); /* INT64_MIN sentinel */
        SET1;
      }
      /* ⊘_{some} — create Some(acc): value stays as-is */
      else if (has_str(ex, "\xe2\x8a\x98") && has_str(ex, "some")) {
        /* acc already holds the value — Some is identity at runtime */
      }
      /* ⊘_{unwrap!} — forced unwrap: trap if None */
      else if (has_str(ex, "\xe2\x8a\x98") && has_str(ex, "unwrap")) {
        GET1;
        buf_byte(b, 0x42); buf_sleb(b, (int64_t)0x8000000000000000LL);
        buf_byte(b, 0x51); /* i64.eq — check if None sentinel */
        buf_byte(b, 0x04); buf_byte(b, 0x40); /* if (is None) */
        buf_byte(b, 0x00); /* unreachable — trap! */
        buf_byte(b, 0x0b); /* end if */
      }
      /* ⊘_{is_none} — push 1 if None, 0 if Some */
      else if (has_str(ex, "\xe2\x8a\x98") && has_str(ex, "is_none")) {
        GET1;
        buf_byte(b, 0x42); buf_sleb(b, (int64_t)0x8000000000000000LL);
        buf_byte(b, 0x51); /* i64.eq */
        buf_byte(b, 0xac); /* i64.extend_i32_s */
        SET1;
      }
      /* ⊘_{is_some} — push 1 if Some, 0 if None */
      else if (has_str(ex, "\xe2\x8a\x98") && has_str(ex, "is_some")) {
        GET1;
        buf_byte(b, 0x42); buf_sleb(b, (int64_t)0x8000000000000000LL);
        buf_byte(b, 0x52); /* i64.ne */
        buf_byte(b, 0xac); /* i64.extend_i32_s */
        SET1;
      }
    }
  }

  if (nf == 0) {
    *out_wasm = NULL;
    *out_len = 0;
    free(src);
    return -1;
  }

  /* Build WASM binary */
  Buf wasm;
  buf_init(&wasm);
  buf_bytes(&wasm, (const uint8_t *)"\0asm", 4);
  uint8_t ver[] = {1, 0, 0, 0};
  buf_bytes(&wasm, ver, 4);

  /* Type section — emit types for 1..max_params */
  /* Type section */
  int num_types = 0;
  int type_params[256];
  int type_returns[256];
  for (int i = 0; i < nf; i++) {
    int found = 0;
    for (int j = 0; j < num_types; j++) {
      if (type_params[j] == funcs[i].nparams &&
          type_returns[j] == funcs[i].nreturns) {
        found = 1;
        break;
      }
    }
    if (!found) {
      type_params[num_types] = funcs[i].nparams;
      type_returns[num_types] = funcs[i].nreturns;
      num_types++;
    }
  }
  {
    Buf tsb;
    buf_init(&tsb);
    buf_uleb(&tsb, num_types);
    for (int i = 0; i < num_types; i++) {
      buf_byte(&tsb, 0x60);           /* func type */
      buf_uleb(&tsb, type_params[i]); /* param count */
      /* Find first func with this signature for types */
      int ref = -1;
      for (int f = 0; f < nf; f++) {
        if (funcs[f].nparams == type_params[i] && funcs[f].nreturns == type_returns[i]) {
          ref = f; break;
        }
      }
      for (int j = 0; j < type_params[i]; j++)
        buf_byte(&tsb, (ref >= 0 && j < 16) ? funcs[ref].param_types[j] : 0x7e);
      buf_uleb(&tsb, type_returns[i]); /* result count */
      for (int j = 0; j < type_returns[i]; j++)
        buf_byte(&tsb, (ref >= 0 && j < 16) ? funcs[ref].return_types[j] : 0x7e);
    }
    buf_byte(&wasm, 1);
    buf_uleb(&wasm, tsb.len);
    buf_bytes(&wasm, tsb.d, tsb.len);
    free(tsb.d);
  }
  /* Function section — map each func to its unique type index */
  Buf fs;
  buf_init(&fs);
  buf_uleb(&fs, nf);
  for (int i = 0; i < nf; i++) {
    for (int j = 0; j < num_types; j++) {
      if (type_params[j] == funcs[i].nparams &&
          type_returns[j] == funcs[i].nreturns) {
        buf_uleb(&fs, j);
        break;
      }
    }
  }
  buf_byte(&wasm, 3);
  buf_uleb(&wasm, fs.len);
  buf_bytes(&wasm, fs.d, fs.len);
  free(fs.d);
  /* Memory section (ID=5) */
  uint8_t ms_d[] = {1, 0, 30};
  buf_byte(&wasm, 5);
  buf_uleb(&wasm, 3);
  buf_bytes(&wasm, ms_d, 3);
  /* Global section (ID=6) — emit if globals used */
  if (max_global_idx >= 0) {
    Buf gs;
    buf_init(&gs);
    buf_uleb(&gs, max_global_idx + 1);
    for (int i = 0; i <= max_global_idx; i++) {
      buf_byte(&gs, 0x7e); /* i64 */
      buf_byte(&gs, 0x01); /* mutable */
      buf_byte(&gs, 0x42);
      buf_sleb(&gs, 0);    /* i64.const 0 */
      buf_byte(&gs, 0x0b); /* end */
    }
    buf_byte(&wasm, 6);
    buf_uleb(&wasm, gs.len);
    buf_bytes(&wasm, gs.d, gs.len);
    free(gs.d);
  }
  /* Export section */
  int main_idx = 0;
  for (int i = 0; i < nf; i++)
    if (strcmp(funcs[i].name, "main") == 0) {
      main_idx = i;
      break;
    }
  Buf es;
  buf_init(&es);
  buf_uleb(&es, 2);
  buf_uleb(&es, 6);
  buf_bytes(&es, (const uint8_t *)"memory", 6);
  buf_byte(&es, 2);
  buf_uleb(&es, 0);
  buf_uleb(&es, 4);
  buf_bytes(&es, (const uint8_t *)"main", 4);
  buf_byte(&es, 0);
  buf_uleb(&es, main_idx);
  buf_byte(&wasm, 7);
  buf_uleb(&wasm, es.len);
  buf_bytes(&wasm, es.d, es.len);
  free(es.d);
  /* Code section */
  Buf cs;
  buf_init(&cs);
  buf_uleb(&cs, nf);
  for (int i = 0; i < nf; i++) {
    Buf body;
    buf_init(&body);
    int np = funcs[i].nparams;
    buf_uleb(&body, 1);
    buf_uleb(&body, 127);
    buf_byte(&body, 0x7e); /* 127 i64 locals */
    /* Prologue: copy WASM params to register locals */
    /* param0 → local[1] (acc), param1 → local[2] (σ₂), etc. */
    for (int p = np - 1; p >= 0; p--) {
      buf_byte(&body, 0x20);
      buf_uleb(&body, p); /* local.get p */
      buf_byte(&body, 0x21);
      buf_uleb(&body, p + 1); /* local.set p+1 */
    }
    buf_bytes(&body, funcs[i].body.d, funcs[i].body.len);
    uint8_t epi[] = {0x20, 0x01, 0x0b};
    buf_bytes(&body, epi, 3); /* epilogue */
    /* Peephole: set1+get1 → tee1 */
    Buf opt;
    buf_init(&opt);
    for (int j = 0; j < body.len; j++) {
      if (j + 3 < body.len && body.d[j] == 0x21 && body.d[j + 1] == 0x01 &&
          body.d[j + 2] == 0x20 && body.d[j + 3] == 0x01) {
        buf_byte(&opt, 0x22);
        buf_byte(&opt, 0x01);
        j += 3;
      } else
        buf_byte(&opt, body.d[j]);
    }
    buf_uleb(&cs, opt.len);
    buf_bytes(&cs, opt.d, opt.len);
    free(body.d);
    free(opt.d);
    free(funcs[i].body.d);
  }
  buf_byte(&wasm, 10);
  buf_uleb(&wasm, cs.len);
  buf_bytes(&wasm, cs.d, cs.len);
  free(cs.d);

  *out_wasm = wasm.d;
  *out_len = wasm.len;
  free(src);
  return 0;
}
#endif
