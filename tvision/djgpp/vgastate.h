/* Copyright 1998-1999 (c) by Salvador Eduardo Tropea
   This code is part of the port of Turbo Vision to gcc, please see the rest
 of the file for copying information.
 */
#ifdef __cplusplus
extern "C" {
#endif
int VGAStateGetBufferSize(void);
int VGAStateSave(void *buffer);
int VGAStateLoad(void *buffer);
#ifdef __cplusplus
}
#endif

