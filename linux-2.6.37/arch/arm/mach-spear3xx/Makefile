#
# Makefile for SPEAr3XX machine series
#

# common files
obj-y	+= spear3xx.o clock.o gpt.o

# spear300 specific files
obj-$(CONFIG_CPU_SPEAR300) += spear300.o

# spear300 boards files
obj-$(CONFIG_MACH_SPEAR300_EVB) += spear300_evb.o


# spear310 specific files
obj-$(CONFIG_CPU_SPEAR310) += spear310.o

# spear310 boards files
obj-$(CONFIG_MACH_SPEAR310_EVB) += spear310_evb.o


# spear320 specific files
obj-$(CONFIG_CPU_SPEAR320) += spear320.o

# spear320 boards files
obj-$(CONFIG_MACH_SPEAR320_EVB) += spear320_evb.o
obj-$(CONFIG_MACH_SPEAR320_HMI) += spear320_hmi.o
obj-$(CONFIG_MACH_SPEAR320_FTM) += spear320_ftm.o

# specific files
obj-$(CONFIG_CPU_SPEAR310) += emi.o
obj-$(CONFIG_CPU_SPEAR320) += emi.o
