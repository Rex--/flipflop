; This file defines a stub for the interrupt vector that jumps to an
; application int vector based on a predefined offset.

#include "boot_config.h"

psect intentry,delta=2
    ljmp PROG_INT_VECT


;; EOF ;;
