### Programa de esteganografia
### Aluno: Matheus Henrique Rodrigues Magalhães

## Estrutura de arquvos

- O programa é divido em 3 arquivos 
```
	main.c
	steg.c
	steg.h
```
o programa main.c é responsável por ler os comandos passados pelo terminal pelo usuario para execução do programa, o arquivos steg.c e steg.h são responsáveis por conter a implementação das funções necessárias e o cabeçalho e protótipo de funções, respectivamente. 

## Compilando 

- Para compilar o programa é necessário ter a bilbioteca libpng que pode ser instalada com o gerenciador de pacotes da sua distribuilção GNU/Linux. 

```bash
gcc -o steg main.c steg.c steg.h -lpng
```

- Depois de compilado o programa pode ser executado invocando o seguinte comando no terminal.

```bash
./steg -enc tux.png msg.txt out.png
```
- O comando '-enc' determina que iremos utilizar o programa para codificar uma determinada mensagem dentor da imagem 'tux.png', a mensagem pode tanto estar contida em um arquivo de texto quanto passada como uma string diretamente, porém, é recomendado que se use um arquivo de texto para evitar erros na leitura de espaços e quebras de linha.

- O último argumento, se omitido, não inlfuencia no funcionamento do programa, visto que, no caso em que não se determine um arquivo de destino o próprio programa criará um arquivo com nomenclatura padrão de 'out.png' como resultado da imagem esteganografada.

- Para leitura da mensagem codificada em uma imagem pode se utilizar os comandos:
```bash
./steg -dec out.png
```
- O argumento '-dec' determina que estamos utilizando o programa em modo de decodificacao, e o proximo é o nome do arquvio de imagem o qual queremos desesteganografar.

- Podemos utilizar também um terceiro argumento opcional caso queiramos extrair o texto decodificado para um arquivo txt, onde indicaremos o nome do arquivo a ser criado
