#include "MorseCode.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h> 
#include <stdlib.h>  // srand() and random() functions
#include <string.h>


int main(void){

    HAL_Init();

    //Enabling ports
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    // Initializing pins for lights and buttons
    InitializePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  
    InitializePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP,0); 

    //Wait for the player to press the button on the nucleo board to start the game
    while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){ 
    }

    int round = 0; // Counts the round number

    //The array, words, holds random colors that could be in the message
    //The array, message, creates a color sequence, with a length of 4, using the available colors in the array, words
    //The array, message_in_numbers, holds the created color sequence numerically (1 = pink, 2 = blue, 3 = green)
    char *words[3] = {"pink" , "blue" , "green"};
    char *message[4];
    int message_in_numbers[4];


    srand(HAL_GetTick());

    //create a random color sequence using the 3 colors in the array, words
    for(int i = 0; i < 4; i ++){
        message[i] = words[rand() % 3];  // use random number between 0 and 2
    }

    // Convert the message into numbers to make the player input easier to compare. 
    // Allows for any colors to be used in the array, words.
    for(int i = 0; i < 4; i ++){
        if(message[i] == words[0]){
            message_in_numbers[i] = 1;
        }else if(message[i] == words[1]){
            message_in_numbers[i] = 2;
        }else if(message[i] == words[2]){
            message_in_numbers[i] = 3;
        }
    }
    
    //In each round, a colour is communicated to the reader using morse code and then the game waits for the user button input
    for(round; round < 4; round++){

        HAL_Delay(1000); // Delay after clicking button

        //Communicate the words to the reader in morse code (using dots + dashes) with the LED
        for(int inner = 0; inner < strlen(message[round]); inner++){
            conversion_to_morse(message[round][inner]);
            HAL_Delay(2500); // Delay between letters
        }

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);  // Blue light dot to indicate the end of the color
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);

        // Check if the player inputs the correct color using the buttons
        if(check_input(message_in_numbers, round)){
        }else{
            break;
        }
    }

    // If the player reaches the final round, they must entire the entire color sequence all at one using the buttons
    if(round == 4){
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);  //blue light flashes 3 times to indicate the special final round
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);

        if(check_input(message_in_numbers, round)){
            win();
        }else{
            lose();
        }
    }else{
        lose();
    }
}

// Function is called once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick();
}

// Compares the user button input with the color sequence according to the round number
bool check_input(int const message_sequence[], int round){
    bool correct = false;
    int input_sequence[4]; 
    int input_colour_sequence[4];

    //In the first four rounds, the user only enters one color. In the final round, the player must entire the entire sequence correctly.
    if(round < 4){

        //Wait for the button to be pressed
        while((!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) || (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) || (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))){
            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){ // pink button - for color at words[0]
                input_sequence[round] = 1;
                break; 
            }else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)){ // blue button - for color at words[1]
                input_sequence[round] = 2;
                break;
            }else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)){ // green button - for color at words[2]
                input_sequence[round] = 3;
                break;
            }
        }

        //Wait for the button to be released
        while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) || (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) || (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)));
        HAL_Delay(500);

        //If the entire input sequence matches the entire color sequence, return true.
        if(input_sequence[round] == message_sequence[round]){
            correct = true;
        }else{
            return false;
        }
    }else if(round == 4){

        for(int i = 0; i < 4; ++i){
            //Wait for the button to be pressed
            while((!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) || (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) || (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))){
                if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){ // pink button - for words[0]
                    input_colour_sequence[i] = 1;
                    break; 
                }else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)){ // blue button - for words[1]
                    input_colour_sequence[i] = 2;
                    break;
                }else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)){ // green button - for words[2]
                    input_colour_sequence[i] = 3;
                    break;
                }
            }
            // Wait for the button to be released
            while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) || (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) || (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)));
            HAL_Delay(500);
        }

        //If the entire input sequence matches the entire color sequence, return true.
        for(int i = 0; i < 4; i++){
            if(input_colour_sequence[i] == message_sequence[i]){
                correct = true;
            }else{
                return false;
            }
        }
    }
            
    return correct;
}

// Takes in each character in the color and communicates the letter in morse code
void conversion_to_morse(char letter){
    if(letter == 'a'){
        dot();  
        space_between_dots_dashes();
        dash();
    }else if(letter == 'b'){
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'c'){
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'd'){
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'e'){
        dot();
    }else if(letter == 'f'){
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'g'){
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'h'){
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'i'){
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'j'){
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'k'){
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'l'){
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'm'){
        dash();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'n'){
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'o'){
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'p'){
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 'q'){
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'r'){
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
    }else if(letter == 's'){
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }else if(letter == 't'){
        dash();
    }else if(letter == 'u'){
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'v'){
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'w'){
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'x'){
        dash();
       space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
       space_between_dots_dashes();
        dash();
    }else if(letter == 'y'){
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dash();
    }else if(letter == 'z'){
        dash();
        space_between_dots_dashes();
        dash();
        space_between_dots_dashes();
        dot();
        space_between_dots_dashes();
        dot();
    }
}

void space_between_dots_dashes(void){
    HAL_Delay(600);
}

void dot(void){
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  
    HAL_Delay(400);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void dash(void){
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  
    HAL_Delay(2000);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void lose(void){
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  //Red light flashes three times
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100); 
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void win(void){
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);  //Green light flashes three times
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100); 
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);  
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
}

//Intializes pins. Learned how to intialize pins through a course.
void InitializePin(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pins;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullups;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}