Program is designed for microcontroled ADuC831. The microcontroler guards correct fluid level in a tank using hysteresis.
With a start of the program there is a need to pass three informations via UART: a height of a tank, an upper desired fluid level and a lower desired fluid level.
Then the main loop starts. It checks the current fluid level via analog-digital converter. If the current fluid level falls below the lower desired
fluid level, a pump is powered by the PWM from the ADuC831 until the current fluid level reaches the upper desired fluid level. The current fluid level
is shown on LCD display.