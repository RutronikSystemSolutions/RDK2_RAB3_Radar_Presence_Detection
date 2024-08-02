# RDK2 RAB3-Radar Presence Detection and GUI 

This code example demonstrates digital signal processing implemented on a PSoC62 for use with a BGT60TR13C radar sensor from Infineon.

<img src="pictures/rdk2_rab3.png" style="zoom:25%;" />

## Requirements

- [ModusToolbox® software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/) **v3.x** [built with **v3.1**]
- [RAB3-Radar](https://www.rutronik24.com/product/rutronik/rab3radar/23169671.html)
- [RDK2](https://www.rutronik24.fr/produit/rutronik/rdk2/16440182.html)

## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v11.3.1 (`GCC_ARM`) - Default value of `TOOLCHAIN`

## Using the code example with a ModusToolbox™ IDE:

The example can be directly imported inside Modus Toolbox by doing:
1) File -> New -> Modus Toolbox Application
2) PSoC 6 BSPs -> RDK2
3) Sensing -> RDK2 RAB3 Presence Detection

A new project will be created inside your workspace.

## Operation

Plug a USB cable into the Kit Prog3 USB connector.
You then have 2 possibilities:
1) Use with a custom GUI (the C# Visual Studio project is located inside the directory gui/src).

In that case, you should have this defined inside the main.c file:

```
#define OUTPUT_FOR_VISUALISATION_GUI
```

2) Use with a standard terminal (like RealTerm) to see the debug messages directly inside the terminal.

In that case, you should have this line inside the main.c file:

```
#undef OUTPUT_FOR_VISUALISATION_GUI
```

## Change the radar configuration
You can change the radar configuration used to measure by generating a new "radar_settings.h" configuration.

Use the Infineon “Radar Fusion GUI” tool to generate a new version of the file.

## Libraries

The project contains a local copy of the sensor-xensiv-bgt60trxx.
Modifications have been made inside the file xensiv_bgt60trxx_mtb.c to detect timeout during SPI transfers.

## Legal Disclaimer

The evaluation board including the software is for testing purposes only and, because it has limited functions and limited resilience, is not suitable for permanent use under real conditions. If the evaluation board is nevertheless used under real conditions, this is done at one’s responsibility; any liability of Rutronik is insofar excluded. 

<img src="pictures/rutronik.png" style="zoom:50%;" />



