#include "Libraries/dynarray.h"

enum comp_types {AND_t, NOT_t, XOR_t, BUTTON_t, OUTPUT_t, OR_t};

// Could probably also be used for module visualisation --> Delegate to different list

typedef struct component 
{
	uint8_t *states; 
	enum comp_types type;
	DYN_ARR inputs, outputs; // Component pointers 
} COMPONENT;

typedef struct component_pointer
{
	int module_index;
	int component_index ; 
} C_POINTER; 

typedef struct visual_component
{
	int sx; int sy;
	int ex; int ey;
	char *name;
	C_POINTER component;
} VISUAL_COMPONENT;

typedef struct module
{	
 	DYN_ARR constituents;
} MODULE;

// Events dictate what will be simulated next
typedef struct event
{
	int trigger_time;
	DYN_ARR *affected;
} EVENT;
