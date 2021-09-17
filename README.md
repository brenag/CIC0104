# CIC0104 - Trabalho-1
O projeto consiste na implementação de um método de tradução de uma linguagem de
montagem hipotética vista em sala de aula para uma representação de código objeto, que será executado por um simulador. 
O montador deve ser capaz de detectar uma grande variedade de erros léxicos, sintáticos e semânticos e gerar um arquivo de saída ".obj". 
O trabalho foi desenvolvido para a disciplina de Software Básico (Introdução aos Softwares de Sistemas) da Universidade de Brasília.

Descrição do Projeto 
=================
<!--ts-->
   * [Features do montador](#Features)      
      * [Possibilidade de trabalhar com vetores (Diretiva SPACE com operando e operações do tipo LABEL+Número](#p1);
      * [Capacidade de ignorar tabulações e espaços desnecessários em qualquer lugar do código](#p2);
      * [Detecção de declaração de rótulos ausentes](#pre-requisitos);
      * [Detecção de declaração de rótulos repetidos](#local-files) ;
      * [Detecção de diretivas e instruções inválidas](#remote-files) ;
      * [Deteção de instruções com quantidade de operandos incorretas](#multiple-files) ;
      * [Detecção de dois rótulos na mesma linha](#combo1);
      * [Detecção de tokens inválidos](#combo);
      * [Geração de arquivo texto contendo o código objeto do programa montado.](#combo2).
<!--te-->
<!--ts-->
   * [Features do simulador](#Features)      
      * Recebe como entrada a saída do montador (com extensão ".obj");
      * Mostra a linha que está sendo executada, valor do acumulador e contador de programa após cada instrução;
      * Mostra informações fornecidas pela instrução OUTPUT e aguarda o usuário digitar ENTER para continuar a execução.
<!--te-->

Como utilizar o montador
=================
<!--ts-->
      * Para compilar o código do montador, basta digitar "g++ montador.cpp -o montador" e em seguida "./montador myprogram.asm" no Windows Powershell ou outro terminal;
      * Caso a section data do arquivo .asm venha antes da section text, serão gerados 2 códigos objeto, sendo um com o arquivo original e outro com a section data ao final;
      * O arquivo .obj gerado ao final sempre contém o código objeto considerando a section data ao final, para que funcione corretamente com o simulador desenvolvido.
<!--te-->

Como utilizar o simulador
=================
<!--ts-->
      * Para compilar o código do simulador, basta digitar "g++ simulador.cpp -o simulador" e em seguida "./simulador myprogram.obj" no Windows Powershell ou outro terminal;
      * O simulador só funciona corretamente com códigos objetos cuja section data se encontra ao final do código, por isso a saída do montador sempre possui esse formato.
      
<!--te-->
