# 🚌 Localiza Bus
<p align="center">
  <img src="https://github.com/CodeByBreno/LocalizaBus-Eletronica/assets/132024181/d488d790-96cc-49bf-9290-99621e1a6807" width="800" alt="logoLocalizaBus">
</p>
O Localiza Bus foi um projeto desenvolvido em grupo para disciplina de Instrumentação Eletrônica, no final do ano de 2023. Ele consiste em um Sistema Embarcado junto à uma Aplicação Web e
Mobile, voltados para visualização da localização em tempo real dos ônibus da UNIVASF (e qualquer outro veículo)

# 💻 Estrutura do Projeto
Ele é formado por um Sistema Embarcado, composto por um ESP32 que se comunica com um módulo GPS NEO-6M, para obter os dados de localização. Além disso, uma combinação de LEDs são
usados para sinalizar o estado da conexão e acesso à localização do dispositivo. Houve ainda um protótipo de como seria o sistema de alimentação, com um módulo controlador que
alterna a alimentação entre uma bateria de lítio com 3.7v (recarregável) e a entrada USB do ônibus
Na parte do software, foi subida uma aplicação para um servidor no Digital Ocean, a fim de prover os dados enviados no formato JSON do sistema embarcado. A aplicação mobile,
desenvolvida em Flutter, lê os dados no servidor e utiliza-os junto à API do Google Maps para exibir a localização atual do dispositivo do mapa.

Devido à problemas com o módulo GPRS (conexão por dados móveis autônoma), optamos por utilizar os dados móveis roteados a partir de algum celular para transmitir os dados
do ESP32 para o servidor. A conexão, até o momento final do projeto, é estática (em código).

<strong>Diagrama do Projeto</strong>
<p align="center">
  <img src="https://github.com/CodeByBreno/LocalizaBus-Eletronica/assets/132024181/1e6e70cd-a3fa-4c3f-b3a0-8f34d813d1a5" width="400" alt="diagramaProjeto">
</p>

# ⚙️ Como Executar o Projeto?

As conexões feitas no circuito estão apresentadas na imagem abaixo:

<p align="center">
  <img src="https://github.com/CodeByBreno/LocalizaBus-Eletronica/assets/132024181/2e51e252-8a0d-48f2-890e-0aaf16fb7ce5" width="800" alt="circuitoProtoboard">
</p>

Feito isso, basta upar o código desse repositório para o ESP32 através da ArduinoIDE. Lembre de instalar todas as bibiotecas devidas (incluidas no projeto)

# 📊 Para mais informações do projeto, clique na imagem abaixo:

<p align="center">
  <a href="https://www.canva.com/design/DAF3bAhzBYQ/sVweig8vEqSc_hNl8vjuJA/view?utm_content=DAF3bAhzBYQ&utm_campaign=designshare&utm_medium=link&utm_source=editor">
    <img src="https://github.com/CodeByBreno/LocalizaBus-Eletronica/assets/132024181/092982b4-c518-4fb6-aa2a-691ba34c560a" width="800" alt="capaSlides">
    <h3 align="center">Apresentação do Projeto - Slides</h3>
  </a>
</p>



