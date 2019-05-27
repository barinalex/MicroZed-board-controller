Author: Aleksandr Barinov
Project: Embedded system with ARM architecture


Installation:
        Download source code from github and run it with a command: make TARGET_IP=192.168.202.[your unit].


Project description:
        The whole program is running on the three threads:
First thread is processing the user input from the knobs, changing the menu state and call the functions of the program like choosing color, time or network connection.
The second thread is processing the output to the leds. It computes the time and color changing and send current colour to the hardware address. You can find all of the function for this thread in the led_control.c (.h) files.
 The third thread is processing communication via ethernet. All of the functions for this thread are in the communication.c (.h) files.


Solution for the menu:
        For the menu nonlinear linked list is used. Its structure defined in the menu.h file. The menu structure contains pointers to the menu functions. In the menu.c file in the function menu(..) are processing changes in the menu such as switching between menus (assignment of the pointer to the next or to the previous menu from the linked list) and some menu functionality calls. With such design you can easily add new menu to the program or a new functionality to the menu. All of the exists menus are defined in the .._menu.c (.h) files.


Network protokol:
For communication UDP protocol is used. One socket is defined for the broadcast and one for the connection between units. Communication starts with the broadcast message to find other available units. After other units are fined there and one of them is chosen init message for connection is sended and then data from knobs are sended until user press disconnect.


Source code files:
* Files “mzapo_..” are provided libraries for the access to the MicroZed hardware.
* The run file is the main.c. Threads are created and runned in the main function.
* Files “.._menu_.c” contains implementation of the menus.
* utils.c contains functions such as rgb to hsv converting, colour and time changing etc.
* lcd_frame.c contains functions for the drawing information to the led.
* led_control.c contain functions for the continuous, flashing, colour flashing computations. 
* connection.c network communication.
* menu.c implementation of the main menu and main cykle for the processing menu changes.
* reg_manager.c - definition of the hardware addresses.
