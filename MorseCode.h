#include "stm32f4xx_hal.h"
#include <stdbool.h> 

void InitializePin(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate);

void conversion_to_morse(char letter);
bool check_input(int const message_sequence[], int round);
void dash(void);
void dot(void);
void win(void);
void lose(void);
void space_between_dots_dashes(void);


