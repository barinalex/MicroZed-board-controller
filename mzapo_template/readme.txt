	Author: Aleksandr Barinov
	Project: Vestavny system s architekturou ARM
Instalace:
	Stahnout zdrojovy kod z githabu a spustit pomoci prikazu make
	TARGET_IP=192.168.202.[prislusna deska].
Popis souboru se zdrojovym kodem:
	- Soubory “mzapo_..” jsou poskytnute knihovny pro pristup k
		hardwaru MicroZed.
	- Spousteci bod programu je main.c. Vytvari se a spousti se vlakna.
	- Soubory “.._menu_.c” odpovidaji za vytvareni prislusneho menu a
		jeho funkci.
	- utils.c obsahuje pomocne funkce jako prevadeni zrgb do hsv a
		zpatky, zmena dat ciselneho typu a taky funkce spolecne vsem
		menu, treba vyber barvy nebo zmena casu.
	- lcd_frame.c rada funkci odpovidajicich za vykresleni na obrazovku
		lcd.
	- led_control.c obsahuje vlakno odpovidajici za vykreslovani na led.
		Obsahuje prislusne funkce pro vypocet continuous, color flash a
		flash effectu.
	- connection.c obsahuje vlakno odpovidajici za komunikace pres sit
		ethernet.
	- menu.c - obsahuje vlakno odpovidajici za zmenu stavu menu a
		volbu uzivatele.
	- reg_manager.c - defenice adres hardwaru.
