# 8 Segment Numeric DIsplay

A 10W/digit numeric display to show numbers in any room.

![AssembledView](Documentation/AssembledView.jpg)

[Interactive BOM for LED Display](https://htmlpreview.github.io/?https://raw.githubusercontent.com/wszeto9/8-Segment-Numeric-Display/main/Documentation/ibom_LED_Panel.html)

[Interactive BOM for LED Display Controller](https://htmlpreview.github.io/?https://raw.githubusercontent.com/wszeto9/8-Segment-Numeric-Display/main/Documentation/ibom_LED_Panel_Controller.html)

[Schematic for LED Display](https://github.com/wszeto9/8-Segment-Numeric-Display/blob/main/Documentation/LED_Panel_Sch.pdf)

[Schematic for LED Display Controller](https://github.com/wszeto9/8-Segment-Numeric-Display/blob/main/Documentation/LED_Panel_Controller_Sch.pdf)


The display is a 65mm x 102mm single layer aluminum PCB. It has a 1x20 SMT male header in order to connect a driver board. The display requires active cooling for duty cycles above 50%. 

The driver is a 75mm x 32mm double layer FR4 PCB. It relies on a 74HC595 shift register to drive a chain of displays using a single data stream. The driving microcontroller can send 3.3V or 5V logic. The logic level is set by the reference voltage on the input. The driver has an onboard buck converter to create the drive voltage of the display. The board can receive 7-16VDC (Peak current draw is 10W/Vin). 
 
# Driving Logic

- Pull nCLR high in order to prevent an accidential clear
- Use SRCLK and SER to send a data packet to the first number's data packet on the right (with LSB first)
- Repeat sending data until all numbers are sent.
- Pulse RCLK to push shifted bits into the outputs
- Use PWM on nOE to adjust the display brightness. Note: The brightness will lower with higher duty cycles.

|Number to display| Data packet|
|--|--|
|0|0b10111011|
|1|0b00000011|
|2|0b00111010|
|3|0b00011110|
|4|0b01000110|
|5|0b01011100|
|6|0b00101110|
|7|0b00110010|
|8|0b01111110|
|9|0b01110010|

# Ordering the LED Panel Parts

From Digikey: quantities are for 1x LED panel.

1x S1113E-20-ND

In addition,

**Red:**

- 20x 90-JE2835ARD-N-0001A0000-N0000001TR-ND

- 10x 118-CRM2512-JW-6R8ELFCT-ND

**Blue:**

- 20x  90-JE2835ABL-N-0005A0000-N0000001CT-ND

- 10x RMCF2512JT18R0CT-ND

**Yellow:**

- 20x 90-JE2835APA-N-0001A0000-N0000001CT-ND
- 10x RMCF2512JT10R0CT-ND

**Green:**
- 20x 90-JE2835AGR-N-0002A0000-N0000001CT-ND
- 10x RMCF2512JT18R0CT-ND

# Ordering the LED Panel Controller Parts

**Digikey Parts**
|Quantity|Digikey PN| Description|
|--|--|--|
|1|732-8632-1-ND|120uF/25V Capacitor|
|1|732-8798-1-ND|330uF/16V Capacitor|
|1|F3375CT-ND|2A Polyfuse|
|1|455-1849-ND|JST PA 1x04 RA Housing|
|1|455-2753-ND|JST PA 1x04 Connector|
|4|455-1325-1-ND|JST PA Crimp|
|1|1292-WR08X000PTLCT-ND|0805 Jumper|

**Amazon Parts**

- 1x [1x20 female pin socket](https://www.amazon.com/Yohii-Female-Header-2-54mm-Connector/dp/B07P1R9CGT/)

- 1x [LM2596 Buck Converter](https://www.amazon.com/gp/product/B0B63GQDJW/)