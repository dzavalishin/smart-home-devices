
//#include "defs.h"

#include "errno.h"
#include "dev_map.h"

struct pod_property;

typedef enum
{
    pt_int16,
    pt_mstring,         // malloced string
    pt_enum32,         	// enum int32 - unimpl!
} dev_property_type_t;

typedef struct dev_properties
{
    struct dev_property *list;
    uint16_t             lsize;

    //! Get pointer to property value
    void *              (*valp)(struct dev_properties *ps, void *context, uint16_t offset );
} dev_properties;


typedef struct dev_property
{
    dev_property_type_t type;
    const char		*name;
    void                *valp;

    uint16_t            offset;
    char                **val_list; // for enums

    errno_t             (*activate)(struct dev_properties *ps, void *context, uint16_t offset, void *vp );
    errno_t             (*setf)(struct dev_properties *ps, void *context, uint16_t offset, void *vp, const char *val);
    errno_t             (*getf)(struct dev_properties *ps, void *context, uint16_t offset, void *vp, char *val, uint16_t len);
} dev_property;


#define PROP_COUNT(__plist) (sizeof(__plist)/sizeof(__plist[0]))

errno_t dev_drv_listproperties( struct dev_major *drv, int nProperty, char *pValue, int vlen );
errno_t	dev_drv_getproperty   ( struct dev_major *drv, const char *pName, char *pValue, int vlen );
errno_t	dev_drv_setproperty   ( struct dev_major *drv, const char *pName, const char *pValue );


