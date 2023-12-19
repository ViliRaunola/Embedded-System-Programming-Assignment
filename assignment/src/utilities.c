/*

 * utilities.c
 *
 *  Created on: 12 Dec 2023
 *      Author: vraun
 */


#include "utilities.h"


// Send one character through UART interface
void uart_send(char c) {
	while (UART_STATUS & XUARTPS_SR_TNFUL);
	UART_FIFO = c;
	while (UART_STATUS & XUARTPS_SR_TACTIVE);
}

// Send string (character array) through UART interface
void uartSendString(char str[BUFFER_SIZE]) {
	char *ptr = str;
	// While the string still continues.
	while (*ptr != '\0') {
		uart_send(*ptr);
		ptr++;
	}
}

// Check if UART receive FIFO is not empty and return the new data
char uart_receive()
{
 if ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY) return 0;
 return UART_FIFO;
}

void handleTaskExit()
{
	AXI_LED_DATA = 0b0000;
	printMenu();
}


/* Source: https://support.xilinx.com/s/question/0D52E00006hpTYOSA2/how-to-print-float-value-on-hyperterminal-using-xilprintf?language=en_US
 * Function to print a float to the UART. Converts the passed float to int and prints it.
 * */
void floatToIntPrint(float fval) {
	int whole, thousandths;
	whole = fval;
	thousandths = abs(round((fval - whole) * 1000));
	if(thousandths == 1000) {
		if(fval < 0) {
			whole--;
		} else {
			whole++;
		}
		thousandths = 0;
	}
	if(whole == 0 && fval < 0 && thousandths != 0) {
		xil_printf("-%d.%03d", whole, thousandths);
	} else {
		xil_printf("%d.%03d", whole, thousandths);
	}
}

/* Can be used to receive a incoming message from UART. Returns a pointer to the created string. */
char* uartReceiveString(){
	static int index = 0;
	static char rx_buf[BUFFER_SIZE];
	char input = uart_receive();

	// If an UART message was received.
	if (input != 0){
		// Depending on the serial terminal used, UART messages can be terminated
		// by either carriage return '\r' or line feed '\n'.
		if (input == '\r' || input == '\n' || index >= BUFFER_SIZE){
			if(index >= BUFFER_SIZE)
			{
				xil_printf("Maximum buffer size reached. UART buffer cleared.\n");
				char tempInput = input;
				while((tempInput != '\r') || (tempInput == '\n' ))
				{
					tempInput = uart_receive();
				}
			}
			rx_buf[index] = '\0';
			index = 0;
			return rx_buf;
		}
		else {
			rx_buf[index] = input;
			index++;
		}
	}
	return 0;
}

/* Checks that the given string is a valid number. Returns true if the number is float or an integer. Works for negative and positive values. */
int isNumber(char* input) {
    int points = 0;
    if(strlen(input) == 1 && (input[0] == '-')) {
        return 0;
    } else if (input[0] == '-') {
        if(strlen(input) > 2 && input[1] == '0'  && input[2] != '.') {
            return 0;
        }
        for(int i=1;i<strlen(input);i++) {
            if(strlen(input) > 1 && input[i] == '.') {
                points++;
                if (points > 1) {
                    return 0;
                } else {
                    continue;
                }
            }
            if(!isdigit(input[i])) {
                return 0;
            }
        }
    } else {
        if(strlen(input) > 1 && input[0] == '0' && input[1] != '.') {
            return 0;
        }
        for(int i=0;i<strlen(input);i++) {
            if(strlen(input) > 1 && input[i] == '.') {
                points++;
                if (points > 1) {
                    return 0;
                } else {
                    continue;
                }
            }
            if(!isdigit(input[i])) {
                return 0;
            }
        }
    }
    return 1;
}

void printMenu()
{
    xil_printf("\n################################## MAIN MENU ##################################\n");
	xil_printf("Select a mode by pressing the 1. button on the board or by typing a number 1-2. Currently idling...\n");
	xil_printf("###############################################################################\n\n");
	xil_printf("Select Mode:\n1. Configuration Mode\n2. Modulating Mode\n\n");

}
