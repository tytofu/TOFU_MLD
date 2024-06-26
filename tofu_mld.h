#ifndef _TOFU_MLD_
#include <assert.h>

#define MAX_STRUCTURE_NAME_SZIE 128
#define MAX_FIELD_NAME_SIZE 128
/*Define OFFSETOF macro function to calculate the offset of a structure member*/
#define OFFSETOF(struct_name, fld_name)                                        \
  (unsigned int)&(((struct_name *)0)->fld_name)
/*Define FIELD_SIZE macro function to calculate the size of a structure member*/
#define FIELD_SIZE(struct_name, fld_name) sizeof(((struct_name *)0)->fld_name)

/*Structure Data base Definition Start */

/*Enumeration for data types*/
typedef enum {
  UINT8,
  UINT32,
  INT32,
  CHAR,
  OBJ_PTR,
  FLOAT,
  DOUBLE,
  OBJ_STRUCT
} data_type_t;
/*Enumeration for bool types*/
typedef enum {
  MLD_FALSE,
  MLD_TRUE
}mld_boolean_t;
// Descibe Data
typedef struct _struct_db_rec_t struct_db_rec_t;

/*Structure to store the information of one field of a C structure*/
typedef struct _field_info_ {
  char fname[MAX_FIELD_NAME_SIZE]; /*Name of the field*/
  data_type_t dtype;               /*Data type of the field*/
  unsigned int size;               /*Size of the field*/
  unsigned int offset;             /*Offset of the field*/
  // Below field is meaningful only if dtype = OBJ_PTR or OBJ_STRUCT
  char nested_str_name[MAX_STRUCTURE_NAME_SZIE];
} field_info_t;

/*Structure to store the inforamtion of one c structure
 *which could have 'n_fields' fields
 * */
struct _struct_db_rec_t {
  struct_db_rec_t *next; /*Pointer to the next structure in the linkedlist*/
  char struct_name[MAX_STRUCTURE_NAME_SZIE]; /*Key*/
  unsigned int ds_size;                      /*Size of the structure*/
  unsigned int n_fields;                     /*No. of fields in the structure*/
  field_info_t *fields; /*pointer to the first element of array of fields*/
};

/*Finally the head of the linked list representing the structure database*/
typedef struct _struct_db_ {
  struct_db_rec_t *head; /*head of the linked list*/
  unsigned int count;    /*the number of record*/
} struct_db_t;

/*Structure Data base Definition Ends*/

/* Printint functions */
/*Prints information about a record in the database*/
void print_structure_rec(struct_db_rec_t *struct_rec);
/*Prints information for all record in the database*/
void print_structure_db(struct_db_t *struct_db);
/*Fn to add the structure record in a structure database*/
int add_structure_to_struct_db(struct_db_t *struct_db,
                               struct_db_rec_t *struct_rec);
/*work:Find the struct record for the second parameter*/
struct_db_rec_t * struct_db_look_up(struct_db_t *struct_db,char *struct_name);
/*Structure Registration helping APIs*/

#define FIELD_INFO(struct_name, fld_name, dtype, nested_struct_name)           \
  {                                                                            \
    #fld_name, dtype, FIELD_SIZE(struct_name, fld_name),                       \
        OFFSETOF(struct_name, fld_name), #nested_struct_name                   \
  }

#define REG_STRUCT(struct_db, st_name, fields_arr)                             \
  do {                                                                         \
    struct_db_rec_t *rec = calloc(1, sizeof(struct_db_rec_t));                 \
    strncpy(rec->struct_name, #st_name, MAX_STRUCTURE_NAME_SZIE);              \
    rec->ds_size = sizeof(st_name);                                            \
    rec->n_fields = sizeof(fields_arr) / sizeof(field_info_t);                 \
    rec->fields = fields_arr;                                                  \
    if (add_structure_to_struct_db(struct_db, rec)) {                          \
      assert(0);                                                               \
    }                                                                          \
  } while (0);

/*Object Database structure definition Starts here*/
typedef struct _object_db_rec_ object_db_rec_t;
struct _object_db_rec_{
  object_db_rec_t *next;
  void *ptr;/*key store address object*/
  unsigned int units;
  struct_db_rec_t *struct_rec;
  mld_boolean_t is_visited;/*Used for Graph traversal -- 用于图遍历的条件*/
  mld_boolean_t is_root;/*Is this object root object?*/
};
typedef struct _object_db_ {
  struct_db_t *struct_db;
  object_db_rec_t *head;
  unsigned int count;
}object_db_t;

/*Dumping functions*/
void print_object_rec(object_db_rec_t *object_rec,int i);
void print_object_db(object_db_t *object_db);
object_db_rec_t *object_db_look_up(object_db_t *object_db,void *ptr);
/*work:打印函数参数所对应的对象的每个字段的值*/
void mld_dump_object_rec_detail(object_db_rec_t *object_db_rec,const char *struct_ptr_name);
/*API to malloc and free the object*/
void * xcalloc(object_db_t *object_db,char *struct_name,int units);
void xfree(object_db_t *object_db,void *ptr);
/*APIs to register root objects*/
/*Not use xcalloc*/
void mld_register_global_as_root(object_db_t *object_db,void *objptr,char *struct_name,unsigned int units);
/*Use xcalloc*/
void mld_set_dynamic_object_as_root(object_db_t *object_db,void *obj_ptr);
#endif // !_TOFU_MLD_
