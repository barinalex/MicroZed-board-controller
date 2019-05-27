#ifndef _STATIC_MENU_H_
#define _STATIC_MENU_H_


void create_static_menu(menu_ *menu);
void create_static_led1_menu(menu_ *menu);
void create_static_led2_menu(menu_ *menu);
void create_static_both_menu(menu_ *menu);

void change_static_led1(menu_ *menu);
void change_static_led2(menu_ *menu);
void change_static_both(menu_ *menu);
void copy_led1_to_led2(menu_ *menu);
void copy_led2_to_led1(menu_ *menu);
void choose_color(int menu_pos);

#endif
