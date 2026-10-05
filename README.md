# MP7 Minimal Viable HCB
Creating a quick solution using a nucleo so that we can run hybrid test days and so the senior design team can do some traction control testing before hardware arrives.

## Rev1 - Single core implementation
### cubemx configure
 - running both canfd peripherals on the M7 core
    - Sevcon requires CAN Open, and it has a handful of ID ranges to avoid, and our other codebases for the pedal box board and the throttle controller are not currently setup to avoid them or implement workarounds. because of this, it will get its own bus for now
 - CANFD Params
    - Basic Parameters
        - Frame Format: Classic Mode
        - Mode: Normal Mode
        - Auto Retransmission: Enabled
        - Transmit Pause: Disable
        - Protocol Exception: Disable
        - Nominal Sink Jump Width: 6
        - Data Prescaler: 1
        - Data Sync Jump Width: 1
        - Data Time Seg1: 1
        - Data Time Seg2: 1
        - Message Ram Offset: 0
        - Std Filters Nbr: 1
        - Ext Filters Nbr: 0
        - TX FIFO Queue Mode: FIFO
    - Bit Timings Parameters
        - Nominal Prescaler: 1
        - Nominal Time Seg 1: 33
        - Nominal Time Seg 2: 6
    - Values from here: https://kvaser.com/support/calculators/can-fd-bit-timing-calculator/?v=0.  12&nbr=1000000&dbr=1000000&nsjw=6&dtps1=20&dtps2=19&f=40000&t=50&d=250&np=1&dp=1
 - 