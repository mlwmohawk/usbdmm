HoldPeak HP-90EPC Meter Reader

There are a few of these Chinese built DMMs on the market under various names 
and vendors that have either an RS-232 or a USB port that can be used for 
reading the data from the meter. There is a project called QtDMM out there that 
may read the meter, but was not available for the Ubuntu version I was using. 
There is also a python project that will read the meter, but doesn’t work for 
python3. Confronted with either wrestling with old Qt code or old Python code, 
I opted for the third choice of writing something quick and dirty to do just 
what I wanted. All I wanted was to record the output of the meter.

The tricky part of this “protocol” is that it is kind of not really a 
protocol, its basically a stream-grab of the display information. Numbers are 
not presented in any sort of useful number format but as the segment bits of a 
seven segment display. You are basically getting the LCD panel display control 
bits out of the serial port. The task is to decode the seven segment display 
information and map the various nomenclature display bits to something usable.

The serial baud rate is 2400 baud. The unit outputs 14 bytes of data a few 
times a second. Each byte of data contains two four bit nibbles. The upper 4 
bits are an index and it is numbered 1-14 (0x1-0xE). The lower four bits are 
the data. There are four display “digits” in bytes 2-3, 4-5, 6-7, and 8-9. 
Each set of two four bit nibbles combine into one unsigned char. The msb is 
either the sign or decimal point. The remaining 7 bits are the segments of the 
digits.  Bytes 1, 10, 11, 12, 13, and 14 each have four bits of information 
about the meaning of the four digits, i.e. ohms, amps, volts, milli, micro, 
etc. 

On most linux variants, this should just compile with make. No special 
libraries or packages should be needed.

I separated the code into a generic “dmm” class and a specific HP-90EPC 
class. Its probably a waste of time for such a simple project, but it could be 
usable for other DMMs if they differ slightly.
