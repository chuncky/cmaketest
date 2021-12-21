#ifndef _LCD_REV_H_
#define _LCD_REV_H_

/*rule:
version number include 4 numbers: Major.Sub.Minor.Patch
Major: Very importent update. If major version is different, total lcd drivers should be updated.
Sub: Add some importent function. If sub version is different, total lcd drivers should be updated.
Minor: Add some simple function. If minor version is different, files can be updated as needed.
Patch: Fix some bugs. If patch version is different, files can be updated as needed.
*/

#define LCD_DRV_REV_NUM "1.2.0.6"

/*1.0.0.0: original version at 2020.6.22*/
/*1.0.0.1: */
/*1.0.1.0: update pinmux config*/ 
/*1.0.1.1*/
/*1.1.0.0: Add fstn panel support, change panel structure. Must update all files.*/
/*1.1.0.1*/
/*1.1.0.2*/
/*1.1.1.0: Add Landscape support: (LCM must support HW rotation function)*/
/*1.1.1.1*/
/*1.1.1.2*/
/*1.1.1.3*/
/*1.1.2.0: Add Rotation support, remove landscape support,replaced with rotation support*/
/*1.1.2.1*/
/*1.2.0.0: Add dual panel support*/
/*1.2.0.1*/
/*1.2.0.2*/
/*1.2.0.3*/
/*1.2.0.4*/
/*1.2.0.5*/
/*1.2.0.6*/

#endif /*_LCD_REV_H_*/
