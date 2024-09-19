/*
 * Code manette
 *
 * Created: 2023-02-07 14:23:13
 * Author : Equipe 27
 */ 

//Librairies
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include "utils.h"
#include "driver.h"
#include "uart.h"


int main(void)
{
	//Initialisation 
	adc_init();
	lcd_init();
	uart_init(UART_0);
	sei();
	
	//Variables pour les informations
	uint8_t BoutonTirer;
	uint8_t BoutonMonterDescendre;
	uint8_t BoutonMode;
	uint8_t BoutonInertie;
	uint8_t valpot;
	uint8_t x;
	uint8_t y;

	//Variables pour boutons pour etat du bouton et un tempo
	int nouveauBouton= 1;
	int etatBouton = 0;

	//Mode de la voiture
	int etatMode = 0;

	//Tableau pour les informations à afficher
	char mode[33];
	char info[33];
	char tab[33];
	
	//Setup des boutons
	DDRA=clear_bit(DDRA,PA2);
	PORTA=set_bit(PORTA,PA2);
	DDRD=clear_bit(DDRD,PD5);
	PORTD=set_bit(PORTD,PD5);
	DDRD=clear_bit(DDRD,PD6);
	PORTD=set_bit(PORTD,PD6);
	DDRD=clear_bit(DDRD,PD7);
	PORTD=set_bit(PORTD,PD7);
	
    while (1) 
    {
		//Setup Lcd
		lcd_clear_display();
		lcd_set_cursor_position(0,0);

		//Lecture des composants
		y = adc_read(PA1);
		x = adc_read(PA0);
		valpot = adc_read(PA3);
		BoutonTirer = read_bit(PIND,PD7);
		BoutonMonterDescendre = read_bit(PIND,PD5);
		BoutonInertie = read_bit(PIND,PD6);
		BoutonMode = read_bit(PINA, PA2);
		
		//Mode Tire
		if (BoutonMode == 0 && nouveauBouton == 1 && etatBouton == 0)
		{
			//Mode Tire
			etatMode = 1;
			nouveauBouton = 0;
			etatBouton ++;
		}

		//Mode Course
		else if (BoutonMode == 0 && nouveauBouton == 0 && etatBouton == 0)
		{
			//Mode Course
			etatMode = 0;
			nouveauBouton = 1;
			etatBouton++;
		}

		//Remise à zéro de la tempo
		if (BoutonMode == 1)
		{
			etatBouton = 0;
		}
		
		if (etatMode == 0)
		{
			//Affichage Mode Course
			sprintf(info, " ");
			sprintf(info, "X=%d, Y=%d, V=%d, Mode C", x, y, valpot);
			sprintf(mode, " ");
			sprintf(mode, "C");
			lcd_write_string(info);
		}

		else
		{
			//Affichage Mode Tire
			sprintf(info, " ");
			sprintf(info, "X=%d, Y=%d, V=%d, Mode T,%d,%d,%d", x, y, valpot, BoutonMonterDescendre,BoutonInertie, BoutonTirer);
			sprintf(mode, " ");
			sprintf(mode, "T");
			lcd_write_string(info);
		}
		
		//Envoie des informations
		sprintf(tab, "%d, %d, %d, %d, %d, %d, %d, %s \r\n", x, y, valpot, BoutonInertie, BoutonMonterDescendre, BoutonTirer, BoutonMode, mode);
		uart_put_string(UART_0, tab);
		_delay_ms(50);		
    }
}