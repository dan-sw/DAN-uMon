/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  startup_entry.S
 Purpose    :  First uMON entry point located in TCM
 Owner		:  Sergey Krasnitsky
 Created    :  11.11.2010
\**********************************************************************/

	.section		.SharedResetVector.text, "ax"
	.align	4
	movi	a0, 0x404
	slli	a0, a0, 20
	jx		a0
