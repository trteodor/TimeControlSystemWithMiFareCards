# Time control system with MiFare cards and STM

In this project, using MiFare cards, I designed system to check when an user hit a card to sensor, then I showed this information on an HTTP page, which was shared on the local network by STM32 MCU.

While I was creating this project, I had an oppurtunity to met HTTPd protocol and understood how download time from time server(SNTP).

This project was created for educational purposes mainly to learn about Ethernet in STM32

During programming, I used structures, queues, and many new things that I used for the first time. I am satisfied a bit from the project mainly because I had opportunity to use in practive mentioned things. 

When I wrote the code, it was at a point in my life when I was just starting my adventure as a programmer. Therefore, the code quality is very poor, but the effect achieved gives satisfaction, which is why I decided to share the project here.

Video Demo:
https://youtu.be/aZ1AEwbcjIA

Based on STM32 board: NUCLEO-H723ZG

The Aplication files are in ./LwIP/App/...

TopLayer of this application is in - LWIP/App/rc522app.c and http.c, OLED_SSD1306_Task.c 

![Visualisation](https://github.com/trteodor/Nucleo-STM32H723ZG-SimpleETH_ownHTTP/blob/master/WizualizacjaHTTP.PNG)
