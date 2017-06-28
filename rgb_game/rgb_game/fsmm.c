/*
 * GccApplication1.c
 *
 * Created: 27/06/2017 22:44:07
 * Author : JAHBENDS
 */ 


#include "sam.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NR_STATES 3
#define NR_EVENTS 2

/* off topic
#define BUFFER_LENGTH 16

typedef struct
{
	uint8_t buffer_uart[BUFFER_LENGTH];
	uint8_t length;
	uint8_t index;
}struct_uart

struct_uart received_data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }


void example_funct_receive_uart()
{
	received_data.buffer_uart[received_data.index] = ler_dado_uart();
	length += 1;
	index = (index+1) % BUFFER_LENGTH;
}

 end off topic*/

typedef struct
{
	void (*ptrFunc) (void); uint8_t NextState;
}FSM_STATE_TABLE;

typedef enum
{
	INIT=0, PLAY, END
} State_t;


typedef enum
{
	EV_1=0, EV_2
} Events;

State_t current_state = INIT;
Events act_event = EV_1;

void init_f(void);
void play_f(void);
void end_f(void);



const FSM_STATE_TABLE StateTable[NR_STATES][NR_EVENTS] =
{
/*INIT */	init_f, INIT,	init_f, PLAY,		
/*PLAY */	play_f, PLAY,	play_f, END,		
/*END */	end_f, INIT,	end_f, PLAY		
};


/*
	Só inicia recebendo 'i' senão fica no mesmo estado
	EV_1 = evento nulo
	EV_2 = receber instrução para iniciar jogo
*/
void init_f(void)
{
	printf("Funcao init\n");
	char a;
	scanf("%c", &a);
fflush(stdin);
    __fpurge(stdin);

	if (a == 'i')
	{
		act_event = EV_2;
	}
	else
		act_event = EV_1;

	current_state = StateTable[current_state][act_event].NextState;
}

/*
	Só sai recebendo 'f' senão fica no mesmo estado
	EV_1 = continuar jogando
	EV_2 = receber instrução para parar jogo
*/

void play_f(void)
{
	printf("Funcao play\n");
	char a;
	scanf("%c", &a);
fflush(stdin);
    __fpurge(stdin);

	if (a == 'f')
		act_event = EV_2;
	else
		act_event = EV_1;

	current_state = StateTable[current_state][act_event].NextState;
}


/*
	Só sai recebendo 'p' para parar e 'j' para jogar
*/

void end_f(void)
{
	printf("Funcao end\n");
	char a;
	while ( (a != 'j') && (a != 'p') )
	{
		printf("Loop Funcao end\n");
		scanf("%c", &a);
fflush(stdin);
    __fpurge(stdin);
	}

	if (a == 'j')
		act_event = EV_2;
	else if (a == 'p')
		act_event = EV_1;

	current_state = StateTable[current_state][act_event].NextState;
}

int main(void)
{
	while(1)
	{
		StateTable[current_state][act_event].ptrFunc();
	}
}
