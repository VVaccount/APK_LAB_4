#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

int melody[] = {  
	659, 622, 659, 622, 659, 493, 622, 523, 440, 0
};

int clock[] = {
	200, 200, 200, 200, 200, 200, 200, 200, 200, 0
};

void getRandom(long max) {
	//Порт 42 – канал 2, связан с динамиком

	long kd, kdLow, kdHigh;
	//10 11 010 0 – канал 2, старший и младший байты пишутся по очереди, режим 2 (программируемый генератор импульсов), формат (двоичный)
	outp(0x43, 0xB4);

	kd = max;
	outp(0x42, kd % 256);
	kd /= 256;
	outp(0x42, kd);
	outp(0x61, inp(0x61) | 1);//включить канал 2, не включая динамик

	getch();

	outp(0x43, 10000000);//outp(0x43, KK 00 0000);

	kdLow = inp(0x42);
	kdHigh = inp(0x42);
	kd = kdHigh * 256 + kdLow;

	printf("%ld\n", kd);

	// Выключаем канал 2.

	outp(0x61, inp(0x61) & 0xfe);
}

void printKd() {
	long kdLow, kdHigh, kd, kdArray[10000];
	for (int iChan = 0; iChan < 3; iChan++) {
		//init, turn ON for kd2 (0х61)
		if (iChan == 2)
		{
			outp(0x61, inp(0x61) | 1);
		}
		for (int j = 0; j < 10000; j++) {
			//select a channel
			outp(0x43, iChan << 6);//outp(0x43, KK 00 0000);

			kdLow = inp(0x40 + iChan);
			kdHigh = inp(0x40 + iChan);
			kd = kdHigh*256 + kdLow;
			kdArray[j] = kd;
		}
		//Find MAX …
		for (j = 0; j < 10000; j++) {
			if (kdArray[j] > kd)
			{
				kd = kdArray[j];
			}
		}
		printf("%X\n", kd);
	}
	outp(0x61, inp(0x61) & 0xfe);
}

void printWords(){
	char temp;
	int state;
    //Для каждого канала
	for(int iChannel = 0; iChannel < 3; iChannel++) {
		//select a channel
		//outp(0x43, 11 10 К2 К1 К0 0);
		outp(0x43, 11100000 + (2 << iChannel));
		//read
		state = inp(0x40 + iChannel);

		//make binary …
		for (int i = 7; i >= 0; i--)        //find bits
		{
			temp = state % 2;
			state /= 2;         //fill the screen
			printf("%c",temp + '0');
		}
		printf("\n");
	}
}

void setFrequency(unsigned int freq){
    //Порт 42 – канал 2, связан с динамиком

	long base = 1193180, kd;
	// Устанавливаем режим 3 для второго канала таймера, 10 11 011 0 – канал, старший+младший байты, режим, формат (двоичный)
	outp(0x43, 0xB6); 
	kd = base / freq;
	outp(0x42, kd % 256); 
	kd /= 256; 
	outp(0x42, kd); 
}

void play() {
	int i;

    for (i = 0; melody[i] != 0 || clock[i] != 0; i++) {

		setFrequency(melody[i]);
		// Включаем громкоговоритель. Сигнал от
		// канала 2 таймера теперь будет проходить
		// на вход громкоговорителя.
		outp(0x61, inp(0x61) | 3);
		//Пауза
		delay(clock[i]);
		// Выключаем громкоговоритель.
		outp(0x61, inp(0x61) & 0xfc);

    }
}

int main() {

	int i;
	long range;
	char selection;

    while (1) {
		printf("1: Play sound \n");
		printf("2: Print KD\n");
		printf("3: Print condition words\n");
		printf("4: Generate random numbers\n");
		printf("5: Exit(e)\n\n");
		selection = getch();

		switch (selection) {
			case '1':
				play();
				break;
			case '2':
				system("cls");
				printKd();
				getch();
				break;
			case '3':
				system("cls");
				printWords();
				getch();
				break;
			case '4':
				system("cls");
				printf("Enter the above range of random: ");
				scanf("%ld", &range);
				getRandom(range);
				break;
			case 'e':
				printf("Exiting...");
				return 0;
			default:
				printf("Error key! Please try again\n");
				break;
		}
	}
}

