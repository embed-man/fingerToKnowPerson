#include "sqlite3.h"

#define FCMD_ADD_FINGER1 0x01
#define FCMD_ADD_FINGER2 0x02
#define FCMD_ADD_FINGER3 0x03
#define DEL_SPEC_USER    0x04
#define DEL_ALL_USERS    0x05
#define GET_FINGER_PIC   0x24
#define GET_FINGER_SPEC  0x23

typedef struct student{
	unsigned char id[4];
	unsigned char name[32];
	unsigned char spec[193];
}STU;

extern int add_finger(unsigned short  id, int finger_fd);

extern int get_finger_pic(int finger_fd);

extern STU get_id_from_finger(int finger_fd, sqlite3 *db);