// #include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "Libraries/dynarray.h"
#include "components.h"
// #include <SDL2/SDL.h>

DYN_ARR event_queue;
DYN_ARR all_modules;
int simulation_time;

#define GET_COMPONENT_POINTER(c_p) GET_ITEMP((GET_ITEMP(all_modules, c_p.module_index, MODULE))->constituents, c_p.component_index, COMPONENT)

// Order events in order of least trigger time to greatest
int comprevents (const void *a, const void *b)
{
	EVENT *x = (EVENT *)a;
	EVENT *y = (EVENT *)b;

	if      (x->trigger_time > y->trigger_time) return  1;
	else if (x->trigger_time < y->trigger_time) return -1;
	return 0;
}

// Finds new state of basic component
uint8_t evaluate_component (const COMPONENT *component) 
{
	if (component->type == BUTTON_t) 
	{
		return component->states[0] ? 0 : 1;
	}

	COMPONENT *component_pointer = GET_COMPONENT_POINTER(GET_ITEM(component->inputs, 0, C_POINTER));

	#define INPUTS_LOOP(ops) for (int i = 1; i < component->inputs.nelems; i++)                  \
							 {                                                                   \
								state ops GET_COMPONENT_POINTER(GET_ITEM(component->inputs, i, C_POINTER))->states[0]; \
							 }; return state;          

	uint8_t state = component_pointer->states[0];

	switch (component->type)
	{
		case AND_t: INPUTS_LOOP(&=)
		case XOR_t: INPUTS_LOOP(^=)
		case OR_t: case OUTPUT_t:
				INPUTS_LOOP(|=)  
		case NOT_t:  
			return state ? 0 : 1; 
	}
    #undef INPUTS_LOOP
}

void update_component (COMPONENT* component)
{
	uint8_t new_state = evaluate_component(component);
	if (new_state != component->states[0]) 
	{
		component->states[0] = new_state;
		int trigger_delay = 1;
		// Possible manhattan distance delay for wires
		/*if   (component->type == WIRE_t) trigger_delay = ((component->sx - component->ex) + 
														 (component->sy - component->ey) + 1);
		else                             trigger_delay = 1;*/
		EVENT new_event = {trigger_delay + simulation_time, &component->outputs};
		push_back(&event_queue, &new_event); 
	}
}

C_POINTER add_component (enum comp_types type) 
{
	MODULE *globals = GET_ITEMP(all_modules, 0, MODULE);
	int index = (globals->constituents).nelems;
	//printf("index: %d\n", index);
	COMPONENT new_comp = {calloc(1, sizeof(uint8_t)), type, init_array(sizeof(C_POINTER)), init_array(sizeof(C_POINTER))};
	push_back(&globals->constituents, &new_comp);

	if (type == BUTTON_t)
	{
		EVENT new_event = {-1, &(GET_COMPONENT_POINTER(((C_POINTER){0, index}))->outputs)};
		push_back(&event_queue, &new_event); 
	}
	return (C_POINTER){0, index};
}

void connect (C_POINTER pointer_comp_1, C_POINTER pointer_comp_2)
{
	COMPONENT *comp_1 = GET_COMPONENT_POINTER(pointer_comp_1);
	COMPONENT *comp_2 = GET_COMPONENT_POINTER(pointer_comp_2);
	push_back(&(comp_1->outputs), &pointer_comp_2);
	push_back(&(comp_2->inputs), &pointer_comp_1);
}

void push_button(C_POINTER button_pointer) 
{
	COMPONENT *button = GET_COMPONENT_POINTER(button_pointer);
	if (button->type != BUTTON_t) return;
	update_component(button);
}


void free_modules(void)
{
	for (int i = 0; i < all_modules.nelems; i++)
	{
		MODULE *module = GET_ITEMP(all_modules, i, MODULE);
		for (int j = 0; j < (module->constituents).nelems; j++)
		{
			COMPONENT *component = GET_ITEMP(module->constituents, j, COMPONENT);
			free(component->states);
			free_array(&(component->inputs)); free_array(&(component->outputs));
		}
		free_array(&(module->constituents));
	}
}


int main(int argc, char **argv)
{
	event_queue = init_array(sizeof(EVENT));
	all_modules = init_array(sizeof(MODULE));
	MODULE *global = malloc(sizeof(MODULE));                
	global->constituents = init_array(sizeof(COMPONENT));
	push_back(&all_modules, global); free(global);
	simulation_time = -1;
	
	// Test circuit implementing XOR
	C_POINTER button1 = add_component(BUTTON_t);
	C_POINTER button2 = add_component(BUTTON_t);
	C_POINTER and1 = add_component(AND_t);
	C_POINTER and2 = add_component(AND_t);
	C_POINTER not1 = add_component(NOT_t);
	C_POINTER not2 = add_component(NOT_t);
	C_POINTER or = add_component(OR_t);
	C_POINTER out = add_component(OUTPUT_t);
	
	connect(button1, not1);
	connect(button2, not2);
	
	connect(not2, and1);
	connect(button1, and1);
	
	connect(not1, and2);
	connect(button2, and2);
	
	connect(and1, or);
	connect(and2, or);
	
	connect(or, out);
	
	push_button(button1);
	
	while (event_queue.nelems != 0)
	{
		simulation_time++;
		sort_array(&event_queue, comprevents);
		// Pop event
		EVENT processing = GET_ITEM (event_queue, 0, EVENT);
 			
		if (processing.trigger_time > simulation_time) continue;

		remove_item(&event_queue, 0);

		for (int i = 0; i < processing.affected->nelems; i++)
		{	
			C_POINTER affected_pointer = GET_ITEM((*processing.affected), i, C_POINTER);
			update_component(GET_COMPONENT_POINTER(affected_pointer));
		} 
	} 
	
	printf("Final XOR output: %d\n", GET_COMPONENT_POINTER(out)->states[0]);

	free_array(&event_queue);
	free_modules();

	return 0;
}
