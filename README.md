# Otimização Paralela de Algoritmo de Processamento de Imagem com OpenMP
Alexandre Brito Gomes - 11857323

Ana Vitoria Gouvea de Oliveira Freitas - 11370196

Filipe Oliveira - 11219161

Na área de processamento de imagem, diversos algoritmos podem ser aplicados à uma imagem para alterar sua composição geral. Dentre eles, temos algoritmos de aplicação de texturas, que podem ser variadas. Um desses algoritmos é o de Blur, que diminui a incidência das cores, deixando-as menos intensas. Esse algoritmo consiste em alterar a intensidade de RGB (Red Green Blue) de cada pixel da imagem, somando um valor na escala de cores (que vai de 0 a 255). Esse foi o algoritmo escolhido para realizar uma otimização ao paralelizar esse processo.


# Paralelização

Para paralelizar, utilizamos a biblioteca OpenMP, visto que realizamos o algoritmo em C. Optamos por paralelizar a operação de soma de cada pixel de maneira paralela. Para isso, é necessário explicar o funcionamento do código no geral. 

Os códigos estão divididos originalmente em: 
1- Diretório *lib*, que consiste em em funções gerais de funcionamento do código
2 - Um diretório com imagens para teste no formato BMP (Windows Bitmap). Imagens nesse formato possuem um cabeçalho com informações sobre a imagem, como quantos pixels a imagem tem e qual é a cor de cada pixel.  Entretanto,  nesse repositório, devido ao tamanho dos arquivos, as imagens não estão disponibilizadas aqui, mas podem ser acessadas em:  https://drive.google.com/drive/folders/1NNY-GGqsoAAHUhEO1f57H9_BMbL6S3yx?usp=sharing .
3 - Um arquivo Makefile para execução do código juntamente de um arquivo main.c. 

No diretório lib, fizemos uma função que realiza o algoritmo de maneira sequencial, e outra que realiza de maneira paralela, com 4 threads como default, mas esse valor pode ser alterado, explicado posteriormente nas instruções de como rodar o programa.
Podemos observar as diretivas de paralelismo no  trecho do código abaixo. 

```void  parallelBlurImage(BMP header, int  numThreads){
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
Utilizamos o `#pragma  omp  parallel  for  shared(header, kernel) num_threads(numThreads)` para paralelizar o loop de realizar a alteração de cada pixel, mantendo as variáveis header e kernel como shared, uma vez que elas não serão alteradas, e utilizamos a diretiva `#pragma omp simd` para indicar que as operações no loop podem ser vetorizadas. e `reduction(+:sumRed, sumGreen, sumBlue)` para garantir que essas variáveis sejam incrementadas (pois é uma soma) corretamente no final do loop paralelo. 

## Execução

Para executar o código, temos duas opções: 
1 - Caso queira executar o código sequencial, basta seguir o processo padrão de compilação do Makefile, que consiste em `make all` para compilar e `make run` para executar. 
2 - Para o  caso paralelo, basta compilar com `make omp` e `make run` para executar. É possível também passar como parâmetro o número de threads que se deseja executar, passando como parâmetro da seguinte maneira: `make run NUM_THREADS=N`, sendo N o número de threads desejado.
 Após isso, basta digitar o caminho para a imagem desejada, e o código será executado. 

## Experimentação

Quanto à experimentação, é importante destacar o processador utilizado, que foi um AMD Ryzen 5 3500U with Radeon Vega Mobile Gfx     2.10 GHz, que  é um processador de quatro núcleos e oito threads. Com isso, realizamos testes com no máximo 8 threads. Para realização dos testes, observamos que com imagens BMP simples, o código apresentava resultados muito semelhantes no sequencial e no paralelo, com o paralelo chegando a demorar mais em alguns casos. Logo, para isso, geramos imagens muito grandes para poder comparar desempenho de uma maneira melhor. Geramos imagens de 102 MB, 286 MB, 1.11 GB e 2.51GB, que serão chamadas como referência ao decorrer do trabalho respectivamente de img1, img2, img3 e img4.  

Os resultados podem ser visualizados na tabela abaixo. 

|  |  |
|--|--|
|  |  |


|                |Tamanho do arquivo                          |Tempo Sequencial| Tempo paralelo 4 Threads |  Tempo paralelo 6 Threads | Tempo paralelo 7 Threads |Tempo paralelo 8 Threads
|--|--|--|--|--|--|--|
|Img1|`102 MB`            |29.947614s|26.893698s|28.861535s|28.096920s|27.222799s
|Img2|`286 MB`|80.327332s|70.571968s|69.451736s|69.309054s|69.738661s
|Img3|`1.11 GB`|314.285037s| 285.388405s| 288.75784s|281.343241s|279.237382s| 
|Img4|`2.51 GB`|736.409302s|693.592241s| 676.287846s| 691.473114s| 680.071006s

Podemos observar mais visualmente também por meio dos seguintes gráficos a diferença do sequencial em relação ao paralelo. 

![Alt text](/results/result_102mb.png "Versão sequencial x paralela - Imagem 102MB")
![Alt text](/results/result_286mb.png "Versão sequencial x paralela - Imagem 286MB")
![Alt text](/results/result_1gb.png "Versão sequencial x paralela - Imagem 1GB")
![Alt text](/results/result_2gb.png "Versão sequencial x paralela - Imagem 2.51GB")

## Análise
Com a visualização dos resultados, podemos ver que o desempenho sequencial perde em tempo em comparação ao tempo paralelo, apesar da diferença não ser tão significativa, resultando em um speedup de 1.089, utilizando por exemplo os números da Img4 no melhor e pior caso. É importante também notar que o melhor desempenho, no geral, se dá com o uso de 6 threads. 
