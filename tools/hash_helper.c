#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint64_t fnv1a(const char *str) {
    uint64_t hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            printf("%s: %llu (0x%016llx)\n", argv[i], (unsigned long long)fnv1a(argv[i]), (unsigned long long)fnv1a(argv[i]));
        }
        return 0;
    }

    const char *cmds[] = {
        "skip_block_comment", "scan_line", "hash_line", "dispatch", "emit_ir", "main",
        "∀ₛₚₑ𝒸", "Ψ", "∈", "𝔘", "⊢_Γ", "{", "}",
        "(∂_Ω ⊢_{Γ;Δ} σ₃)^{seq}",
        "(⊣_{Δ;Γ} ∂_Ω)^{seq} :=",
        "(η_0 ∘_{2-Cat} ∂_Ω)^{Kan}",
        "⊥_{𝒯}→^{ex falso}⊤_{𝒯}"
    };
    for (int i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++) {
        printf("%s: %llu (0x%016llx)\n", cmds[i], (unsigned long long)fnv1a(cmds[i]), (unsigned long long)fnv1a(cmds[i]));
    }
    return 0;
}
