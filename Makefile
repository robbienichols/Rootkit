KMOD= hook_read # Name of KLD to build.
SRCS= hello_read.c # List of source files.
.include <bsd.kmod.mk>