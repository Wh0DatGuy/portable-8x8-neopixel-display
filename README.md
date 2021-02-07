# portable-8x8-neopixel-display
This is a visual equalizer that uses an 8x8 neopixel grid to display 7 audio bands (using the MSGEQ7 chip from sparkfun) while the last column is used to display the battery level.
Inside is an arduino uno with a custom shield, powered by a 18650 battery, with charge and converter board.
Using the buttons on the side it is capable of:

setting thebrightness of the display
setting the sensitivity of the chip
setting the orientation of the display
writing phrases using the phrase composer menu
setting the direction of the rainbow effect
setting the speed of the rainbow effect

libraries used:
- neopixel: https://github.com/adafruit/Adafruit_NeoPixel
- battery: https://github.com/rlogiacco/BatterySense

link of 3d-printed enclosure: https://www.thingiverse.com/thing:4725156

link of gerber files: https://oshwlab.com/Gaming_Dub/visual-eq

links of all the components used:
- display: https://www.amazon.it/gp/product/B078HYP681/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1
- boost converter: https://www.amazon.it/gp/product/B07PMRWRBX/ref=ppx_yo_dt_b_asin_image_o04_s00?ie=UTF8&psc=1
- dupont connectors: https://www.amazon.it/gp/product/B07DLT56TM/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1

Will probably get future code updates to implement more functions.
