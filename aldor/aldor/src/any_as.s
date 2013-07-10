#if defined(sparc) & defined(unix) & defined(sun) & !defined(__GNUC__)
.text
        .align 4
        .global _get_fsr
        .proc   04
_get_fsr:
        !#PROLOGUE# 0
        save %sp,-112,%sp
        !#PROLOGUE# 1
        st %fsr, [%fp-12]
        ld [%fp-12],%i0
        b L4
        nop
L4:
        ret
        restore
        .align 4
        .global _set_fsr
        .proc   020
_set_fsr:
        !#PROLOGUE# 0
        save %sp,-104,%sp
        !#PROLOGUE# 1
        st %i0,[%fp+68]
        ld [%fp+68], %fsr
L5:
        ret
        restore
#endif

