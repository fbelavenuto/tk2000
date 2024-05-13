# TK2000 microcomputer emulator

Emulates a TK2000 computer (Clone MPF-II)

This emulator was created to test my learning in C++. The required level is C++17.

I used the SDL2 graphics library to render the image and generate sound. For configuration I used a simple menu system rendering texts with a rasterized font on top of the TK2000 video itself.

# Usage

Download the latest release and unzip it to a folder, no installation required. To use SDL2 it is necessary that some libraries (supplied) are included with the binary.

Run the binary. To call up the menu, press F2 on the keyboard and use the up and down arrow keys to navigate. Use the ENTER key to accept the selected menu.

# Development

I developed the emulator in MS Visual Studio Community 2022 (MSVC22). Make a clone of the repository and open the root directory in MSVC22 to be able to work on the sources.

# Known issues

There is a problem with sound generation, there may be some glitches during sound playback. This problem remains unresolved for now.



# Portuguese

Emula um computador TK2000 (Clone MPF-II)

Este emulador foi criado para testar meu aprendizado na linha C++. O nível requerido é o C++17.

Usei a biblioteca gráfica SDL2 para fazer a renderização da imagem e geração de som. Para a configuração utilizei um sistema de menu simples renderizando textos com uma fonte rasterizada em cima do próprio vídeo do TK2000.

# Uso

Baixe o último release e descompacte em alguma pasta, não é necessário instalação. Por usar o SDL2 é necessário que algumas bibliotecas (fornecido) estejam junto ao binário.

Execute o binário. Para chamar o menu aperte F2 no teclado e use as setas direcionais para cima e para baixo para navegar. Utilize a tecla ENTER para aceitar o menu selecionado.

# Desenvolvimento

Desenvolvi o emulador no MS Visual Studio Community 2022 (MSVC22). Faça um clone do repositório e abra o diretório root no MSVC22 para poder trabalhar nos fontes.

# Problemas conhecidos

Há um problema com a geração do som, pode haver alguns glitches durante a reprodução do som. Este problema continua sem solução por enquanto.

