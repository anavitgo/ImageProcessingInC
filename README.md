# Otimização Paralela de Algoritmo de Processamento de Imagem com OpenMP
**Ana Vitoria Gouvea de Oliveira Freitas - 11370196**\
**Alexandre Brito Gomes - 11857323**\
**Filipe Oliveira - 11219161**

Na área de processamento de imagens, existem vários algoritmos aplicáveis para modificar a aparência global de uma imagem. Dentre eles, destacam-se algoritmos que aplicam texturas, oferecendo uma ampla gama de efeitos. Um exemplo desses algoritmos é o de suavização, que reduz a incidência das cores, resultando em tonalidades menos intensas. Esse método implica na modificação da intensidade dos canais RGB (Red, Green, Blue) de cada pixel na imagem, adicionando um valor à escala de cores, que varia de 0 a 255. Nesse contexto, optou-se por otimizar esse processo por meio da paralelização do algoritmo.


# Paralelização

Para paralelizar, utilizamos a biblioteca OpenMP, visto que essa API contempla códigos em linguagem C. Optamos por paralelizar a operação de convolução da imagem, utilizando um kernel 3x3. Para isso, é necessário explicar o funcionamento do código no geral. 

Os códigos estão divididos originalmente em: 
1. Diretório *lib*, que consiste em funções utilitárias de leitura, escrita e aplicação do algoritmo na imagem
2. Diretório com imagens para teste no formato BMP (Windows Bitmap), uma vez que são imagens com cabeçalho menor e de fácil leitura
3. Arquivo Makefile para compilação, execução e definição de flags necessárias para paralelização do códgio 

No diretório lib, fizemos uma função que realiza o algoritmo de maneira sequencial, e outra que realiza de maneira paralela, sendo 4 a quantidade de threads padrão.

Abaixo, é possível observar a operação de filtragem de imagem conhecida como convolução, utilizando um kernel 3x3. É possível notar que o algoritmo percorre a imagem excluindo a borda de 1 pixel, garantindo que o kernel se ajuste ao redor de cada pixel da imagem. Para cada pixel na imagem, o código realiza uma operação de convolução, multiplicando cada valor do kernel pelo valor correspondente na imagem e somando os resultados para cada canal de cor (vermelho, verde e azul).

```
void  parallelBlurImage(BMP header, int  numThreads){
    float v = 1.0 / 9.0;
	float kernel[3][3]= {{v, v, v},
						{v, v, v},
						{v, v, v}};

	for(int x = 1; x < header.height - 1; x++) {					
		for(int y = 1; y < header.width - 1; y++) {
			float sumRed = 0.0;
			float sumGreen = 0.0;
			float sumBlue = 0.0;
			for(int i = -1; i <= 1; ++i) {
				for(int j = -1; j <= 1; ++j) {	
					sumRed = sumRed + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].red;
					sumGreen = sumGreen + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].green;
					sumBlue = sumBlue + (float)kernel[i + 1][j + 1] * header.colorTable[(x + i) * header.width + (y + j)].blue;
				}
			}
			header.colorTable[(x) * header.width + (y)].red = sumRed;
			header.colorTable[(x) * header.width + (y)].green = sumGreen;
			header.colorTable[(x) * header.width + (y)].blue = sumBlue;
		}
	}

}
```
Utilizamos o `#pragma  omp  parallel  for  shared(header, kernel) num_threads(numThreads)` para paralelizar o loop de alteração de cada pixel, mantendo as variáveis header e kernel como shared, uma vez que elas não serão alteradas, e utilizamos a diretiva `#pragma omp simd` para indicar que as operações no loop podem ser vetorizadas. e `reduction(+:sumRed, sumGreen, sumBlue)` para garantir que essas variáveis sejam incrementadas corretamente no final do loop paralelo. 

## Execução

##### Sequencial

1. `make all` - Compila o código
2. `make run` - Executa o código
Note que o cursor do terminal irá esperar o input do usuário que deve ser o caminho completo da imagem. É possível verificar esse caminho por meio do comando `pwd`.

##### Paralela
1. `make omp` - Compila o código e aciona a flag `USE_OMP`
2. `make run` - Executa o código e na região paralela cria 4 threads. Para criar mais threads que o padrão é possível utilizar `make run NUM_THREADS=<#>`

## Experimentação

Quanto à experimentação, é importante destacar o processador utilizado: AMD Ryzen 5 3500U, Radeon Vega Mobile Gfx 2.10 GHz, que  é um processador de quatro núcleos. Para realização dos testes, observamos que com imagens BMP simples, o tempo de execução apresentava resultados muito semelhantes para a versão sequencial e paralela, sendo que a versão paralela chegava a demorar mais em alguns casos. Logo, para isso, geramos imagens grandes a fim de compararmos o desempenho real de cada versão. Geramos imagens de 102 MB, 286 MB, 1.11 GB e 2.51GB.  

Os resultados obtidos podem ser visualizados na tabela abaixo:


|                |Tamanho do arquivo                          |Tempo Sequencial| Tempo paralelo 4 Threads |  Tempo paralelo 6 Threads | Tempo paralelo 7 Threads |Tempo paralelo 8 Threads
|--|--|--|--|--|--|--|
|Img1|`102 MB`            |29.947614s|26.893698s|28.861535s|28.096920s|27.222799s
|Img2|`286 MB`|80.327332s|70.571968s|69.451736s|69.309054s|69.738661s
|Img3|`1.11 GB`|314.285037s| 285.388405s| 288.75784s|281.343241s|279.237382s| 
|Img4|`2.51 GB`|736.409302s|693.592241s| 676.287846s| 691.473114s| 680.071006s

Podemos observar mais visualmente também por meio dos seguintes gráficos a diferença do sequencial em relação ao paralelo. 

### Versão sequencial x paralela - Imagem 102MB
![Alt text](/results/result_102mb.png )
---

### Versão sequencial x paralela - Imagem 286MB
![Alt text](/results/result_286mb.png )
---

### Versão sequencial x paralela - Imagem 1GB
![Alt text](/results/result_1gb.png )
--- 

### Versão sequencial x paralela - Imagem 2.51GB
![Alt text](/results/result_2gb.png )
---

## Análise
Com a visualização dos resultados, é possível ver que o desempenho sequencial é ligeiramente pior que o paralelo, apesar da diferença não ser tão significativa, resultando em um speedup de 1.089, utilizando por exemplo os números da imagem de 2GB no melhor e pior caso. É importante também notar que o melhor desempenho, no geral, se dá com o uso de 6 threads.
