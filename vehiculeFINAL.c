/*
 * Code Vehicule
 *
 * Created: 2023-02-06 15:38:07
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
	//Initialisation des composantes importantes
	lcd_init();
	uart_init(UART_0);
	pwm0_init();
	adc_init();
	sei();
	pwm0_init();
	pwm1_init(20000);
	pwm2_init();
	
	//Variables
	uint8_t  x;
	uint8_t  y;
	uint8_t  valPot;
	uint8_t  BoutonInertie;
	uint8_t  BoutonMonterDescendre;
	uint8_t  BoutonInertie;
	uint8_t  BoutonMode;

	//Tableau pour afficher
	char mode[40];
	char str[40];

	//État pour les boutons 
	int tempo = 0;
	int etatBouton = 0;
	int etatBouton1 = 0;
	
	//Constante de reduction pour les diagonales
	int REDUCTION = 0.5;

	//Setup des LEDs
	DDRB = set_bits(DDRB, 0b11111111);
	
	// Dans une boucle infinie
	while(1){
	
	//Si il y a des lignes dans le buffer			
	if (uart_rx_buffer_nb_line(UART_0) != 0){
		
		//Setup du LCD
		lcd_clear_display();
		lcd_set_cursor_position(0,0);

		//Recuperation des informations
		uart_get_line(UART_0, str, 100);
		sscanf(str, "%d, %d, %d, %d, %d, %d, %d, %s",  &x, &y, &valPot, &BoutonInertie, &BoutonMonterDescendre, &BoutonTirer, &BoutonMode, mode);
		sprintf(str, "X=%d, Y=%d, V=%d, %s", x, y, valPot, mode);

		//Affichage des donnees
		lcd_write_string(str);
		
		//PB4 + PB2 == roues de gauche
		//PB3 + PB1 == roues de droite
		
		//Mode Course sur vehicule
		if (Mode[0] == 'C')
		{

			//avancer
			if (y == 255 && (x > 115 && x < 135))
			{
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
		
			//reculer
			else if (y == 0 && (x >= 0 && x <= 255))
			{
				//Direction
				PORTB = clear_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
		
			//tourner vers droite
			else if (x == 255 && (y > 115 && y < 135))
			{
				//Direction
				PORTB = clear_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
		
			//tourner vers gauche
			else if (x == 0 && (y > 115 && y < 135))
			{	
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}

			//diagonale haute-gauche
			else if ((x == 0  && y > 135) || (y==255 && x < 115))
			{
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot*REDUCTION);
			}
		
			//diagonale haute-droit
			else if ((x == 255 && y > 135) || (y == 255 && x > 130))
			{
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot*REDUCTION);
				pwm0_set_PB4(valPot);
			}
		
			//Immobile
			else
			{
				//Remise à zéro
				pwm0_set_PB4(0);
				pwm0_set_PB3(0);
				PORTB = clear_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
			}
			_delay_ms(50); // Petit délai pour ralentir un peu la boucle
		}
		
		//Mode Tire du vehicule
		if (Mode[0] == 'T')
		{
			//avancer
			if (y == 255 && (x > 0 && x < 255))
			{
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
			
			//par derriere
			else if (y == 0 && (x > 0 && x < 255))
			{
				//Direction
				PORTB = clear_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
			
			//tourner vers droite
			else if (x == 255 && (y > 120 && y < 130))
			{
				//Direction
				PORTB = clear_bit(PORTB, PB1);
				PORTB = set_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
			
			//tourner vers gauche
			else if (x == 0 && (y > 120 && y < 130))
			{
				//Direction
				PORTB = set_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
				//Puissance
				pwm0_set_PB3(valPot);
				pwm0_set_PB4(valPot);
			}
			
			//monter ou descendre le canon
			else if (BoutonMonterDescendre == 0)
			{
				//descend si le potentiometre est à 0
				if (valPot == 0)
				{
					//Direction
					PORTB = set_bit(PORTB, PB0);
					//Puissance
					pwm2_set_PD6(235);
				} 
				//monte si le potentimere est à 255
				else if (valPot == 255)
				{
					//Direction
					PORTB = clear_bit(PORTB, PB0);
					//Puissance
					pwm2_set_PD6(235);
				}
			}
			
			//tirer
			else if (BoutonTirer == 0 && etatBouton1 == 0 && tempo == 0)
			{
				pwm1_set_PD5(1950);
				etatBouton1 = 1;
				tempo++;
			}
			//recharge
			else if (BoutonTirer == 0 && etatBouton1 == 1 && tempo == 0)
			{
				pwm1_set_PD5(1150);
				etatBouton1 = 0;
				tempo++;
			}
			
			//allumer roue 
			else if (BoutonTirer == 0 && etatBouton == 0 && tempo == 0 )
			{
				pwm2_set_PD7(210);
				etatBouton = 1;
				tempo++;
			}
			
			//eteindre roue
			else if (BoutonTirer == 0 && etatBouton == 1 && tempo == 0)
			{
				pwm2_set_PD7(0);
				etatBouton = 0;
				tempo++;
			}
					
			//Immobile et Aucun bouton
			else
			{
				//Remise à zéro
				pwm0_set_PB4(0);
				pwm0_set_PB3(0);
				PORTB = clear_bit(PORTB, PB1);
				PORTB = clear_bit(PORTB, PB2);
				pwm2_set_PD6(0);
				tempo = 0;
			}
		_delay_ms(50); // Petit délai pour ralentir un peu la boucle
		}
	   }
	}
}




	