#include "stack.h"	/*struct definition*/
#include <stdlib.h>	/*free, malloc*/
#include <string.h>	/*memcpy*/
#include <assert.h>	/*asserts*/

struct stack
{
	size_t size_of_element;
	void* end;
	void* top;
	char base[1];

};

stack_t* StackCreate(size_t size_of_element, size_t num_of_elements)
{
	stack_t *current_stack = NULL;
	
	assert(size_of_element && num_of_elements);

	current_stack = malloc(num_of_elements * size_of_element +
														 sizeof(stack_t) - 1);
	/*allocating enough memory to accomodate struct with stack*/

	if (!current_stack)
	{
		return NULL;
	}

	current_stack->size_of_element = size_of_element;
	current_stack->end = current_stack->base + 
						(num_of_elements * size_of_element);
	current_stack->top = current_stack->base;
	
	return current_stack;
}

int StackPush(stack_t* stack, const void* element)
{
	assert(stack);

	if (stack->top == stack->end) /*checking if we've reached the top*/
	{
		return 1;
	}
	
	memcpy(stack->top, element, stack->size_of_element);

	stack->top = (char *)stack->top + stack->size_of_element;
	/*adding the given element and moving pointer to next position*/

	return 0;
}

void StackPop(stack_t* stack)
{
	assert(stack);

	stack->top = (char *)stack->top - stack->size_of_element;	
	/*if not at base, move pointer back to last element*/
}

void *StackPeek(stack_t* stack)
{
	void* temp = NULL;

	assert(stack);

	if (!StackSize(stack))
	{
		return NULL;
	}

	temp = (char*)stack->top - stack->size_of_element;/*show the last element*/

	return temp;
}

size_t StackSize(const stack_t *stack)
{
	assert(stack);

	return (((char*)stack->top - (char*)stack->base) / stack->size_of_element);
}	/*number of bytes in stack divided by element size = number of elements*/

void StackDestroy(stack_t* stack)
{
	assert(stack);
	
	free(stack);
	stack = NULL;
}

static void SortedStackInsert(stack_t *stack, int data)
{
	int number = 0;

	if (StackSize(stack) == 0 || *(int *)StackPeek(stack) >= data)
	{
		StackPush(stack, &data);
		return;
	}

	number = *(int *)StackPeek(stack);
	StackPop(stack);
	SortedStackInsert(stack, data);
	StackPush(stack, &number);
}

void StackSortRecursive(stack_t *stack)
{
	int number = 0;

	assert(stack);

	if (StackSize(stack) > 0)
	{
		number = *(int *)StackPeek(stack);
		StackPop(stack);
		StackSortRecursive(stack);
		SortedStackInsert(stack, number);
	}
	
	return;
}