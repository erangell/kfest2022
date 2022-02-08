# kfest2022

Von Neumann machine emulator - see DiskImages directory

In college my professor taught us this hypothetical computer that has a memory of 100 decimal numbers, each from 000-999.
There are 10 opcodes - the hundreds digit of each word is interpreted as the opcode if the number is executed.
The tens and ones digits represent a memory location in the system.
This emulator can teach a lot about how machine language works, common bugs, memory management, I/O, etc.
Images of the emulator can be saved as EXECutable text files in Apple DOS 3.3
The startup image is a 3x3 Game Of Life neighbor counting program that uses almost every word of memory.
A Turing Machine emulator was built with the system.  To run it, press 7 to exit, then type:
EXEC VN.TURING

Continuation of my 35 year slackfest:

DiskImages directory has Annunciator 2 MIDI 2021 Update and Edasm assembler environment used to build it.
This system was demonstrated in a Kansasfest 2018 lightning talk and described in Paleotronic magazine.
Requires Apple 2e or 2GS with 128K and 80 columns.  Midi OUT cable is connected to Annunciator #2 of the game socket.

2021 update allows you to use the D key to mute drum track on MIDI channel 9.
(D cycles between display of track with asterisk, no display and drums muted, display MIDI note letters for track).
The MIDI out can then be connected to another machine with a Passport MIDI card running a mockingboard tone generator program for the MIDI IN data.

