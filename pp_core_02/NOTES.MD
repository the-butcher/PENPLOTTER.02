- create Bluetooth logic to determine BT capabilities
- check how many segments can be kept in memory (should be a lot more than the ArduinoR4)

- create Motor code (all 4) to be able to make a valid estimation on how much time a single pulse will consume

- see what happens if code runs longer than a single pulse allows, is there a way to "lock" and skip pulses (or dynamically adapt frequency)

- check if precision allows (entry position will always be exact) precalculation of segment values

  - this would allow for segments to be prepared on the main thread and then be used in the interrupt function, this would keep the potential interrupt run time shorter since it would only have to process pulses and nothing else

- what benefit would adding a display have?

  - bluetooth connection status could be indicated through the neopixel led
    ? available memory
    ? frequency

- would be nice to be able to recalibrate after pen change though
