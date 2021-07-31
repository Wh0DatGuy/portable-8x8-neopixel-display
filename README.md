# portable-8x8-neopixel-display
The pictures on thingiverse are of a prototype, so there's components soldered around. The new boards are already updated to include those components.
This is a visual equalizer that uses an 8x8 neopixel grid to display 7 audio bands (using the MSGEQ7 chip from sparkfun) while the last column is used to display the battery level.
Inside is an arduino uno with a custom shield, powered by a 18650 battery, with charge and converter board.
Using the buttons on the side it is capable of:

- setting thebrightness of the display
- setting the sensitivity of the chip
- setting the orientation of the display
- writing phrases using the phrase composer menu
- setting the direction of the rainbow effect
- setting the speed of the rainbow effect

libraries used:
- neopixel: https://github.com/adafruit/Adafruit_NeoPixel
- battery: https://github.com/rlogiacco/BatterySense

link of 3d-printed enclosure: https://www.thingiverse.com/thing:4725156

link of gerber files: https://oshwlab.com/Gaming_Dub/visual-eq

links of all the components used:
- MSGEQ7 chip: https://www.sparkfun.com/products/10468
- display: https://www.amazon.it/gp/product/B078HYP681/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1
- boost converter: https://www.amazon.it/gp/product/B07PMRWRBX/ref=ppx_yo_dt_b_asin_image_o04_s00?ie=UTF8&psc=1
- dupont connectors: https://www.amazon.it/gp/product/B07DLT56TM/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1
- female connector: https://www.amazon.it/ARCELI-Straight-Connector-Assortimento-Prototype/dp/B07Q1XBGFB/ref=sr_1_6?__mk_it_IT=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=PCB+3+Pin&qid=1612886450&s=electronics&sr=1-6 (to solder to led matrix)
- female jack: https://www.amazon.it/Haljia-supporto-cuffie-connettore-fissaggio/dp/B07422MCSX/ref=sr_1_7?__mk_it_IT=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=PCB+3+Pin&qid=1612886450&s=electronics&sr=1-7
- female micro-usb: https://www.amazon.it/WayinTop-Adattatore-Convertitore-Alimentazione-Breadboard/dp/B07W13X3TD/ref=sr_1_6?__mk_it_IT=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=9YD38PP5X1I8&dchild=1&keywords=usb+breakout+board&qid=1612886493&sprefix=usb+brea%2Celectronics%2C157&sr=8-6
- battery spring: https://www.amazon.it/NO-LOGO-DAJIE-TANGHUANG-batterie-Universal/dp/B08638GWW5/ref=sr_1_51?__mk_it_IT=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=aa+battery+spring&qid=1612887139&sr=8-51

Will probably get future code updates to implement more functions.
