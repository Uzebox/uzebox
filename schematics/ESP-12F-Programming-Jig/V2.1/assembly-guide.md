# A Programming Jig for ESP-12E/F Wi-Fi modules
For PCB revision 1.2+

> [!NOTE]  
> This assembly guide offers many assembly options depending on what you need. For instance, if you want an "universal" programmer that supports both 3.3V and 5V versions of FTDI cables or just using pogo pins vs. a header to connect to the ESP-12 module. The guide will refer to the "***5V version***" as the version that supports both FTDI cable versions and requires all parts. If you only need to supports the "***3.3V version***", many parts can be ommited and the related assembly steps skipped.

## Revision History
| Version | Date | Author | Description |
| :---- | :---- | :---- | :---- |
| 1 | 19-Sep-2024 | A.Bourque | Initial release |


## Parts list verification
> [!IMPORTANT]  
> Ensure you have all the required parts before starting, 

<!--
> [!TIP] 
> Click on the images for a bigger view.
-->
| Component Image | Schematic Reference | Description |
| ----- | ----- | ----- |
| <img src="assets/guide/header_6pos_90deg.jpg" alt="Header" width="250" > | FTDI 		 | Header, 90 degrees 		 |
| <img src="assets/guide/tactile_sw.jpg" alt="Tactile Switche" width="250" >  | ESP_RST, ESP_PROG  | Tactile 	switch (2x)				 |
| <img src="assets/guide/cap_100nF.jpg" alt=".100nF capactito" width="250" >  | C12 | 100nF ceramic capacitor ***(5V version)***	 |
| <img src="assets/guide/cap_1uF.png" alt="1uF Capacitor" width="250" >  | C16,C20  | 1uF electrolytic capacitor (2x) ***(5V version)***			 		 |
| <img src="assets/guide/reg_3v3.png" alt="3.3V Voltage Regulator" width="250" >  | IC1 	 | 3.3V voltage regulator ***(5V version)*** |
| <img src="assets/guide/header_2pos.jpg" alt="Header, 2pos" width="250" >  | N/A 	 | Header, male, 2 positions ***(5V version)***  |
| <img src="assets/guide/jumper.jpg" alt="Jumper" width="250" >  | N/A 	 | Jumper ***(5V version)***  |
| <img src="assets/guide/pogo_pin.jpg" alt="pogo pin/header" width="250" > <img src="assets/guide/header_fem_8pos_2mm.jpg" alt="pogo pin" width="250" >  | N/A 	 | Spring loaded contact (aka pogo pin) (8x) <br/>* OR *<br/>  8 position female header, 2mm pitch (2x) |
| <img src="assets/guide/esp-jig-cad-model.jpg" alt="3D printed enclosure" width="250" > | N/A 	 | [3D printed enclosure](https://github.com/Uzebox/uzebox/tree/master/cad/Enclosures/ESP-12F-Programming-Jig/V1.2) to hold the ESP-12 module in place when using pogo pins. |

## Tools required

To assemble this kit you will need the following tools:

| Tool | Description |
| :---: | ----- |
| <img src="assets/guide/iron.png" alt="Soldring Iron" width="150" > | A basic soldering iron, 25W-40W. |
| <img src="assets/guide/solder.png" alt="Solder" width="150" >  | Solder, rosin core, 60/40 type. |
| <img src="assets/guide/pliers.png" alt="Long Nose Plyers" width="150" >  | Long nose pliers. |
| <img src="assets/guide/cutters.png" alt="Shear Cutters" width="150" >  | Regular cutters will do fine, but shear cutters will do a better job. |
| <img src="assets/guide/FTDI-cable.jpg" alt="FTDI cable" width="150" >  | 3.3V or 5V FTDI USB-to-Serial cable (TTL-234X-xxx).|



## Building the kit

<table>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-1.jpg" alt="" width="600"></td>
        <td>Solder the 6-pin header.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-2b.jpg" alt="" width="600"></td>
        <td>Solder the pogo pins in the illustrated positions (pins TXD, RXD, GPIO0, GPIO15, GND, VCC, CH_PD, RESET). Alternatively, solder the two 8 pin headers.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-3.jpg" alt="" width="600"></td>
        <td>Solder the tactile switches.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-4.jpg" alt="" width="600"></td>
        <td>If you build the <b><i>3.3V version</i></b>, make a jumper with a piece of bare wire as illustrated.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-5.jpg" alt="" width="600"></td>
        <td>Solder it in the illutrated place to select 3.3V.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-6.jpg" alt="" width="600"></td>
        <td>For the <b><i>5V version</i></b>, add the remaining parts: the voltage regulator, capacitors and jumper header.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-8.jpg" alt="" width="600"></td>
        <td>Versions with headers and pogo pins assembled.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-9.jpg" alt="" width="600"></td>
        <td>If using headers, you must mount 2mm pitch male headers on the ESP-12 module as illustrated.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-10.jpg" alt="" width="600"></td>
        <td>Then it can be socketed on the jig for programming.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-12.jpg" alt="" width="600"></td>
        <td>If using pogo pins, insert the jig into the 3D printed enclosure and insert the ESP-12 module.</td>
    </tr>
    <tr>
        <td width="50%"><img src="assets/guide/assembly-14.jpg" alt="" width="600"></td>
        <td>Close and connect the FTDI cable and you are ready to upgrade the firmware.</td>
    </tr>        
</table>

## Programming
The upgrading of the ESP-12 to the latest firmware can be done using Espressif's ESP tools. See the wiki for [the procedure](https://uzebox.org/wiki/ESP8266_Manual_Upgrade#New_method_with_ESP_tools).

## 3D enclosure

See: https://github.com/Uzebox/uzebox/tree/master/cad/Enclosures/ESP-12F-Programming-Jig/V1.2

<br/>
<img src="assets/guide/cc-bysa.png" alt="Creative Commons BY-SA">

Pictures and content of this document are released under a [Creative Commons Attribution-Share Alike 3.0 Unported License](http://creativecommons.org/licenses/by-sa/3.0/).  
Uzebox is a reserved trademark.  
Copyright © Belogic   