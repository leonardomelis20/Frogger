#ifndef BUFFER_H
#define BUFFER_H

#include "strutture.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void init_buffer (Circular_buffer* buffer); 
void destroy_buffer(Circular_buffer* buffer); 
int produce_msg(Circular_buffer* buffer, Messaggio msg);
int consume_msg(Circular_buffer* buffer, Messaggio* msg); 

#endif