#ifndef _DOPE_CLIPBOARD_H
#define _DOPE_CLIPBOARD_H

struct clipboard_services {
	void     (*set)    (char *dataIn, s32 length);
	void     (*get)    (char **dataOut, s32 *length);
};

#endif /* _DOPE_CLIPBOARD_H */ 

