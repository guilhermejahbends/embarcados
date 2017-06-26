// RBG LED - Automatic Color Cycling
//

//definir variavel pra escolher uma das opções de sequencia
int valor;
printf("1: Fácil \n 2: Médio \n 3: Difícil \n");
scanf("%d",&valor);

int redPin = 11;
int bluePin = 10;
int greenPin = 9;

int redIn = 0;
int greenIn = 1;
int blueIn = 2;

int redVal;
int greenVal;
int blueVal;

void setup()
{
	redVal = 255;
	greenVal = 255;
	blueVal = 255;
	update();
}

// Esta função atualiza as saídas do LED.
void update()
{
	analogWrite(redPin, redVal);
	analogWrite(greenPin, greenVal);
	analogWrite(bluePin, blueVal);
}

// Esta função atualiza uma das variáveis de cores
// ficando cada vez mais claro ou ficando mais fraco.
// Ele também atualiza as saídas e atrasos por 10 milissegundos.
// definir sequência das cores... ainda não sei como fazer...

 switch ( valor )
 {
	 case 1 :
	 
	 break;
	 
	 case 2 :
	 
	 break;
	 
	 case 3 :
	
	 break;
	 

 
 getch();
 return 0;
 }
 

void color_morph(int* value, int get_brighter)
{
	for (int i = 0; i < 255; i++)
	{
		if (get_brighter)
		(*value)--;
		else
		(*value)++;
		
		update();
		delay(10);
	}
}

void loop()
{
	// começa preto (tudo fora)
	color_morph(&redVal,   1); // transition to red
	color_morph(&greenVal, 1); // transition to yellow
	color_morph(&redVal,   0); // transition to green
	color_morph(&blueVal,  1); // transition to aqua
	color_morph(&redVal,   1); // transition to white
	color_morph(&greenVal, 0); // transition to violet
	color_morph(&redVal,   0); // transition to blue
	color_morph(&blueVal,  0); // transition to black (all off)
}
