1. Connect rpi and PC by jumpers
   2. note: RX and TX must be crossed
3. Connect to /dev/tty.xxx
   4. device: e.g., /dev/tty.SLAB_USBtoUART, /dev/tty.usbserial-0001
   5. how to connect: e.g,
      6. `sudo screen /dev/tty.usbserial-0001 115200`
      7. or use applications like SerialTools
