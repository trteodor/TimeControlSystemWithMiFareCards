# NUCLEO-H723ZG ETH Own Simple HTTP
Hi! 
Here i test HTTPd protocol. On my site i driving LEDs and Viewing actual time in microcontroller 
downloaded from SNTP. I also Control Worker time with RC522

It is the most advanced application I have created so far. During programming, I used structures, queues, and many new things that I used for the first time. I am a bit satisfied with the quality of the code created.


I created this application to improve my skills programming

Video Demo:
https://youtu.be/aZ1AEwbcjIA


Because i can't correctly configure SNTP implemented in LwIP library. I implemented my own SNTP protocol.

The Aplication files are in ./LwIP/App/... <-here
TopLayer is in - LWIP/App/rc522.c and http.c, OLED_SSD1306_Task.c 

![Visualisation](https://github.com/trteodor/Nucleo-STM32H723ZG-SimpleETH_ownHTTP/blob/master/WizualizacjaHTTP.PNG)
